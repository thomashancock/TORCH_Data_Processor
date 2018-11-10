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
#include <algorithm>

// LOCAL
#include "WordBundle.hpp"
#include "ThreadSafeQueue.hpp"
#include "InputFile.hpp"
#include "ReadoutIdentifier.hpp"
#include "BoardIdentifier.hpp"

//! Read files and outputs word bundles
/*!
	Reads in files passed by stageFiles(). Files are sorted according to the chain ID, device ID and file number to ensure they are read in the correct order.

	Once files are sorted, for each board ID (combination of chain ID and device ID, see BoardIdentifier class), the number of files is counted. If the files counts are all identical, the FileReader will continue in TimeSync mode. If not, it will continue in SizeSync Mode.

	<B>TimeSync Mode</B>

	TimeSync Mode assumes new files are begun as soon as one of the previous is full. This means each will start at the same point in time, but that each file could be a different size in bytes.

	In TimeSync mode, files are groups together based on file number and read in together, so as to ensure synchronisation is kept across the different boards.

	<B>SizeSync Mode</B>

	SizeSync Mode assums a new file is created for a given board as soon as the previous one is full. This leads to synchonisitiy between the different boards, and hence each board can have a different number of files.

	In SizeSync mode, the rate at which each file is read is scaled based on the relative number of files counted for each of the boards. This was board with less files are read more slowly so as to ensure syncronisation is kept.
 */

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

	void processDataPacket(
		const BoardIdentifier& boardID,
		std::unique_ptr< std::ifstream >& streamPtr
	);

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

	//! Checks if all values stored in a map are identical
	/*!
		For a map with BoardIdentifier keys, will return true if all values are
		equal, otherwise will return false.

		Created as a template function to allow use over all the std::maps stored
		in the FileReader.
	 */
	template<class T>
	bool areElementsIdentical(
		std::map< BoardIdentifier, T > map
	);

private:
	enum SyncType {
		equalSize, //! Represents SizeSync Mode
		equalTime, //! Represents TimeSync Mode
		null //! Equivallent of 0, used to ensure a mode has been selected
	}; //!< Enum to store which kind of syncronisation is to be used

private:
	std::map< BoardIdentifier, std::list<InputFile> > m_inputFiles; //!< Input file storage
	std::map< BoardIdentifier, std::unique_ptr< std::ifstream > > m_inputStreams; //!< Vector of input streams

	SyncType syncType = null; //!< The type of syncronisation determined from the input files

	unsigned int m_rateCounter = 0; //!< Used to scale the read speed based on m_relativeRates
	unsigned int m_counterMax = 0; //!< Stores the maximum value m_rateCounter should reach
	std::map< BoardIdentifier, unsigned int > m_relativeRates; //!< The relative read speed of each input stream (i.e. Board ID)

	std::map< BoardIdentifier, unsigned int > m_fileLengths; //!< Map to store file lengths
	std::map< BoardIdentifier, bundleWorkspace > m_bundleWorkspaces; //!< Map of pointers used to create WordBundles

	std::array< std::shared_ptr<bundleBuffer>, 4> m_wordBundleBuffers; //!< Pointers to shared WordBundle buffers
	unsigned int nextBundleBufferIndex = 0; //!< Used to cycle through buffers (currently disabled due to bug)

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
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<class T>
bool FileReader::areElementsIdentical(
	std::map< BoardIdentifier, T > map
) {
	// Get the first value
	const T firstVal = map.begin()->second;

	// Use std::all_of to check if all elements in the map are equal to the first
	return std::all_of(
		std::next(map.begin()), map.end(),
		[firstVal] (typename decltype(map)::const_reference t) {
			return t.second == firstVal;
		}
	);
}


#endif /* FILEREADER_H */
