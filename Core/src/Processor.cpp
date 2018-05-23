#include "Processor.hpp"

// STD
#include <fstream>
#include <ios>
#include <map>
#include <thread>
#include <sstream>

// LOCAL
#include "Debug.hpp"
#include "BinaryDecoding.hpp"
#include "PacketTreeManager.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Processor::Processor(
	std::unique_ptr<const Config> config
) :
	m_config(std::move(config)),
	m_eventBuffer(m_config->getTDCList())
{
	// Technically redundant assertion
	// m_eventBuffer initialization will cause seg fault if m_config == nullptr
	// Kept in case of future modifications moving the m_eventBuffer initilization
	ASSERT(nullptr == config);

	// Print config settings
	m_config->print();

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
	initializePacketBuffers(m_config->getTDCList());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Processor::~Processor() {
	ErrorSpy::getInstance().print();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::processFiles(
	const std::string outputFile,
	const std::vector<std::string> fileNames
) {
	const auto mode = m_config->getRunMode();
	if (RunMode::QuickCheck == mode) {
		runQuickCheck(fileNames);
	} else if (RunMode::LowLevel == mode) {
		runLowLevel(outputFile,fileNames);
	} else if (RunMode::Parallel == mode) {
		runParallel(outputFile,fileNames);
	} else {
		// Run Serial by default
		runSerial(outputFile,fileNames);
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
					<< "Board: " << bundle->getReadoutBoardID() << " "
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
	const std::string outputFile,
	const std::vector<std::string>& fileNames
) {
	STD_LOG("Mode: LowLevel");

	// Declare Output Manager
	std::unique_ptr<PacketTreeManager> manager =
		std::make_unique<PacketTreeManager>(m_config,outputFile.c_str());

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
	const std::string outputFile,
	const std::vector<std::string>& fileNames
) {
	STD_LOG("Mode: Serial");

	std::unique_ptr<EventTreeManager> manager =
		std::make_unique<EventTreeManager>(m_config,outputFile.c_str(),m_config->getTDCList().size());

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

			// Write Events
			writeEvents(manager);
		}
	}

	// Write output tree
	manager->writeTree();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::runParallel(
	const std::string outputFile,
	const std::vector<std::string>& fileNames
) {
	STD_LOG("Mode: Parallel");

	const auto maxTasks = std::thread::hardware_concurrency() - 1;
	if (0 == maxTasks) {
		STD_ERR("Only single core is available. Please run in serial mode instead.");
		return;
	}
	STD_LOG("NCores Detected: " << maxTasks + 1);

	// INCOMPLETE
	// std::unique_ptr<EventTreeManager> manager =
	// 	std::make_unique<EventTreeManager>(outputFile.c_str(),m_tdcIDs.size());
  //
	// std::atomic_bool isComplete { false };
	// while (!isComplete) {
	// 	// If a complete event is present, schedule a write task
	// 	if (m_eventBuffer.isCompleteStored()) {
	// 		writeEvents();
	// 	}
  //
	// 	// Schedule make events
  //
	// 	// If Bundle Buffer is not empty, schedule packet making
  //
	// 	//
  //
  //
	// }

	// Write output tree
	// manager->writeTree();
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
							STD_ERR("Exception: " << e.what() << ". Index " << index);
						}
					} else {
						// If packet is not good, delete it
						currPacket.reset(nullptr);
					}
					ASSERT(nullptr == currPacket);
					currPacket = std::make_unique<Packet>(bundle->getReadoutBoardID(),bundle->getROCValue(),word);
					ASSERT(currPacket != nullptr);
				} else if (currPacket != nullptr) {
					if (3 == dataType) {
						ASSERT(currPacket != nullptr);
						currPacket->addTrailer(word);
					} else if (4 == dataType || 5 == dataType) {
						ASSERT(currPacket != nullptr);
						currPacket->addDataline(word);
					} else {
						std::stringstream errorMessage;
						errorMessage << "Invalid Datatype " << dataType;
						ErrorSpy::getInstance().logError(errorMessage.str(),currPacket->getReadoutBoardID(),currPacket->getTDCID());
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
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::writeEvents(
	std::unique_ptr<EventTreeManager>& manager
) {
	while (m_eventBuffer.isCompleteStored()) {
		auto events = m_eventBuffer.popToComplete();
		for (auto& event : events) {
			ASSERT(nullptr != event);
			manager->add(std::move(event));
			ASSERT(nullptr == event);
		}
	}
}
