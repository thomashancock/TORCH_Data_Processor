#ifndef CONFIG_H
#define CONFIG_H

// STD
#include <string>
#include <list>
#include <set>

// ROOT
#include "TXMLEngine.h"

// LOCAL
#include "Debug.hpp"
#include "ModesEnum.hpp"
#include "ReadoutIdentifier.hpp"

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
	RunMode getRunMode() const { return m_mode; }

	//! Return the channel mapping
	auto getChannelMappingKey() const { return m_channelMapping; }

	//! Return the edge modifier
	auto getEdgeModifierKey() const { return m_edgeModifier; }

	//! Returns a set of channels to exclude from edge matching
	auto getEdgeMatchingExclusions() const { return m_edgeMatchingExclusions; }

	//! Returns the list of stored readout board Ids
	auto getReadoutList() const { return m_readoutList; }

	//! Returns the list of stored TDC Ids
	auto getTDCList() const { return m_tdcList; }

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
		const std::string nodeName, //!< Node the attribute belongs to
		const std::string attribute, //!< Attribute name
		const std::string value //!< Attribute value
	);

	//! Returns the mode of operation as a string
	std::string getModeAsString() const;

	inline unsigned int stoui(
		const std::string value
	) const;

private:
	bool m_configRead = false; //!< Has config been read succesfully?

	RunMode m_mode = RunMode::Serial; //!< Stores the desired run mode

	std::string m_channelMapping = "Std8x64Mapping"; //!< Stores the channel mapping
	std::string m_edgeModifier = "FlipEven"; //!< Stores the edge modifier

	std::set<unsigned int> m_edgeMatchingExclusions; //! Stores channels to exclude from edge matching

	std::list<ReadoutIdentifier> m_readoutList;
	std::list<unsigned int> m_tdcList; //!< Stores the available TDCs

	// std::list<unsigned int> m_readoutList; //!< Stores the available TDCs
	// std::list<unsigned int> m_deviceList; //!< Stores the available TDCs
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Inlines:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
inline unsigned int Config::stoui(
	const std::string value
) const {
	int tmp = 0;
	try {
		tmp = std::stoi(value);
	} catch (std::exception& e) {
		STD_ERR("Exception: " << e.what() << ". String was: " << value);
	}
	return static_cast<unsigned int>(tmp);
}

#endif /* CONFIG_H */
