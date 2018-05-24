#include "InputFile.hpp"

// STD
#include <iostream>
#include <sstream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
InputFile::InputFile(
	const std::string filePath
) noexcept :
	m_filePath(filePath)
	// m_filePath(std::move(filePath))
{
	// Extract file name from full path
	const auto filename = m_filePath.substr(m_filePath.rfind('/') + 1);

	std::stringstream reader(filename);
	std::string segment;
	unsigned int count = 0;
	while(std::getline(reader,segment,'_')) {
		if ("Device" == segment) {
			// Finding Device in string triggers read (i.e. count = 1)
			count++;
		} else if (1 == count) {
			// If count == 1, the readout board ID is next
			m_readoutBoardID = std::stoi(segment);
			count++;
		} else if (2 == count) {
			// If count == 2, the file number is next
			m_fileNumber = std::stoi(segment);
			m_informationComplete = true;
			break;
		}
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFile::print() const {
	std::cout << "Path: " << m_filePath << std::endl;
	std::cout << "ReadoutBoardID: " << m_readoutBoardID << std::endl;
	std::cout << "FileNumber:     " << m_fileNumber << std::endl;
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
	if ((m_fileNumber == other.getFileNumber())&&(m_readoutBoardID == other.getReadoutBoardID())) {
		return true;
	} else {
		return false;
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
