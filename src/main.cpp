// STD
#include <string>
#include <vector>
#include <memory>

// LOCAL
#include "Debug.hpp"
#include "Config.hpp"
#include "Processor.hpp"

void parseInputs(
	const int argc,
	char** argv,
	std::string& configFile,
	std::string& outputFile,
	std::vector<std::string>& fileNames
) {
	ASSERT(fileNames.empty());

	int opIndex = 1;
	while (opIndex < argc) {
		if (argv[opIndex][0]=='-') {
			STD_LOG("Parsing '" << argv[opIndex] << "'");
			std::string opStr = argv[opIndex];
			if ("-con" == opStr) {
				opIndex++;
				configFile = argv[opIndex];
			}
			if ("-out" == opStr) {
				opIndex++;
				outputFile = argv[opIndex];
			}
		} else {
			const std::string inFileName = argv[opIndex];
			fileNames.push_back(inFileName);
		}
		opIndex++;
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char** argv) {
	std::cout << "Program Build: " << GITVERSION << std::endl;

	// Set Default location for file
	std::string configFile = "./Config.xml";
	std::string outputFile = "./TORCH_Tree.root";

	// Create input file vector
	std::vector<std::string> fileNames;

	// Parse input options
	parseInputs(argc,argv,configFile,outputFile,fileNames);

	// Read Config
	auto config = std::make_unique<const Config>(configFile);

	if (config->isConfigured()) {

		// Sort files to ensure events come together
		// TODO

		// Perform Data Processing
		STD_LOG("Processing Data");
		Processor processor(std::move(config));
		ASSERT(nullptr == config);

		try {
			processor.processFiles(fileNames);
		} catch (std::exception& e) {
			STD_ERR("Exception: " << e.what());
		}
	}

	return 0;
}
