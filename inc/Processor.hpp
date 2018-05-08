#ifndef PROCESSOR_H
#define PROCESSOR_H

// STD
#include <memory>

// LOCAL
#include "Config.hpp"

class Processor {
public:
	Processor(
		std::unique_ptr<const Config> config
	);

	void processFiles(
		const std::vector<std::string> fileNames
	);

private:
	//! Make Word Bundles (has ROC value and words found preceding it)

	//! Process Word Bundle into packets

	//! Build Events from Packets

	//! Write Events to ROOT file

private:
	std::unique_ptr<const Config> m_config;
};

#endif /* PROCESSOR_H */
