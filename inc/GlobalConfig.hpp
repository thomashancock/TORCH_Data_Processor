#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <string>
#include <vector>
#include <memory>

#include "Debug.hpp"

//! Class which manages programs configuration
/*!
	Reads the passed config file and configures the rest of the program accordinly
*/
class GlobalConfig {
public:
	//! Default Constructor
	GlobalConfig();
	//! Constructor with passed config file
	GlobalConfig(
		std::string configFile //!< The config file to be read
	);
	//! Deconstructor
	~GlobalConfig();

	//! Prints details of the stored configuration
	void printConfig();

	//! Tells the rest of the program whether to create human-readable output files
	bool debugOutput() { return m_debugOutput; }

	//! Tells the rest of the program whether to only output low level information
	bool LowLevelOnly() { return m_lowLevelOnly; }

	//! Used to tell the program whether to skip checking for complete events
	bool skipCompleteEventCheck() { return m_skipCompleteEventCheck; }

	//! Used to tell the program whether to apply the 2017 testbeam channel fix
	bool channelFix2017() { return m_channelFix2017; }

	//! Returns the list of stored TDC IDs
	std::shared_ptr< std::vector<unsigned int> > getTDCList() { return m_tdcList; }

private:
	//! Parses options read from the config file
	void parseOption(
		std::string key,  //!< The options key identifier
		std::string value //!< The value the option is set to
	);

private:
	bool m_debugOutput = false; //!< Stores whether debug output should be created
	bool m_lowLevelOnly = false; //!< Stores whether to only output low level information
	bool m_skipCompleteEventCheck = false; //!< Stores whether the complete event check should be skipped
	bool m_channelFix2017 = false; //!< Stores whether the 2017 channel fix should be applied
	std::shared_ptr< std::vector<unsigned int> > m_tdcList; //!< List of valid TDC IDs
};

#endif /* GLOBALCONFIG_H */
