#include "Config.hpp"

// STD
#include <algorithm>

// LOCAL
#include "Debug.hpp"
#include "BoardIdentifier.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Config::Config(
	const std::string configFile
) {
	STD_LOG("Config(): Reading Config");

	ASSERT(m_edgeMatchingExclusions.empty());
	ASSERT(m_readoutList.empty());
	// ASSERT(m_tdcList.empty());

	parseConfigFile(configFile);

	// Sort TDC ID list
	m_readoutList.sort();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Config::print() const {
	std::cout << "Configuration:" << std::endl;
	// Print Mode
	std::cout << "\tMode: " << this->getModeAsString() << std::endl;

	// Print Mapping
	std::cout << "\tChannel Mapping: " << m_channelMapping << std::endl;

	// Print Edge Modifier
	std::cout << "\tEdge Modifier: " << m_edgeModifier << std::endl;

	// Print excluded edge matching channels
	if (!m_edgeMatchingExclusions.empty()) {
		std::cout << "\tWill skip edge matching on channels: ";
		for (auto& entry : m_edgeMatchingExclusions) {
			std::cout << entry << " ";
		}
		std::cout << std::endl;
	}

	// Print Readout IDs
	std::cout << "\tReadout IDs:" << std::endl;
	for (const auto& id : m_readoutList) {
		std::cout << "\t\t" << id << std::endl;
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Config::parseConfigFile(
	const std::string configFile
) {
	STD_LOG("Config(): Parsing file " << configFile);

	// Create XML engine
	TXMLEngine* xmlEngine = new TXMLEngine();
	ASSERT(nullptr != xmlEngine);

	// Now try to parse xml file
	auto xmldoc = xmlEngine->ParseFile( configFile.c_str() );

	// Check file read correctly
	if (!xmldoc) {
		STD_ERR("Unable to read " << configFile);
	} else {
		auto mainNode  = xmlEngine->DocGetRootElement(xmldoc);

		processNode(xmlEngine, mainNode);

		// Release memory before exit
		xmlEngine->FreeDoc(xmldoc);
	}

	delete xmlEngine;

	m_configRead = true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Config::processNode(
	TXMLEngine* xmlEngine,
	XMLNodePointer_t node
) {
	// Check name
	const std::string name = xmlEngine->GetNodeName(node);

	// Process Attributes
	auto attr = xmlEngine->GetFirstAttr(node);
	while (attr) {
		processOption(name, xmlEngine->GetAttrName(attr),xmlEngine->GetAttrValue(attr));
		attr = xmlEngine->GetNextAttr(attr);
	}

	// Process children nodes
	auto child = xmlEngine->GetChild(node);
	while (child) {
		processNode(xmlEngine, child);
		child = xmlEngine->GetNext(child);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Config::processOption(
	const std::string nodeName,
	const std::string attribute,
	const std::string value
) {
	STD_LOG("Node: " << nodeName << " | Attr: " << attribute << " | Value: " << value);

	// Read mode of operation
	if ("mode" == attribute) {
		if ("QuickCheck" == value) {
			m_mode = RunMode::QuickCheck;
		} else if ("LowLevel" == value) {
			m_mode = RunMode::LowLevel;
		} else if ("Parallel" == value) {
			m_mode = RunMode::Parallel;
		} else if ("Serial" == value) {
			m_mode = RunMode::Serial;
		} else {
			WARNING("Unknown Config value for " << attribute << ": " << value);
			m_mode = RunMode::Serial;
		}
	}

	// Get Channel Mapping
	if (("channelMapping" == nodeName)&&("mapping" == attribute)) {
		m_channelMapping = value;
	}

	// Get Edge Modifier
	if (("edgeModifier" == nodeName)&&("modifier" == attribute)) {
		m_edgeModifier = value;
	}

	// Add exclusion to set
	if (("disableEdgeMatching" == nodeName)&&("channelID" == attribute)) {
		m_edgeMatchingExclusions.insert(stoui(value));
	}

	// Extract Readout IDs
	static unsigned int chainID = 0;
	static unsigned int deviceID = 0;

	static bool chainSet = false;
	static bool deviceSet = false;

	if (("chain" == nodeName)&&("id" == attribute)) {
		chainID = stoui(value);
		chainSet = true;
	}

	if (("device" == nodeName)&&("id" == attribute)) {
		deviceID = stoui(value);
		deviceSet = true;
		if (!chainSet) {
			WARNING("Device ID set before Chain ID. Check order in config file.");
		}
	}

	// Extract TDC IDs
	if ("tdcID1" == attribute || "tdcID2" == attribute) {
		if (!chainSet) {
			WARNING("TDC ID set before Chain ID. Check order in config file.");
		}
		if (!deviceSet) {
			WARNING("Device ID set before Chain ID. Check order in config file.");
		}

		const auto tdcid = stoui(value);
		const auto identifier = ReadoutIdentifier(BoardIdentifier(chainID, deviceID), tdcid);

		const auto found = std::find(m_readoutList.begin(), m_readoutList.end(), identifier);
		if (found == m_readoutList.end()) {
			m_readoutList.push_back(identifier);
		} else {
			WARNING(identifier << " is a duplicate");
		}

		// Record list of unique TDC IDs (used for buffer creation)
		if (std::find(m_tdcList.begin(), m_tdcList.end(), tdcid) == m_tdcList.end()) {
			m_tdcList.push_back(tdcid);
		}
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string Config::getModeAsString() const {
	if (RunMode::QuickCheck == m_mode) {
		return "QuickCheck";
	} else if (RunMode::LowLevel == m_mode) {
		return "LowLevel";
	} else if (RunMode::Serial == m_mode) {
		return "Serial";
	} else if (RunMode::Parallel == m_mode) {
		return "Parallel";
	} else {
		// Mode should always match one of the possible values
		ASSERT(false);
		return ""; // Prevents compiler warning
	}
}
