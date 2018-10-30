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
#include "ChannelMappings.hpp"
#include "EdgeModifiers.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Processor::Processor(
	std::unique_ptr<const Config> config
) :
	m_config(std::move(config)),
	m_eventBuffer(m_config->getReadoutList())
{
	// Technically redundant assertion
	// m_eventBuffer initialization will cause seg fault if m_config == nullptr
	// Kept in case of future modifications moving the m_eventBuffer initilization
	ASSERT(nullptr == config);

	// Print config settings
	m_config->print();

	// Set Channel Mapping and Edge Modifier
	chlmap::setChannelMapping(m_config->getChannelMappingKey());
	edgmod::setEdgeModifier(m_config->getEdgeModifierKey());

	// Initialise Word Bundle Buffers
	for (auto& ptr : m_wordBundleBuffers) {
		ptr = std::make_shared<bundleBuffer>();
	}

	// Initialise file reader
	m_fileReader = std::make_unique<FileReader>(m_config->getReadoutList(), m_wordBundleBuffers);
	ASSERT(nullptr != m_fileReader);

	// After FileReader initialization, should be two pointers refering to the bundle buffers
	#ifdef LOCAL_DEBUG
	for (const auto& ptr : m_wordBundleBuffers) {
		ASSERT(nullptr != ptr);
		ASSERT(2 == ptr.use_count());
	}
	#endif /* LOCAL_DEBUG */

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
	const std::vector<std::string>& fileNames
) {
	// Load files into file reader
	ASSERT(nullptr != m_fileReader);
	m_fileReader->stageFiles(fileNames);

	// Run chosen mode
	const auto mode = m_config->getRunMode();
	if (RunMode::QuickCheck == mode) {
		runQuickCheck();
	} else if (RunMode::LowLevel == mode) {
		runLowLevel(outputFile);
	} else if (RunMode::Parallel == mode) {
		runParallel(outputFile);
	} else {
		// Run Serial by default
		runSerial(outputFile);
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
void Processor::runQuickCheck() {
	STD_LOG("Mode: QuickCheck");

	int bundleCount = 0;
	while (!m_fileReader->haveFilesExpired()) {
		m_fileReader->runProcessingLoops(1);

		for (auto& buffer : m_wordBundleBuffers) {
			while(!buffer->empty()) {
				bundleCount += 1;
				auto bundle = buffer->popFront();
				std::cout << "New Bundle = " << bundle->getBoardID()
				          << " ROC: " << bundle->getROCValue() << std::endl;
				while (!bundle->empty()) {
					bindec::printWord(bundle->getNextWord());
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
	const std::string outputFile
) {
	STD_LOG("Mode: LowLevel");

	// Declare Output Manager
	std::unique_ptr<PacketTreeManager> manager =
		std::make_unique<PacketTreeManager>(m_config,outputFile.c_str());

	// Process files
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
				// manager->add(std::move(entry.second.popFront()));
				manager->add(entry.second.popFront());
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
	const std::string outputFile
) {
	STD_LOG("Mode: Serial");

	std::unique_ptr<EventTreeManager> manager =
		std::make_unique<EventTreeManager>(m_config,outputFile.c_str(),m_config->getReadoutList().size());

	// Process files
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

	// Dump remaining events
	auto events = m_eventBuffer.dumpAll();
	for (auto& event : events) {
		ASSERT(nullptr != event);
		manager->add(std::move(event));
		ASSERT(nullptr == event);
	}

	// Write output tree
	manager->writeTree();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::runParallel(
	const std::string outputFile
) {
	STD_LOG("Mode: Parallel");
	WARNING("This mode is not currently implemented. Please run in \"Serial\" mode instead.");

	const auto maxTasks = std::thread::hardware_concurrency() - 1;
	if (0 == maxTasks) {
		STD_ERR("Only single core is available. Please run in serial mode instead.");
		return;
	}
	STD_LOG("NCores Detected: " << maxTasks + 1);

	outputFile.c_str(); // Prevents compiler warning from incomplete function

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

		// Check that the bundle is complete
		if (bundle->isComplete()) {
			// If it is, remove words until it's empty
			while (!bundle->empty()) {
				const auto word = bundle->getNextWord();

				// Check data type of word is one we expect
				const auto dataType = bindec::getDataType(word);
				if ((dataType > 5)||(dataType < 2)) {
					// If not, log an error
					std::stringstream errorMessage;
					errorMessage << "Invalid Datatype " << dataType;
					ErrorSpy::getInstance().logError(errorMessage.str(), bundle->getBoardID());
					continue;
				}

				// If data type is one which is expected, use it to construct packets
				// First check for header, as this will trigger the creation of a new packet
				if (2 == dataType) {
					// If a packet is already being constructed, it will be incomplete
					if (nullptr != currPacket) {
						// A complete packet should not reach this point
						ASSERT(!currPacket->isComplete());
						// Log that a packet was dumped
						ErrorSpy::getInstance().logError("Dumped incomplete packet",ReadoutIdentifier(bundle->getBoardID(),bindec::getTDCID(word)));
						// Delete the incomplete packet
						currPacket.reset(nullptr);
					}
					// Make a new packet
					// In order to make a new packet, no packet should currently be stored
					ASSERT(nullptr == currPacket);
					// Make a new packet
					currPacket = std::make_unique<Packet>(bundle->getBoardID(),bundle->getROCValue(),word);
					ASSERT(currPacket != nullptr);
				// If the word is not a header word, check if a packet is currently in construction
				} else if (currPacket != nullptr) {
					// Check word data type and add appropriately
					if (3 == dataType) {
						ASSERT(currPacket != nullptr);
						currPacket->addTrailer(word);

						// A packet should be complete when a trailer is added
						// Check the newly completed packet is good (consistent + complete)
						if (currPacket->isGood()) {
							// If it is, add it to the acket buffers
							const auto index = currPacket->getReadoutID().getTDCID();
							try {
								// Attempt to add the packet to the appropriate buffer
								m_packetBuffers.at(index).push(std::move(currPacket));
							} catch (std::exception& e) {
								STD_ERR("Exception: " << e.what() << ". Index " << index << ". Please ensure TDC ID " << index << " is present in the arrangement section of the config file.");
								// Packet cannot be moved, so destory it
								currPacket.reset(nullptr);
							}
						} else {
							// If packet is not good, delete it
							ErrorSpy::getInstance().logError("Bad Packet Dumped",currPacket->getReadoutID());
							currPacket.reset(nullptr);
						} /* if (currPacket->isGood()) */
					// If data type indicates a data word, simply add it to current packet
					} else if (4 == dataType || 5 == dataType) {
						ASSERT(currPacket != nullptr);
						currPacket->addDataline(word);
					}
				} else {
					// If this point is reached, a word has been found while a packet is not being constructed
					// This means it has appeared out of sequence (i.e. it is not between a header and a trailer)
					// An error is logged, and the word is discarded
					ErrorSpy::getInstance().logError("Word found out of sequence",bundle->getBoardID());
				} /* if (2 == dataType) */
			} /* while (!bundle->empty()) */
		} /* if (bundle->isComplete()) */
	} /* while (!wordBundleBuffer->empty()) */
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
				m_eventBuffer.addPacket(m_packetBuffers.at(*it).popFront());
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
	// Check for complete events and write to file if found
	while (m_eventBuffer.isCompleteStored()) {
		auto events = m_eventBuffer.popToComplete();
		for (auto& event : events) {
			ASSERT(nullptr != event);
			manager->add(std::move(event));
			ASSERT(nullptr == event);
		}
	}

	// Check for buffer bloat and dump if needed
	if (m_eventBuffer.isBloated()) {
		WARNING("Dumping events due to buffer bloat");
		auto events = m_eventBuffer.dumpHalf();
		for (auto& event : events) {
			ASSERT(nullptr != event);
			manager->add(std::move(event));
			ASSERT(nullptr == event);
		}
	}
}
