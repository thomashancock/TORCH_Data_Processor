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

	// Spoof Config Settings
	// "0A" = 10, 11
	// "0B" = 8, 9
	// "0C" = 12, 13
	// "0D" = 14, 15

	// Initialise Packet Buffers
	std::list<unsigned int> tdcIDs { 8, 9, 10, 11, 12, 13, 14, 15 };
	initializePacketBuffers(tdcIDs);
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
void Processor::initializePacketBuffers(
	const std::list<unsigned int>& tdcIDs
) {
	for (const auto& id : tdcIDs) {
		// Initialise Packet Buffer
		// ThreadSafeQueue takes no constructor arguments, so can construct simply by accessing the entry
		m_packetBuffers[id];

		// Initialise Packet Buffer Flags
		// atomic_bool cannot be copied or moved, so must construct in place
		m_packetBufferFlags[id] = false;
	}
	ASSERT(m_packetBuffers.size() == tdcIDs.size());
	ASSERT(m_packetBuffers.size() == m_packetBufferFlags.size());
}
// -----------------------------------------------------------------------------
//
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
