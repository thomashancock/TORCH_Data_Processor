#include "InputFile.hpp"

// STD
#include <sstream>

// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
InputFile::InputFile(
	const std::string filePath
) :
	m_filePath(filePath)
{
	// Extract file name from full path
	const auto filename = m_filePath.substr(m_filePath.rfind('/') + 1);
	const auto rawname = filename.substr(0, filename.find_last_of("."));

	std::stringstream reader(rawname);
	std::string segment;
	unsigned int count = 0;
	while(std::getline(reader,segment,'_')) {
		if ("Chain" == segment) {
			// Finding Chain in string triggers read (i.e. count = 1)
			count = 1;
		} else if (2 == count) {
			// If count == 2, the readout board ID is next
			m_chainID = std::stoi(segment);
		} else if (4 == count) {
			// If count == 4, the readout board ID is next
			m_deviceID = std::stoi(segment);
		} else if (5 == count) {
			// If count == 5, the file number is next
			m_fileNumber = std::stoi(segment);
		} else if (6 == count) {
			// If count == 6, the file number is next
			m_timestamp = std::stoll(segment);
			m_informationComplete = true;
			break;
		}
		if (count > 0) count++;
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool InputFile::operator< (
	const InputFile& other
) const {
	return m_fileNumber < other.getFileNumber();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool InputFile::operator> (
	const InputFile& other
) const {
	return m_fileNumber > other.getFileNumber();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool InputFile::operator== (
	const InputFile& other
) const {
	if ((m_fileNumber == other.getFileNumber())&&(this->getBoardID() == other.getBoardID())) {
		return true;
	} else {
		return false;
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::ostream& operator<<(
	std::ostream& os,
	const InputFile& inputFile
) {
	os << "Path: " << inputFile.m_filePath << "\n"
	   << "ChainID:    " << inputFile.m_chainID << "\n"
	   << "DeviceID:   " << inputFile.m_deviceID << "\n"
	   << "FileNumber: " << inputFile.m_fileNumber << "\n"
	   << "Timestamp:  " << inputFile.m_timestamp;
	return os;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
