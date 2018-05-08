#ifndef CONFIG_H
#define CONFIG_H

// STD
#include <string>

// ROOT
#include "TXMLEngine.h"

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

private:
	bool m_configRead = false;

};

#endif /* CONFIG_H */
