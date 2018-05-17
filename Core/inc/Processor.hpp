#ifndef PROCESSOR_H
#define PROCESSOR_H

// STD
#include <memory>
#include <unordered_map>
#include <string>
#include <list>
#include <array>
#include <set>

// LOCAL
#include "Config.hpp"
#include "ModesEnum.hpp"
#include "ErrorSpy.hpp"
#include "FileReader.hpp"
#include "ThreadSafeQueue.hpp"
#include "ThreadSafeEventMap.hpp"
#include "WordBundle.hpp"
#include "Packet.hpp"
#include "EventTreeManager.hpp"

//! Class which manages overall processing of data
/*!
	Is setup based on values stores in Config
*/
class Processor {
	using bundleBuffer = ThreadSafeQueue< std::unique_ptr<WordBundle> >;
	using packetBuffer = ThreadSafeQueue< std::unique_ptr<Packet> >;

public:
	//! Constructor
	Processor(
		std::unique_ptr<const Config> config
	);

	//! Destructor
	~Processor();

	void processFiles(
		const std::vector<std::string> fileNames
	);

private:
	//! Initilises a packet buffer for each TDC ID
	void initializePacketBuffers(
		const std::list<unsigned int>& tdcIDs
	);

	//! Runs QuickCheck Mode
	void runQuickCheck(
		const std::vector<std::string>& fileNames //!< List of files to process
	);

	//! Runs LowLevel Mode
	void runLowLevel(
		const std::vector<std::string>& fileNames //!< List of files to process
	);

	//! Runs Serial Mode
	void runSerial(
		const std::vector<std::string>& fileNames //!< List of files to process
	);

	//! Runs Parallel Mode
	void runParallel(
		const std::vector<std::string>& fileNames //!< List of files to process
	);

	//! Process word bundles into packets
	void makePackets(
		std::shared_ptr<bundleBuffer> wordBundleBuffer //!< Buffer to read from
	);

	//! Build Events from Packets
	void makeEvents();

	//! Writes events from the event buffer to a root file through the manager
	void writeEvents(
		std::unique_ptr<EventTreeManager>& manager
	);

private:
	const RunMode m_mode; //!< Run mode (set by config)

	const std::list<unsigned int> m_tdcIDs; //!< List of TDC IDs (set by config)

	std::unique_ptr<FileReader> m_fileReader = nullptr; //!< File Reader object pointer

	std::array< std::shared_ptr<bundleBuffer>, 4> m_wordBundleBuffers = {{ nullptr }}; //!< Word Bundle Buffers (one for each slot)

	std::unordered_map< unsigned int, packetBuffer> m_packetBuffers; //!< Packet Buffers

	ThreadSafeEventMap m_eventBuffer; //!< Event Buffer
};

#endif /* PROCESSOR_H */
