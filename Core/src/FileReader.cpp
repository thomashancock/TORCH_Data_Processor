#include "FileReader.hpp"

// STD
#include <atomic>
#include <sstream>
#include <utility>
#include <algorithm>

// LOCAL
#include "BinaryDecoding.hpp"
#include "ErrorSpy.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
FileReader::FileReader(
	const std::list<ReadoutIdentifier>& readoutList,
	std::array< std::shared_ptr<bundleBuffer>, 4> wordBundleBuffers
) :
	m_wordBundleBuffers(std::move(wordBundleBuffers))
{
	// Validate inputs
	#ifdef LOCAL_DEBUG
	for (auto& bufferPtr : m_wordBundleBuffers) {
		ASSERT(nullptr != bufferPtr);
	}
	#endif /* LOCAL_DEBUG */
	ASSERT(m_inputFiles.empty());
	ASSERT(m_inputStreams.empty());
	ASSERT(readoutList.size() > 0);

	// TODO: Check all readoutBoardIDs are unique
	std::list<BoardIdentifier> boardList;
	for (const auto& entry : readoutList) {
		const auto boardID = entry.getBoardID();
		if (std::find(boardList.begin(), boardList.end(), boardID) == boardList.end()) {
			boardList.push_back(boardID);
		}
	}

	// Initialise maps
	for (const auto& boardID : boardList) {
		// Create a space to store files corresponding to the readout board ID
		m_inputFiles.insert(std::make_pair(boardID, std::list<InputFile>() ));

		// Create an input stream for the board ID
		m_inputStreams[boardID] = std::unique_ptr<std::ifstream>(nullptr);
		ASSERT(nullptr == m_inputStreams[boardID]);

		// Create a space to store the input stream length
		m_fileLengths.emplace(std::make_pair(boardID, 0));

		// Add a workspace to the bundle workspace map
		bundleWorkspace newWorkspace {{ nullptr }};
		m_bundleWorkspaces.insert(std::make_pair(boardID, std::move(newWorkspace)));
	}

	// Check map sizes are correct
	ASSERT(m_inputFiles.size() == boardList.size());
	ASSERT(m_inputStreams.size() == boardList.size());
	ASSERT(m_fileLengths.size() == boardList.size());
	ASSERT(m_bundleWorkspaces.size() == boardList.size());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileReader::stageFiles(
	const std::vector<std::string>& files
) {
	// Add files to file reader storage
	for (const auto& file : files) {
		addFile(file);
	}

	for (auto& entry : m_inputFiles) {
		// Sort stored files according to file number for each readout
		entry.second.sort();
		// Set relative rate based on number of files
		m_relativeRates[entry.first] = entry.second.size();
	}

	//TODO: Scale relative rate to smallest possible integer for each boardID

	// Set m_counterMax to max value in rates
	for (const auto& entry : m_relativeRates) {
		m_counterMax = (entry.second > m_counterMax) ? entry.second : m_counterMax;
	}

	// Stage the first file for each readout board
	for (const auto& entry : m_inputFiles) {
		stageNextFile(entry.first);
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
	for (unsigned int i = 0; i < nLoops; i++) {
		this->runProcessingLoop();
	}

	// Mark function as done
	readLock = false;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FileReader::addFile(
	const std::string& filePath
) {
	// Create an input file object from the file path
	InputFile inputFile(filePath);

	if (inputFile.isComplete()) {
		// Add the input file to the map of input files
		auto found = m_inputFiles.find(inputFile.getBoardID());
		if (m_inputFiles.end() != found) {
			found->second.push_back(std::move(inputFile));
		} else {
			STD_ERR("File " << inputFile.getFilePath() << " has invalid BoardID: " << inputFile.getBoardID());
		}
	} else {
		STD_ERR("Unable to derive sufficient information from file path: " << filePath);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileReader::runProcessingLoop() {
	ASSERT(m_counterMax > 0);

	// Loop through input streams
	for (auto& entry : m_inputStreams) {
		auto& boardID = entry.first;

		// If rate counter is greater than realtive rate for the boardID, skip this processing pass
		// This helps ensure syncronisation between the different readouts
		if (m_rateCounter > m_relativeRates[boardID]) {
			continue;
		}

		// Run processing pass of file
		auto& streamPtr = entry.second;
		if (nullptr == streamPtr) {
			// Check if there are more files to process
			if (!m_inputFiles[boardID].empty()) {
				// Remove previous first file from list if is not the first file
				m_inputFiles[boardID].pop_front();

				// Stage a new file
				stageNextFile(boardID);
			}
		} else {
			// Check if stream has expired
			if (!streamPtr->good()) {
				streamPtr.reset();
				continue;
			}

			// Read Header Information
			const auto nDataBytes = readHeaderLine(streamPtr);

			// Check number of bytes is as expected
			if (!isNDataBytesValid(boardID, streamPtr, nDataBytes)) {
				continue;
			}

			// Get workspace to create bundles
			auto& workspace = m_bundleWorkspaces[boardID];

			// Calculate number of blocks using bit shift to protect against floating point precision errors
			// 1 Block = 4 Words = 16 Bytes
			const unsigned int nDataBlocks = nDataBytes >> 4;
			for (unsigned int iBlock = 0; iBlock < nDataBlocks; iBlock++) {
				const auto block = readDataBlock(streamPtr);
				// Block => Slot Mapping
				// 0 => A, 1 => B, 2 => C, 3 => D

				ASSERT(workspace.size() == block.size());

				for (unsigned int i = 0; i < workspace.size(); i++) {
					// Reference assignment to ease readbility
					const auto& word = block[i];

					// Check word is not a filler word
					if (fillerWords.find(word) == fillerWords.end()) {
						// See if existing bundle has been created
						if (workspace[i] != nullptr) {
							// If bundle exists, check for ROC value
							if (15 == bindec::getDataType(word)) {
								// If ROC found, add to bundle
								workspace[i]->setRocValue(bindec::getROCValue(word));

								// Add bundle to buffer
								ASSERT(nextBundleBufferIndex < m_wordBundleBuffers.size());
								m_wordBundleBuffers[nextBundleBufferIndex++]->push(std::move(workspace[i]));

								// Cycle through buffers to spread data evenly between them
								if (nextBundleBufferIndex > 3) {
									nextBundleBufferIndex = 0;
								}

								// Ensure move was succesful
								ASSERT(workspace[i] == nullptr);
							} else {
								// If ROC is not found, add word to bundle
								ASSERT(workspace[i] != nullptr);
								workspace[i]->addWord(word);
							}
						} else {
							// If no bundle exists, add a new bundle
							if (15 != bindec::getDataType(word)) {
								// Only make a new bundle for a non-roc words
								workspace[i] = std::make_unique<WordBundle>(boardID);
								// Ensure bundle was created
								ASSERT(workspace[i] != nullptr);
								// Add word to bundle
								workspace[i]->addWord(word);
							}
						}
					}
				}
			}

			if (streamPtr->tellg() == m_fileLengths[boardID]) {
				streamPtr.reset();
			}
		}
	}

	// Update rate counter
	m_rateCounter = (m_rateCounter > m_counterMax) ? 0 : m_rateCounter + 1;
}
