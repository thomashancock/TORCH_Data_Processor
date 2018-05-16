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

private:
	const RunMode m_mode; //!< Run mode (set by config)

	const std::list<unsigned int> m_tdcIDs; //!< List of TDC IDs (set by config)

	// const std::set<unsigned int> fillerWords = { 0xA0A0A0A0, 0xB0B0B0B0, 0xC0C0C0C0, 0xD0D0D0D0 }; //! Set containing filler words (move to config?)

	std::shared_ptr<ErrorSpy> m_errorSpy;

	std::unique_ptr<FileReader> m_fileReader = nullptr; //!< File Reader object pointer

	std::array< std::shared_ptr<bundleBuffer>, 4> m_wordBundleBuffers = {{ nullptr }}; //!< Word Bundle Buffers (one for each slot)

	std::unordered_map< unsigned int, packetBuffer> m_packetBuffers; //!< Packet Buffers

	ThreadSafeEventMap m_eventBuffer; //!< Event Buffer
};

#endif /* PROCESSOR_H */
