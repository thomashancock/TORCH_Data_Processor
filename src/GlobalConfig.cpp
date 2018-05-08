#include "GlobalConfig.hpp"

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
GlobalConfig::GlobalConfig() {
	STD_LOG("GlobalConfig(): Default Constructor Called");

	m_tdcList = std::shared_ptr< std::vector<unsigned int> > (new std::vector<unsigned int>);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GlobalConfig::GlobalConfig(
	std::string configFile
) :
GlobalConfig()
{
	STD_LOG("GlobalConfig(): Constructor Called");

	std::ifstream file(configFile);

	if (file) {
		// Parse config file
		std::string line;
		while (std::getline(file, line)) {
			std::istringstream is_line(line);
			std::string key;
			if (std::getline(is_line, key, '=')) {
				std::string value;
				if (std::getline(is_line, value)) {
					parseOption(key,value);
				}
			}
		}

		if (0 == m_tdcList->size()) {
			STD_ERR("No valid TDC IDs provided. Program will exit.");
			exit(0);
		} else {
			std::sort(m_tdcList->begin(),m_tdcList->end());
		}
	} else {
		std::cout << "No config file provided. Will use default config." << std::endl;
		m_tdcList->push_back(8);
		m_tdcList->push_back(9);
		m_tdcList->push_back(10);
		m_tdcList->push_back(11);
		m_tdcList->push_back(12);
		m_tdcList->push_back(13);
		m_tdcList->push_back(14);
		m_tdcList->push_back(15);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GlobalConfig::~GlobalConfig() {
	STD_LOG("GlobalConfig(): Deconstructor Called");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GlobalConfig::printConfig() {
	if (true == m_debugOutput) {
		std::cout << "Debug Output Requested" << std::endl;
	}

	std::cout << "Will run with the following TDC IDs:" << std::endl;
	for (auto& id : *m_tdcList) {
		std::cout << id << ", ";
	}
	std::cout << std::endl;

	if (true == m_lowLevelOnly) {
		std::cout << "Will only produce low level output" << std::endl;
	}

	if (true == m_skipCompleteEventCheck) {
		std::cout << "Will skip complete event check" << std::endl;
	}

	if (true == m_channelFix2017) {
		std::cout << "Bad channel fix for 2017 data will be applied" << std::endl;
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void GlobalConfig::parseOption(
	std::string key,
	std::string value
) {
	if ('#' == key.at(0)) {
		return;
	}

	STD_LOG("GlobalConfig(): Parsing " << key << ". Value " << value);

	if("DebugOutput" == key) {
		if (("1" == value) || ("true" == value) || ("True" == value)) {
			m_debugOutput = true;
		} else {
			m_debugOutput = false;
		}
	}

	if("AddTDCID" == key) {
		const int tdcID = std::atoi(value.c_str());
		if (tdcID < 0) {
			STD_ERR("Invalid TDC ID found in config file. Please correct.");
			exit(0);
		} else {
			ASSERT(tdcID < 0x7FFFFFFF);
			m_tdcList->push_back((unsigned int) tdcID);
		}
	}

	if("LowLevelOnly" == key) {
		if (("1" == value) || ("true" == value) || ("True" == value)) {
			m_lowLevelOnly = true;
		} else {
			m_lowLevelOnly = false;
		}
	}

	if("SkipCompleteEventCheck" == key) {
		if (("1" == value) || ("true" == value) || ("True" == value)) {
			m_skipCompleteEventCheck = true;
		} else {
			m_skipCompleteEventCheck = false;
		}
	}

	if("ChannelFix2017" == key) {
		if (("1" == value) || ("true" == value) || ("True" == value)) {
			m_channelFix2017 = true;
		} else {
			m_channelFix2017 = false;
		}
	}
}
