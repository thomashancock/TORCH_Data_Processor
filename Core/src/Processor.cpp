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
	m_wordBundleBuffers = std::make_shared< std::array< bundleBuffer, 4> >();
	ASSERT(nullptr != m_wordBundleBuffers);

	// Initialise file reader
	m_fileReader = std::make_unique<FileReader>(1,m_wordBundleBuffers);
	ASSERT(nullptr != m_fileReader);

	// After FileReader initialization, should be two pointers refering to the bundle buffers
	ASSERT(2 == m_wordBundleBuffers.use_count());

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
		STD_LOG("Mode: QuickCheck");
		int bundleCount = 0;

		for (auto& file : fileNames) {
			ASSERT(nullptr != m_fileReader);
			m_fileReader->stageFiles(std::vector<std::string>{ file });

			while (!m_fileReader->haveFilesExpired()) {
				m_fileReader->runProcessingLoops(1);

				for (auto& buffer : *m_wordBundleBuffers.get()) {
					while(!buffer.empty()) {
						bundleCount += 1;
						auto bundle = buffer.popFront();
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

	} else if (RunMode::LowLevel == m_mode) {
		STD_LOG("Mode: LowLevel");

		std::unique_ptr<PacketTreeManager> manager =
			std::make_unique<PacketTreeManager>("Output.root");

		// for (auto& file : fileNames) {
		// 	// Read File into WordBundle buffer
		// 	processFile(file);
    //
		// 	// makePackets();
    //   //
		// 	// for (auto& entry : m_packetBuffers) {
		// 	// 	// std::cout << "TDC ID: " << entry.first << std::endl;
		// 	// 	while(!entry.second.empty()) {
		// 	// 		// const auto packet = entry.second.popFront();
		// 	// 		manager->add(std::move(entry.second.popFront()));
		// 	// 	}
		// 	// }
		// }

		manager->writeTree();

	} else if (RunMode::Parallel == m_mode) {
		STD_LOG("Mode: Parallel");

		const auto nCores = std::thread::hardware_concurrency();
		std::cout << nCores << " Cores Detected" << std::endl;

	} else {
		STD_LOG("Mode: Serial");

		std::unique_ptr<EventTreeManager> manager =
			std::make_unique<EventTreeManager>("Output.root",m_tdcIDs.size());

		// for (auto& file : fileNames) {
		// 	// Read File into WordBundle buffer
		// 	processFile(file);
    //
		// 	// makePackets();
    //   //
		// 	// makeEvents();
    //   //
		// 	// while (m_eventBuffer.isCompleteStored()) {
		// 	// 	auto events = m_eventBuffer.popToComplete();
		// 	// 	for (auto& event : events) {
		// 	// 		ASSERT(nullptr != event);
		// 	// 		manager->add(std::move(event));
		// 	// 		ASSERT(nullptr == event);
		// 	// 	}
		// 	// }
		// }

		manager->writeTree();
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
// void Processor::makePackets() {
// 	// Need to ensure this function can only be run on a single thread
//
// 	// Pointer to store current packet
// 	std::unique_ptr<Packet> currPacket = { nullptr };
//
// 	// Loop through bundles in buffer
// 	while (!m_wordBundleBuffer.empty()) {
// 		auto bundle = m_wordBundleBuffer.popFront();
// 		ASSERT(bundle != nullptr);
//
// 		if (bundle->isComplete()) {
// 			while (!bundle->empty()) {
// 				const auto word = bundle->getNextWord();
// 				const auto dataType = bindec::getDataType(word);
// 				if (2 == dataType) {
// 					if (currPacket != nullptr && currPacket->isGood()) {
// 						const auto index = currPacket->getTDCID();
// 						try {
// 							m_packetBuffers.at(index).push(std::move(currPacket));
// 						} catch (std::exception& e) {
// 							STD_ERR("Exception: " << e.what());
// 						}
// 					} else {
// 						// If packet is not good, delete it
// 						currPacket.reset(nullptr);
// 					}
// 					ASSERT(nullptr == currPacket);
// 					currPacket = std::make_unique<Packet>(bundle->getROCValue());
// 					ASSERT(currPacket != nullptr);
// 					currPacket->addHeader(word);
// 				} else if (currPacket != nullptr) {
// 					if (3 == dataType) {
// 						ASSERT(currPacket != nullptr);
// 						currPacket->addTrailer(word);
// 					} else if (4 == dataType || 5 == dataType) {
// 						ASSERT(currPacket != nullptr);
// 						currPacket->addDataline(word);
// 					} else {
// 						STD_ERR("Invalid Data Type found: " << dataType);
// 					}
// 				}
// 			}
// 		}
// 	}
// }
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
				STD_LOG("Finished " << *it);
				// If buffer is empty, remove from index list
				bufferIndexes.erase(it++);
			} else {
				// Extract front packet, add to event buffer
				m_eventBuffer.addPacket(std::move(m_packetBuffers.at(*it).popFront()));
			}
		}
		STD_LOG_VAR(bufferIndexes.size());
	}
}
