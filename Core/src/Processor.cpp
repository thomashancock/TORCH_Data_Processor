#include "Processor.hpp"

// STD
#include <fstream>
#include <ios>
#include <map>

// LOCAL
#include "Debug.hpp"
#include "BinaryDecoding.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Processor::Processor(
	std::unique_ptr<const Config> config
) :
	m_config(std::move(config))
{
	// Assert move operation was succesful
	ASSERT(nullptr == config);

	// Assert buffers are empty
	ASSERT(m_wordBundleBuffer.empty());
	ASSERT(m_eventBuffer.empty());

	m_config->print();

	// Spoof Config Settings
	// "0A" = 10, 11
	// "0B" = 8, 9
	// "0C" = 12, 13
	// "0D" = 14, 15

	// Initialise Packet Buffers
	std::list<unsigned int> tdcIDs { 8, 9, 10, 11, 12, 13, 14, 15 };
	initializePacketBuffers(tdcIDs);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::processFiles(
	const std::vector<std::string> fileNames
) {
	// SEQUENTIAL VERSION
	STD_LOG_VAR(fileNames.size());
	for (auto& file : fileNames) {
		// Read File into WordBundle buffer
		processFile(file);

		// Once all WordBundle buffers are !empty
		// Make Packets from each buffer

		// Once all PacketBuffers are flagged ready, make events

		// Write Events to Root File

	}

	std::cout << ((true == m_wordBundleBuffer.empty()) ? "Empty" : "Full") << std::endl;

	// PARALLEL VERSION
	// for (auto& file : fileNames) {
	//
	// }
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

		// Initialise Packet Buffer Flags
		// atomic_bool cannot be copied or moved, so must construct in place
		m_packetBufferFlags[id] = false;
	}
	ASSERT(m_packetBuffers.size() == tdcIDs.size());
	ASSERT(m_packetBuffers.size() == m_packetBufferFlags.size());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::processFile(
	const std::string fileName
) {
	// Load file to be read by input stream
	std::ifstream inputData(fileName, std::ios::in | std::ios::binary);

	// Check input stream is good
	if (!inputData.good()) {
		//TODO: Error Log
		return;
	}

	// Create wordBundle pointers
	std::array<std::unique_ptr<WordBundle>, 4> bundles = {{ nullptr }};

	// Get file length
	inputData.seekg(0, std::ios::end);
	const unsigned int fileLength = inputData.tellg();
	inputData.seekg(0);

	while (inputData.good()) {
		if (fileLength == inputData.tellg()) {
			STD_LOG("End of file reached");
			break;
		}

		unsigned int readoutBoardNumber = 0;
		unsigned int nDataBytes = 0;
		readHeaderLine(inputData,readoutBoardNumber,nDataBytes);

		if (nDataBytes > 4096) {
			STD_ERR("nDataBytes " << nDataBytes << " > 4096 found in file " << fileName << ". Skipping rest of file.");
			return;
		}

		if (0 != nDataBytes % 4) {
			STD_ERR("Length of data packet is not dividible by 4. File: " << fileName << ". Skipping rest of file.");
			return;
		}

		if (0 != nDataBytes % 16) {
			// TODO: Replace WARNING with log of error in ErrorSpy
			WARNING("Incomplete data block detected. Will skip to next block");

			// Skip requisite number of bytes to find a new header word
			for (unsigned int i = 0; i < nDataBytes; i++) {
				inputData.get();
			}
		}

		const static std::map<int, char> blockIndex = {
			std::make_pair(0,'A'),
			std::make_pair(1,'B'),
			std::make_pair(2,'C'),
			std::make_pair(3,'D'),
		};

		// Calculate number of blocks using bit shift to protect against floating point precision errors
		// 1 Block = 4 Words = 16 Bytes
		const unsigned int nDataBlocks = nDataBytes >> 4;
		// STD_LOG_VAR(nDataBlocks);
		for (unsigned int iBlock = 0; iBlock < nDataBlocks; iBlock++) {

			const auto block = readDataBlock(inputData);
			// Block => Slot Mapping
			// 0 => A, 1 => B, 2 => C, 3 => D

			ASSERT(bundles.size() == block.size());

			for (int i = 0; i < bundles.size(); i++) {
				// Reference assignment to ease readbility
				const auto& word = block[i];

				// Check word is not a filler word
				if (fillerWords.find(word) == fillerWords.end()) {
					// See if existing bundle has been created
					if (bundles[i] != nullptr) {
						// If bundle exists, check for ROC value
						if (15 == bindec::getDataType(word)) {
							// If ROC found, add to bundle
							bundles[i]->setRocValue(bindec::getROCValue(word));

							// Add bundle to buffer
							m_wordBundleBuffer.push(std::move(bundles[i]));
							// Ensure move was succesful
							ASSERT(bundles[i] == nullptr);
						} else {
							// If ROC is not found, add word to bundle
							ASSERT(bundles[i] != nullptr);
							bundles[i]->addWord(word);
						}
					} else {
						// If no bundle exists, add a new bundle
						bundles[i] = std::make_unique<WordBundle>(readoutBoardNumber, blockIndex.at(i));
						// Ensure bundle was created
						ASSERT(bundles[i] != nullptr);
						// Add word to bundle
						bundles[i]->addWord(word);
					}
				}
			}
		}
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::readHeaderLine(
	std::ifstream& inputData,
	unsigned int& readoutBoardNumber,
	unsigned int& nDataBytes
) {
	// Read 4 bytes of data
	char byte1 = inputData.get();
	char byte2 = inputData.get();
	char byte3 = inputData.get();
	char byte4 = inputData.get();

	// Reconstruct bytes into appropriate variables
	// bytes implicitly converted to int to combine into unsigned integer
	readoutBoardNumber = (256 * byte1) + byte2;
	nDataBytes = (256 * byte3) + byte4;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::array<unsigned int,4> Processor::readDataBlock(
	std::ifstream& inputData
) {
	// Format is DCBA DCBA DCBA DCBA. Want AAAA BBBB CCCC DDDD
	// Each number representes one byte
	// Number gives order in which each byte will be read
	// Slot D  0,  4,  8, 12
	// Slot C  1,  5,  9, 13
	// Slot B  2,  6, 10, 14
	// Slot A  3,  7, 11, 15

	// Declare array to store block
	std::array<unsigned int, 4> block = {{ 0 }};

	// mult is used to offset eact byte to construct a 32 bit word
	for (auto& mult : { 0x01000000, 0x00010000, 0x00000100, 0x00000001 } ) {
		// Iterate in reverse order to give D = 3, C = 2, B = 1, A = 0
		for (int i = 3; i >= 0; i--) {
			block[i] += mult * inputData.get();
		}
	}

	// block will be moved due to RVO
	return block;
}
