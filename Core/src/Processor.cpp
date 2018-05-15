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
	// Assert buffers are empty
	// ASSERT(m_wordBundleBuffer.empty());

	// Print config settings
	ASSERT(nullptr != config);
	config->print();

	// Initialise Word Bundle Buffers
	m_wordBundleBuffers = std::make_shared< std::array< bundleBuffer, 4> >();

	// Initialise file reader
	STD_LOG("INITIALISING FILE READER");
	m_fileReader = std::make_unique<FileReader>(1,m_wordBundleBuffers);
	// Should be two pointers refering to the bundle buffers
	ASSERT(2 == m_wordBundleBuffers.use_count());
	STD_LOG("DONE");

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
// void Processor::processFile(
// 	const std::string fileName
// ) {
// 	// Load file to be read by input stream
// 	std::ifstream inputData(fileName, std::ios::in | std::ios::binary);
//
// 	// Check input stream is good
// 	if (!inputData.good()) {
// 		//TODO: Error Log
// 		return;
// 	}
//
// 	// Create wordBundle pointers
// 	std::array<std::unique_ptr<WordBundle>, 4> bundles = {{ nullptr }};
//
// 	// Get file length
// 	inputData.seekg(0, std::ios::end);
// 	const unsigned int fileLength = inputData.tellg();
// 	inputData.seekg(0);
//
// 	while (inputData.good()) {
// 		if (fileLength == inputData.tellg()) {
// 			STD_LOG("End of file reached");
// 			break;
// 		}
//
// 		unsigned int readoutBoardNumber = 0;
// 		unsigned int nDataBytes = 0;
// 		readHeaderLine(inputData,readoutBoardNumber,nDataBytes);
//
// 		if (nDataBytes > 4096) {
// 			STD_ERR("nDataBytes " << nDataBytes << " > 4096 found in file " << fileName << ". Skipping rest of file.");
// 			return;
// 		}
//
// 		if (0 != nDataBytes % 4) {
// 			STD_ERR("Length of data packet is not dividible by 4. File: " << fileName << ". Skipping rest of file.");
// 			return;
// 		}
//
// 		if (0 != nDataBytes % 16) {
// 			// TODO: Replace WARNING with log of error in ErrorSpy
// 			WARNING("Incomplete data block detected. Will skip to next block");
//
// 			// Skip requisite number of bytes to find a new header word
// 			for (unsigned int i = 0; i < nDataBytes; i++) {
// 				inputData.get();
// 			}
// 		}
//
// 		// const static std::map<int, char> blockIndex = {
// 		// 	std::make_pair(0,'A'),
// 		// 	std::make_pair(1,'B'),
// 		// 	std::make_pair(2,'C'),
// 		// 	std::make_pair(3,'D'),
// 		// };
//
// 		// Calculate number of blocks using bit shift to protect against floating point precision errors
// 		// 1 Block = 4 Words = 16 Bytes
// 		const unsigned int nDataBlocks = nDataBytes >> 4;
// 		// STD_LOG_VAR(nDataBlocks);
// 		for (unsigned int iBlock = 0; iBlock < nDataBlocks; iBlock++) {
//
// 			const auto block = readDataBlock(inputData);
// 			// Block => Slot Mapping
// 			// 0 => A, 1 => B, 2 => C, 3 => D
//
// 			ASSERT(bundles.size() == block.size());
//
// 			for (int i = 0; i < bundles.size(); i++) {
// 				// Reference assignment to ease readbility
// 				const auto& word = block[i];
//
// 				// Check word is not a filler word
// 				if (fillerWords.find(word) == fillerWords.end()) {
// 					// See if existing bundle has been created
// 					if (bundles[i] != nullptr) {
// 						// If bundle exists, check for ROC value
// 						if (15 == bindec::getDataType(word)) {
// 							// If ROC found, add to bundle
// 							bundles[i]->setRocValue(bindec::getROCValue(word));
//
// 							// Add bundle to buffer
// 							// m_wordBundleBuffer.push(std::move(bundles[i]));
// 							// Ensure move was succesful
// 							ASSERT(bundles[i] == nullptr);
// 						} else {
// 							// If ROC is not found, add word to bundle
// 							ASSERT(bundles[i] != nullptr);
// 							bundles[i]->addWord(word);
// 						}
// 					} else {
// 						// If no bundle exists, add a new bundle
// 						bundles[i] = std::make_unique<WordBundle>(readoutBoardNumber);
// 						// Ensure bundle was created
// 						ASSERT(bundles[i] != nullptr);
// 						// Add word to bundle
// 						bundles[i]->addWord(word);
// 					}
// 				}
// 			}
// 		}
// 	}
// }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// void Processor::readHeaderLine(
// 	std::ifstream& inputData,
// 	unsigned int& readoutBoardNumber,
// 	unsigned int& nDataBytes
// ) {
// 	// Read 4 bytes of data
// 	char byte1 = inputData.get();
// 	char byte2 = inputData.get();
// 	char byte3 = inputData.get();
// 	char byte4 = inputData.get();
//
// 	// Reconstruct bytes into appropriate variables
// 	// bytes implicitly converted to int to combine into unsigned integer
// 	readoutBoardNumber = (256 * byte1) + byte2;
// 	nDataBytes = (256 * byte3) + byte4;
// }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// std::array<unsigned int,4> Processor::readDataBlock(
// 	std::ifstream& inputData
// ) {
// 	// Format is DCBA DCBA DCBA DCBA. Want AAAA BBBB CCCC DDDD
// 	// Each number representes one byte
// 	// Number gives order in which each byte will be read
// 	// Slot D  0,  4,  8, 12
// 	// Slot C  1,  5,  9, 13
// 	// Slot B  2,  6, 10, 14
// 	// Slot A  3,  7, 11, 15
//
// 	// Declare array to store block
// 	std::array<unsigned int, 4> block = {{ 0 }};
//
// 	// mult is used to offset eact byte to construct a 32 bit word
// 	for (auto& mult : { 0x01000000, 0x00010000, 0x00000100, 0x00000001 } ) {
// 		// Iterate in reverse order to give D = 3, C = 2, B = 1, A = 0
// 		for (int i = 3; i >= 0; i--) {
// 			block[i] += mult * inputData.get();
// 		}
// 	}
//
// 	// block will be moved due to RVO
// 	return block;
// }
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
