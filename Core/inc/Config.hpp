#ifndef CONFIG_H
#define CONFIG_H

// STD
#include <string>
#include <list>

// ROOT
#include "TXMLEngine.h"

// LOCAL
#include "ModesEnum.hpp"

//! Class which manages programs configuration
/*!
	Reads the passed config file and configures the rest of the program accordinly
*/
class Config {
public:
	//! Constructor with passed config file
	Config(
		const std::string configFile
	);

	//! Returns whether config was succesfully read
	bool isConfigured() const { return m_configRead; }

	//! Returns the run mode
	const RunMode getRunMode() const { return m_mode; }

	//! Returns the list of stored TDC Ids
	const std::list<unsigned int> getTDCList() const { return m_tdcList; }

	//! Prints details of the stored configuration
	void print() const;

private:
	//! Parses options read from the config file
	void parseConfigFile(
		const std::string configFile  //!< Path of file to be read
	);

	//! Extract information from a node and its children
	void processNode(
		TXMLEngine* xmlEngine, //!< XML Engine pointer
		XMLNodePointer_t node //!< Node to be processed
	);

	//! Processes an option read from the config XML file
	void processOption(
		const std::string attribute, //!< Attribute name
		const std::string value //!< Attribute value
	);

	//! Returns the mode of operation as a string
	std::string getModeAsString() const;

private:
	bool m_configRead = false; //!< Has config been read succesfully?

	RunMode m_mode = RunMode::Serial; //!< Stores the desired run mode

	std::list<unsigned int> m_tdcList; //!< Stores the available TDCs
};

#endif /* CONFIG_H */
