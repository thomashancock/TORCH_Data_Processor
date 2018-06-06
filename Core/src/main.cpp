// STD
#include <string>
#include <vector>
#include <memory>
#include <fstream>

// LOCAL
#include "Debug.hpp"
#include "Config.hpp"
#include "Processor.hpp"

void addFilesFromList(
	const std::string listFile,
	std::vector<std::string>& fileNames
) {
	std::cout << "Adding files from list file: " << listFile << std::endl;
	std::ifstream input(listFile);
	std::string line = "";
	while (getline(input, line)) {
		fileNames.push_back(line);
	}
	input.close();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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
			if ("-list" == opStr) {
				opIndex++;
				addFilesFromList(argv[opIndex],fileNames);
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
	std::cout << "Program Build: " << GITTAG << std::endl;

	// Set Default location for file
	std::string configFile = "./Config.xml";
	std::string outputFile = "./Output.root";

	// Create input file vector
	std::vector<std::string> fileNames;

	// Parse input options
	parseInputs(argc,argv,configFile,outputFile,fileNames);

	// Read Config
	auto config = std::make_unique<const Config>(configFile);

	if (config->isConfigured()) {
		// Perform Data Processing
		STD_LOG("Processing Data");
		Processor processor(std::move(config));
		ASSERT(nullptr == config);

		try {
			processor.processFiles(outputFile,fileNames);
		} catch (std::exception& e) {
			STD_ERR("Exception: " << e.what());
		}
	}

	return 0;
}
