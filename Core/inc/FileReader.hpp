#ifndef FILEREADER_H
#define FILEREADER_H

// STD
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <set>
#include <array>
#include <list>
#include <map>
#include <unordered_map>

// LOCAL
#include "WordBundle.hpp"
#include "ThreadSafeQueue.hpp"
#include "InputFile.hpp"
#include "ReadoutIdentifier.hpp"
#include "BoardIdentifier.hpp"

//! Read files and outputs word bundles
class FileReader {
	using bundleBuffer = ThreadSafeQueue< std::unique_ptr<WordBundle> >;
	using bundleWorkspace = std::array< std::unique_ptr<WordBundle>, 4>;

public:
	//! Constructor
	FileReader(
		const std::list<ReadoutIdentifier>& readoutList, //!< List of readout boards present
		std::array< std::shared_ptr<bundleBuffer>, 4> //!< Shared Pointer to WordBundle buffers
	);

	//! Stages files to be read out together
	void stageFiles(
		const std::vector<std::string>& files
	);

	//! Checks if all files have finished being read
	inline bool haveFilesExpired() const;

	//! Reads nLoops data blocks from each staged file
	void runProcessingLoops(
		const unsigned int nLoops
	);

private:
	//! Adds a file to be read out
	void addFile(
		const std::string& filePath //!< Path fo the file to add
	);

	//! Stages a new file
	inline void stageNextFile(
		const BoardIdentifier& boardID //!< Readout Board ID to stage for
	);

	//! Reads a single data block from each file
	void runProcessingLoop();

	//! Reads a header line from the passed stream
	inline unsigned int readHeaderLine(
		std::unique_ptr<std::ifstream>& inputData //!< The stream to read from
	);

	//! Checks if a number of bytes is valid, and takes action if not
	inline bool isNDataBytesValid(
		const BoardIdentifier& boardID,
		std::unique_ptr<std::ifstream>& inputData, //!< Stream nDataBytes was read from
		const unsigned int nDataBytes
	);

	//! Reads a data block line from the passed stream
	inline std::array<unsigned int,4> readDataBlock(
		std::unique_ptr<std::ifstream>& inputData //!< The stream to read from
	);

private:
	std::map< BoardIdentifier, std::list<InputFile> > m_inputFiles; //!< Input file storage
	std::map< BoardIdentifier, std::unique_ptr< std::ifstream > > m_inputStreams; //!< Vector of input streams

	unsigned int m_rateCounter = 0;
	unsigned int m_counterMax = 0;
	std::map< BoardIdentifier, unsigned int > m_relativeRates;

	std::map< BoardIdentifier, unsigned int > m_fileLengths; //!< Map to store file lengths
	std::map< BoardIdentifier, bundleWorkspace > m_bundleWorkspaces; //!< Map of pointers used to create WordBundles

	std::array< std::shared_ptr<bundleBuffer>, 4> m_wordBundleBuffers; //!< Pointers to shared WordBundle buffers
	unsigned int nextBundleBufferIndex = 0;

	const std::set<unsigned int> fillerWords = { 0xA0A0A0A0, 0xB0B0B0B0, 0xC0C0C0C0, 0xD0D0D0D0 }; //! Set containing filler words (move to config?)
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Inlines
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool FileReader::haveFilesExpired() const {
	// When a file is exhausted, it's popped from the file list
	// Therefore, check for non-empty lists
	for (const auto& entry : m_inputFiles) {
		if (!entry.second.empty()) {
			return false;
		}
	}
	return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileReader::stageNextFile(
	const BoardIdentifier& boardID
) {
	if (!m_inputFiles[boardID].empty()) {
		auto& streamPtr = m_inputStreams[boardID];
		ASSERT(nullptr == streamPtr);
		const auto& filePath = m_inputFiles[boardID].front().getFilePath();
		streamPtr = std::make_unique<std::ifstream>(filePath, std::ios::in | std::ios::binary);
		ASSERT(nullptr != streamPtr);

		STD_LOG("Staging " << filePath);

		// Check file is valid
		if (!streamPtr->good()) {
			WARNING("Error reading " << filePath);
			return;
		} else {
			// If file is valid, record file length
			streamPtr->seekg(0, std::ios::end);
			m_fileLengths[boardID] = streamPtr->tellg();
			streamPtr->seekg(0);
		}
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int FileReader::readHeaderLine(
	std::unique_ptr<std::ifstream>& inputData
) {
	// Read in header word
	// Discard first two bytes (mult = 0)
	unsigned int output = 0;
	for (const auto& mult : { 0, 0, 0x00000100, 0x00000001 } ) {
		output += mult * inputData->get();
	}
	return output;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FileReader::isNDataBytesValid(
	const BoardIdentifier& boardID,
	std::unique_ptr<std::ifstream>& streamPtr,
	const unsigned int nDataBytes
) {
	const auto& filePath = m_inputFiles[boardID].front().getFilePath();

	if (nDataBytes > 4096) {
		STD_ERR("nDataBytes " << nDataBytes << " > 4096 found in file " << filePath << ". Skipping rest of file.");
		streamPtr.reset();
		return false;
	}

	if (0 != nDataBytes % 4) {
		STD_ERR("Length of data packet (" << nDataBytes << ") is not dividible by 4. File: " << filePath << ". Skipping rest of file.");
		streamPtr.reset();
		return false;
	}

	if (0 != nDataBytes % 16) {
		WARNING("Incomplete data block detected in file " << filePath << ". Will skip to next block");

		// Skip requisite number of bytes to find a new header word
		for (unsigned int i = 0; i < nDataBytes; i++) {
			streamPtr->get();
		}
		return false;
	}

	return true;
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

	// mult is used to offset each byte to construct a 32 bit word
	for (const auto& mult : { 0x01000000, 0x00010000, 0x00000100, 0x00000001 } ) {
		// Iterate in reverse order to give D = 3, C = 2, B = 1, A = 0
		for (int i = 3; i >= 0; i--) {
			block[i] += mult * inputData->get();
		}
	}

	// block will be moved due to RVO
	return block;
}

#endif /* FILEREADER_H */
