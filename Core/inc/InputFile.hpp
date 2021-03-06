#ifndef INPUTFILE_H
#define INPUTFILE_H

// STD
#include <string>
#include <iostream>

// LOCAL
#include "BoardIdentifier.hpp"

//! Stores information about a specific input file
class InputFile {
public:
	//! Constructor
	InputFile(
		const std::string filePath //!< Path of the file
	);

	//! Was all information able to be extracted?
	auto isComplete() const { return m_informationComplete; }

	//! Getter for file path
	auto getFilePath() const { return m_filePath; }

	//! Getter for readout board ID
	auto getBoardID() const { return BoardIdentifier(m_chainID, m_deviceID); }

	//! Getter for file number
	auto getFileNumber() const { return m_fileNumber; }

	//! Operator overload for <
	bool operator< (
		const InputFile& other //!< InputFile to be compared to
	) const;

	//! Operator overload for >
	bool operator> (
		const InputFile& other //!< InputFile to be compared to
	) const;

	//! Equality operator overload
	bool operator==(
		const InputFile& other //!< InputFile to be compared to
	) const;

	//! Overload << operator
	friend std::ostream& operator<<(
		std::ostream& os, //!< Output stream
		const InputFile& inputFile //!< BoardIdentifier to be output
	);

private:
	bool m_informationComplete = false; //!< Records whether all information was able to be extracted from the file path

	const std::string m_filePath; //!< Path of the file
	unsigned int m_chainID = 0; //!< Readout board ID of the file
	unsigned int m_deviceID = 0; //!< Readout board ID of the file
	unsigned int m_fileNumber = 0; //!< File number of the file
	unsigned long long m_timestamp = 0; //!< Timestamp of the file
};

#endif /* INPUTFILE_H */
