#include "Processor.hpp"

// STD
#include <fstream>
#include <ios>

// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Processor::Processor(
	std::unique_ptr<const Config> config
) :
	m_config(std::move(config))
{
	// Assert move operation was succesful
	ASSERT(nullptr == config);

	m_config->print();


}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Processor::processFiles(
	const std::vector<std::string> fileNames
) {
	// SEQUENTIAL VERSION
	STD_LOG_VAR(fileNames.size());
	for (auto& file : fileNames) {
		// Read File into WordBundle buffer
		processFile(file);

		// Once all WordBundle buffers are !empty
		// Make Packets from each buffer

		// Once all PacketBuffers are flagged ready, make events

		// Write Events to Root File

	}

	// PARALLEL VERSION
	// for (auto& file : fileNames) {
	//
	// }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Processor::processFile(
	const std::string fileName
) {
	std::ifstream inputData(fileName, std::ios::in | std::ios::binary);

	if (!inputData.good()) {
		return;
	}

	inputData.seekg(0, std::ios::end);
	const unsigned int fileLength = inputData.tellg();
	inputData.seekg(0);

	while (inputData.good()) {
		if (fileLength == inputData.tellg()) {
			STD_LOG("End of file reached");
			break;
		}

		inputData.get();
	}
}
