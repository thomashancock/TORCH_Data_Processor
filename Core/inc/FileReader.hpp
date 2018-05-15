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
		std::shared_ptr< std::array< bundleBuffer, 4> > wordBundleBuffers
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

	void readHeaderLine(
		std::unique_ptr<std::ifstream>& inputData,
		unsigned int& readoutBoardNumber,
		unsigned int& nDataBytes
	);

	std::array<unsigned int,4> readDataBlock(
		std::unique_ptr<std::ifstream>& inputData
	);

private:
	mutable std::mutex m_mut;

	std::vector< std::unique_ptr< std::ifstream > > m_inputStreams;
	std::vector< unsigned int > m_fileLengths;
	std::vector< bundleWorkspace > m_bundleWorkspaces;

	// std::array< ThreadSafeQueue< std::unique_ptr<WordBundle> > ,4> m_wordBundleBuffers;
	std::shared_ptr< std::array< bundleBuffer, 4> > m_wordBundleBuffers;

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

#endif /* FILEREADER_H */
