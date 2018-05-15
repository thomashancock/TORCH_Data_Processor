#include "Config.hpp"

// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Config::Config(
	const std::string configFile
) {
	STD_LOG("Config(): Reading Config");

	ASSERT(m_tdcList.empty());

	parseConfigFile(configFile);

	// Sort TDC ID list
	m_tdcList.sort();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Config::print() const {
	std::cout << "Configuration:" << std::endl;
	std::cout << "\tMode: " << this->getModeAsString() << std::endl;
	std::cout << "\tTDC IDs: ";
	for (const auto& id : m_tdcList) {
		std::cout << id << " ";
	}
	std::cout << std::endl;
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
		processOption(xmlEngine->GetAttrName(attr),xmlEngine->GetAttrValue(attr));
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
	const std::string attribute,
	const std::string value
) {
	STD_LOG("Adding attr: " << attribute << " value: " << value);

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

	if ("tdcID1" == attribute || "tdcID2" == attribute) {
		const auto id = static_cast<unsigned int>(std::stoi(value));
		const auto found = std::find(m_tdcList.begin(), m_tdcList.end(), id);
		if (found == m_tdcList.end()) {
			m_tdcList.push_back(id);
		} else {
			WARNING("TDC ID " << id << " is a duplicate");
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
