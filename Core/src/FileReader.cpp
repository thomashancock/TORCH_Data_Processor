#include "FileReader.hpp"

// STD
#include <atomic>

// LOCAL
#include "BinaryDecoding.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
FileReader::FileReader(
	const unsigned int nReadoutBoards,
	std::shared_ptr< std::array< bundleBuffer, 4> > wordBundleBuffers
) :
	m_wordBundleBuffers(std::move(wordBundleBuffers))
{
	ASSERT(m_wordBundleBuffers != nullptr);
	ASSERT(m_inputStreams.empty());
	ASSERT(nReadoutBoards > 0);

	for (auto i = 0; i < nReadoutBoards; i++) {
		// Create a stream for each readout board
		m_inputStreams.emplace_back(std::move(nullptr));

		// Create a file length counter for each readout board
		m_fileLengths.push_back(0);

		// Create an array of bundles for readout board
		m_bundleWorkspaces.emplace_back();

		// Set array of bundle pointers to be nullptr
		for (auto& ptr : m_bundleWorkspaces.back()) {
			ptr.reset();
		}
	}

	ASSERT(m_inputStreams.size() == nReadoutBoards);
	ASSERT(m_fileLengths.size() == nReadoutBoards);
	ASSERT(m_bundleWorkspaces.size() == nReadoutBoards);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileReader::stageFiles(
	const std::vector<std::string>& m_files
) {
	ASSERT(m_files.size() == m_inputStreams.size());

	if (!this->haveFilesExpired()) {
		STD_ERR("Cannot stage files while not all previous files have expired");
	} else {
		for (auto i = 0; i < m_files.size(); i++) {
			// Load each file into an input stream
			m_inputStreams[i] = std::make_unique<std::ifstream>(m_files[i], std::ios::in | std::ios::binary);
			ASSERT(m_inputStreams[i] != nullptr);
			// Check file is valid
			if (!m_inputStreams[i]->good()) {
				WARNING("Error reading " << m_files[i]);
				this->clearStreams();
				return;
			} else {
				// If file is valid, record file length
				m_inputStreams[i]->seekg(0, std::ios::end);
				m_fileLengths[i] = m_inputStreams[i]->tellg();
				m_inputStreams[i]->seekg(0);
			}
		}
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileReader::runProcessingLoops(
	const unsigned int nLoops
) {
	// readLock ensures the function cannot run concurrently
	static std::atomic_bool readLock { false };

	// If readlock is true, function is being executed, therefore return
	if (readLock == true) {
		return;
	} else {
		readLock = true;
	}

	// Run n processing loops
	for (auto i = 0; i < nLoops; i++) {
		this->runProcessingLoop();
	}

	// Mark functoon as done
	readLock = false;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FileReader::runProcessingLoop() {
	for (unsigned int i = 0; i < m_inputStreams.size(); i++) {
		if (m_inputStreams[i] != nullptr) {

			// Check if stream has expired
			if (!m_inputStreams[i]->good()) {
				m_inputStreams[i].reset();
				continue;
			}

			// Read Header information
			unsigned int readoutBoardNumber = 0;
			unsigned int nDataBytes = 0;
			readHeaderLine(m_inputStreams[i],readoutBoardNumber,nDataBytes);

			// Check read data is okay
			if (nDataBytes > 4096) {
				//TODO: Log Error
				// STD_ERR("nDataBytes " << nDataBytes << " > 4096 found in file " << fileName << ". Skipping rest of file.");
				m_inputStreams[i].reset();
			}

			if (0 != nDataBytes % 4) {
				// TODO: Log Error
				// STD_ERR("Length of data packet is not dividible by 4. File: " << fileName << ". Skipping rest of file.");
				m_inputStreams[i].reset();
			}

			if (0 != nDataBytes % 16) {
				// TODO: Replace WARNING with log of error in ErrorSpy
				WARNING("Incomplete data block detected. Will skip to next block");

				// Skip requisite number of bytes to find a new header word
				for (unsigned int i = 0; i < nDataBytes; i++) {
					m_inputStreams[i]->get();
				}
			}

			// Calculate number of blocks using bit shift to protect against floating point precision errors
			// 1 Block = 4 Words = 16 Bytes
			auto& workspace = m_bundleWorkspaces[i];

			const unsigned int nDataBlocks = nDataBytes >> 4;
			for (unsigned int iBlock = 0; iBlock < nDataBlocks; iBlock++) {
				const auto block = readDataBlock(m_inputStreams[i]);
				// Block => Slot Mapping
				// 0 => A, 1 => B, 2 => C, 3 => D

				ASSERT(workspace.size() == block.size());

				for (int j = 0; j < workspace.size(); j++) {
					// Reference assignment to ease readbility
					const auto& word = block[j];

					// Check word is not a filler word
					if (fillerWords.find(word) == fillerWords.end()) {
						// See if existing bundle has been created
						if (workspace[j] != nullptr) {
							// If bundle exists, check for ROC value
							if (15 == bindec::getDataType(word)) {
								// If ROC found, add to bundle
								workspace[j]->setRocValue(bindec::getROCValue(word));

								// Add bundle to buffer
								ASSERT(i < m_wordBundleBuffers->size());
								m_wordBundleBuffers->at(i).push(std::move(workspace[j]));
								// Ensure move was succesful
								ASSERT(workspace[j] == nullptr);
							} else {
								// If ROC is not found, add word to bundle
								ASSERT(workspace[j] != nullptr);
								workspace[j]->addWord(word);
							}
						} else {
							// If no bundle exists, add a new bundle
							workspace[j] = std::make_unique<WordBundle>(readoutBoardNumber);
							// Ensure bundle was created
							ASSERT(workspace[j] != nullptr);
							// Add word to bundle
							workspace[j]->addWord(word);
						}
					}
				}
			}
		}
	}

	// Close any exhausted streams
	for (auto i = 0; i < m_inputStreams.size(); i++) {
		if (m_inputStreams[i]->tellg() == m_fileLengths[i]) {
			m_inputStreams[i].reset();
		}
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileReader::readHeaderLine(
	std::unique_ptr<std::ifstream>& inputData,
	unsigned int& readoutBoardNumber,
	unsigned int& nDataBytes
) {
	// Read 4 bytes of data
	char byte1 = inputData->get();
	char byte2 = inputData->get();
	char byte3 = inputData->get();
	char byte4 = inputData->get();

	// Reconstruct bytes into appropriate variables
	// bytes implicitly converted to int to combine into unsigned integer
	readoutBoardNumber = (256 * byte1) + byte2;
	nDataBytes = (256 * byte3) + byte4;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::array<unsigned int,4> FileReader::readDataBlock(
	std::unique_ptr<std::ifstream>& inputData
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
			block[i] += mult * inputData->get();
		}
	}

	// block will be moved due to RVO
	return block;
}
