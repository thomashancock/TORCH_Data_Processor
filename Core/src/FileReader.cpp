#include "FileReader.hpp"

// STD
#include <atomic>
#include <sstream>
#include <utility>

// LOCAL
#include "BinaryDecoding.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
FileReader::FileReader(
	const std::list<unsigned int>& readoutBoardList,
	std::array< std::shared_ptr<bundleBuffer>, 4> wordBundleBuffers
) :
	m_wordBundleBuffers(std::move(wordBundleBuffers))
{
	// Validate inputs
	for (auto& bufferPtr : m_wordBundleBuffers) {
		ASSERT(nullptr != bufferPtr);
	}
	ASSERT(m_inputFiles.empty());
	ASSERT(m_inputStreams.empty());
	ASSERT(readoutBoardList.size() > 0);

	// TODO: Check all readoutBoardIDs are unique

	// Initialise maps
	for (const auto& boardID : readoutBoardList) {
		// Create a space to store files corresponding to the readout board ID
		m_inputFiles.insert(std::make_pair(boardID, std::list<InputFile>() ));

		// Create an input stream for the board ID
		m_inputStreams[boardID] = std::make_unique<std::ifstream>(nullptr);

		// Create a space to store the input stream length
		m_fileLengths.emplace(std::make_pair(boardID, 0));

		// Add a workspace to the bundle workspace map
		bundleWorkspace newWorkspace {nullptr};
		m_bundleWorkspaces.insert(std::make_pair(boardID, std::move(newWorkspace)));
	}

	// Stage the first set of files
	for (const auto& entry : m_inputFiles) {
		stageNextFile(entry.first);
	}

	// Check map sizes are correct
	ASSERT(m_inputFiles.size() == readoutBoardList.size());
	ASSERT(m_inputStreams.size() == readoutBoardList.size());
	ASSERT(m_fileLengths.size() == readoutBoardList.size());
	ASSERT(m_bundleWorkspaces.size() == readoutBoardList.size());
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

	// Sort stored files according to file number for each readout
	for (auto& entry : m_inputFiles) {
		entry.second.sort();
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
		auto found = m_inputFiles.find(inputFile.getReadoutBoardID());
		if (m_inputFiles.end() != found) {
			found->second.push_back(std::move(inputFile));
		} else {
			STD_ERR("File " << inputFile.getFilePath() << " has invalid ReadoutBoardID: " << inputFile.getReadoutBoardID());
		}
	} else {
		STD_ERR("Unable to dervie sufficient information from file path: " << filePath);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileReader::runProcessingLoop() {
	for (auto& entry : m_inputStreams) {
		auto& boardID = entry.first;
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

				for (int i = 0; i < workspace.size(); i++) {
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
								ASSERT(i < m_wordBundleBuffers.size());
								m_wordBundleBuffers[boardID]->push(std::move(workspace[i]));
								// Ensure move was succesful
								ASSERT(workspace[i] == nullptr);
							} else {
								// If ROC is not found, add word to bundle
								ASSERT(workspace[i] != nullptr);
								workspace[i]->addWord(word);
							}
						} else {
							// If no bundle exists, add a new bundle
							workspace[i] = std::make_unique<WordBundle>(boardID);
							// Ensure bundle was created
							ASSERT(workspace[i] != nullptr);
							// Add word to bundle
							workspace[i]->addWord(word);
						}
					}
				}
			}

			if (streamPtr->tellg() == m_fileLengths[boardID]) {
				streamPtr.reset();
			}
		}
	}
}
