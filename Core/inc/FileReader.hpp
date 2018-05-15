#ifndef FILEREADER_H
#define FILEREADER_H

// STD
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <set>
#include <array>
#include <mutex>

// LOCAL
#include "WordBundle.hpp"
#include "ThreadSafeQueue.hpp"

//! Read files and outputs word bundles
class FileReader {
	using bundleBuffer = ThreadSafeQueue< std::unique_ptr<WordBundle> >;
	using bundleWorkspace = std::array< std::unique_ptr<WordBundle>, 4>;

public:
	//! Constructor
	FileReader(
		const unsigned int nReadoutBoards, //!< Number of readout boards present
		std::array< std::shared_ptr<bundleBuffer>, 4>
		// std::shared_ptr< std::array< bundleBuffer, 4> > wordBundleBuffers
	);

	void stageFiles(
		const std::vector<std::string>& m_files
	);

	inline bool haveFilesExpired() const;

	void runProcessingLoops(
		const unsigned int nLoops
	);

private:
	inline void clearStreams();

	void runProcessingLoop();

	inline void readHeaderLine(
		std::unique_ptr<std::ifstream>& inputData,
		unsigned int& readoutBoardNumber,
		unsigned int& nDataBytes
	);

	inline std::array<unsigned int,4> readDataBlock(
		std::unique_ptr<std::ifstream>& inputData
	);

private:
	mutable std::mutex m_mut;

	std::vector< std::unique_ptr< std::ifstream > > m_inputStreams;
	std::vector< unsigned int > m_fileLengths;
	std::vector< bundleWorkspace > m_bundleWorkspaces;

	// std::array< ThreadSafeQueue< std::unique_ptr<WordBundle> > ,4> m_wordBundleBuffers;
	// std::shared_ptr< std::array< bundleBuffer, 4> > m_wordBundleBuffers;
	std::array< std::shared_ptr<bundleBuffer>, 4> m_wordBundleBuffers;

	const std::set<unsigned int> fillerWords = { 0xA0A0A0A0, 0xB0B0B0B0, 0xC0C0C0C0, 0xD0D0D0D0 }; //! Set containing filler words (move to config?)
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Inlines
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool FileReader::haveFilesExpired() const {
	// Check for non-null streams
	for (const auto& streamPtr : m_inputStreams) {
		if (streamPtr != nullptr) {
			return false;
		}
	}
	return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileReader::clearStreams() {
	// Close streams
	for (auto& streamPtr : m_inputStreams) {
		streamPtr.reset();
	}

	// Set file lengths to 0
	for (auto& length : m_fileLengths) {
		length = 0;
	}

	// Delete any semi-constructed bundles
	for (auto& bundlePtrArr : m_bundleWorkspaces) {
		for (auto& bundlePtr : bundlePtrArr) {
			bundlePtr.reset();
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

#endif /* FILEREADER_H */
