#ifndef PROCESSOR_H
#define PROCESSOR_H

// STD
#include <memory>
#include <unordered_map>
#include <string>
#include <atomic>
#include <list>

// LOCAL
#include "Config.hpp"
#include "ThreadSafeQueue.hpp"
#include "WordBundle.hpp"
#include "Packet.hpp"
#include "Event.hpp"

class Processor {
	using packetBuffer = ThreadSafeQueue< std::unique_ptr<Packet> >;

public:
	Processor(
		std::unique_ptr<const Config> config
	);

	void processFiles(
		const std::vector<std::string> fileNames
	);

private:
	void initializePacketBuffers(
		const std::list<unsigned int>& tdcIDs
	);

	//! Make Word Bundles (has ROC value and words found preceding it)
	void processFile(
		const std::string fileName
	);

	//! Process Word Bundle into packets

	//! Build Events from Packets

	//! Write Events to ROOT file

private:
	std::unique_ptr<const Config> m_config; //!< Pointer to configuration object

	ThreadSafeQueue<WordBundle> m_wordBundleBuffer; //!< WordBundle Buffers

	std::unordered_map< unsigned int, std::atomic_bool > m_packetBufferFlags; //!< Packet buffers flags
	std::unordered_map< unsigned int, packetBuffer> m_packetBuffers; //!< Packet Buffers

	std::atomic_flag m_eventFlag; //!< Event Readout Flag
	ThreadSafeQueue< std::unique_ptr<Event> > m_eventBuffer; //!< Event Buffer

	//!< RootManager
};

#endif /* PROCESSOR_H */
