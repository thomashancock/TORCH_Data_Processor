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

	parseConfigFile(configFile);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Config::print() const {
	std::cout << "Configuration:" << std::endl;

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
	STD_LOG("Processing " << name << " node");

	// Process Attributes
	auto attr = xmlEngine->GetFirstAttr(node);
	while (attr) {
		STD_LOG("attr: " << xmlEngine->GetAttrName(attr) << " value: " << xmlEngine->GetAttrValue(attr));
		attr = xmlEngine->GetNextAttr(attr);
	}

	// Process children nodes
	auto child = xmlEngine->GetChild(node);
	while (child) {
		processNode(xmlEngine, child);
		child = xmlEngine->GetNext(child);
	}
}
