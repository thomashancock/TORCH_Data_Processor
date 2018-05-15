#include "Processor.hpp"

// STD
#include <fstream>
#include <ios>
#include <map>
#include <thread>

// LOCAL
#include "Debug.hpp"
#include "BinaryDecoding.hpp"
#include "EventTreeManager.hpp"
#include "PacketTreeManager.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Processor::Processor(
	std::unique_ptr<const Config> config
) :
	m_mode(config->getRunMode()),
	m_tdcIDs(config->getTDCList()),
	m_eventBuffer(m_tdcIDs)
{
	// Print config settings
	ASSERT(nullptr != config);
	config->print();

	// Initialise Word Bundle Buffers
	for (auto& ptr : m_wordBundleBuffers) {
		ptr = std::make_shared<bundleBuffer>();
	}

	// Initialise file reader
	m_fileReader = std::make_unique<FileReader>(1,m_wordBundleBuffers);
	ASSERT(nullptr != m_fileReader);

	// After FileReader initialization, should be two pointers refering to the bundle buffers
	for (const auto& ptr : m_wordBundleBuffers) {
		ASSERT(nullptr != ptr);
		ASSERT(2 == ptr.use_count());
	}

	// Initialise Packet Buffers
	initializePacketBuffers(m_tdcIDs);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::processFiles(
	const std::vector<std::string> fileNames
) {
	if (RunMode::QuickCheck == m_mode) {
		runQuickCheck(fileNames);
	} else if (RunMode::LowLevel == m_mode) {
		runLowLevel(fileNames);
	} else if (RunMode::Parallel == m_mode) {
		runParallel(fileNames);
	} else {
		// Run Serial by default
		runSerial(fileNames);
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Processor::initializePacketBuffers(
	const std::list<unsigned int>& tdcIDs
) {
	for (const auto& id : tdcIDs) {
		// Initialise Packet Buffer
		// ThreadSafeQueue takes no constructor arguments, so can construct simply by accessing the entry
		m_packetBuffers[id];
	}
	ASSERT(m_packetBuffers.size() == tdcIDs.size());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::runQuickCheck(
	const std::vector<std::string>& fileNames
) {
	STD_LOG("Mode: QuickCheck");

	int bundleCount = 0;
	for (auto& file : fileNames) {
		ASSERT(nullptr != m_fileReader);
		m_fileReader->stageFiles(std::vector<std::string>{ file });

		while (!m_fileReader->haveFilesExpired()) {
			m_fileReader->runProcessingLoops(1);

			for (auto& buffer : m_wordBundleBuffers) {
				while(!buffer->empty()) {
					bundleCount += 1;
					auto bundle = buffer->popFront();
					std::cout << "New Bundle = "
					<< "Board: " << bundle->getReadoutBoardNumber() << " "
					<< "ROC: " << bundle->getROCValue() << std::endl;
					while (!bundle->empty()) {
						bindec::printWord(bundle->getNextWord());
					}
				}
			}
		}
	}

	std::cout << bundleCount << " bundles in total" << std::endl;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::runLowLevel(
	const std::vector<std::string>& fileNames
) {
	STD_LOG("Mode: LowLevel");

	// Declare Output Manager
	std::unique_ptr<PacketTreeManager> manager =
		std::make_unique<PacketTreeManager>("Output.root");

	// Loop through files
	for (auto& file : fileNames) {
		// Stage file for reading
		ASSERT(nullptr != m_fileReader);
		m_fileReader->stageFiles(std::vector<std::string>{ file });

		// While file is good
		while (!m_fileReader->haveFilesExpired()) {
			// Run a processing loop
			m_fileReader->runProcessingLoops(1);

			// Make packets from the bundles produced by FileReader
			for (auto& buffer : m_wordBundleBuffers) {
				makePackets(buffer);
			}

			// Pass packets to output manager
			for (auto& entry : m_packetBuffers) {
				while(!entry.second.empty()) {
					manager->add(std::move(entry.second.popFront()));
				}
			}
		}
	}

	// Write Output
	manager->writeTree();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::runSerial(
	const std::vector<std::string>& fileNames
) {
	STD_LOG("Mode: Serial");

	std::unique_ptr<EventTreeManager> manager =
		std::make_unique<EventTreeManager>("Output.root",m_tdcIDs.size());

	// Loop through files
	for (auto& file : fileNames) {
		// Stage file for reading
		ASSERT(nullptr != m_fileReader);
		m_fileReader->stageFiles(std::vector<std::string>{ file });

		// While file is good
		while (!m_fileReader->haveFilesExpired()) {
			// Run a processing loop
			m_fileReader->runProcessingLoops(1);

			// Make packets from the bundles produced by FileReader
			for (auto& buffer : m_wordBundleBuffers) {
				makePackets(buffer);
			}

			// Make Events
			makeEvents();

			// Pass events to output manager
			while (m_eventBuffer.isCompleteStored()) {
				auto events = m_eventBuffer.popToComplete();
				for (auto& event : events) {
					ASSERT(nullptr != event);
					manager->add(std::move(event));
					ASSERT(nullptr == event);
				}
			}
		}
	}

	// Write output tree
	manager->writeTree();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::runParallel(
	const std::vector<std::string>& fileNames
) {
	STD_LOG("Mode: Parallel");

	const auto nCores = std::thread::hardware_concurrency();
	std::cout << nCores << " Cores Detected" << std::endl;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::makePackets(
	std::shared_ptr<bundleBuffer> wordBundleBuffer
) {
	// Pointer to store current packet
	std::unique_ptr<Packet> currPacket = { nullptr };

	// Loop through bundles in buffer
	while (!wordBundleBuffer->empty()) {
		auto bundle = wordBundleBuffer->popFront();
		ASSERT(bundle != nullptr);

		if (bundle->isComplete()) {
			while (!bundle->empty()) {
				const auto word = bundle->getNextWord();
				const auto dataType = bindec::getDataType(word);
				if (2 == dataType) {
					if (currPacket != nullptr && currPacket->isGood()) {
						const auto index = currPacket->getTDCID();
						try {
							m_packetBuffers.at(index).push(std::move(currPacket));
						} catch (std::exception& e) {
							STD_ERR("Exception: " << e.what());
						}
					} else {
						// If packet is not good, delete it
						currPacket.reset(nullptr);
					}
					ASSERT(nullptr == currPacket);
					currPacket = std::make_unique<Packet>(bundle->getROCValue());
					ASSERT(currPacket != nullptr);
					currPacket->addHeader(word);
				} else if (currPacket != nullptr) {
					if (3 == dataType) {
						ASSERT(currPacket != nullptr);
						currPacket->addTrailer(word);
					} else if (4 == dataType || 5 == dataType) {
						ASSERT(currPacket != nullptr);
						currPacket->addDataline(word);
					} else {
						STD_ERR("Invalid Data Type found: " << dataType);
					}
				}
			}
		}
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::makeEvents() {
	// Create list of packet buffers
	std::list<unsigned int> bufferIndexes;
	ASSERT(bufferIndexes.empty());
	for (const auto& entry : m_packetBuffers) {
		bufferIndexes.push_back(entry.first);
	}

	while (!bufferIndexes.empty()) {
		// Loop through buffers
		for (auto it = bufferIndexes.cbegin(); it != bufferIndexes.cend(); /* no increment */) {
			if (m_packetBuffers.at(*it).empty()) {
				// If buffer is empty, remove from index list
				bufferIndexes.erase(it++);
			} else {
				// Extract front packet, add to event buffer
				m_eventBuffer.addPacket(std::move(m_packetBuffers.at(*it).popFront()));
			}
		}
	}
}
