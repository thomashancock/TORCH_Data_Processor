#ifndef PROCESSOR_H
#define PROCESSOR_H

// STD
#include <memory>
#include <unordered_map>
#include <string>
#include <atomic>
#include <list>
#include <array>
#include <set>

// LOCAL
#include "Config.hpp"
#include "ModesEnum.hpp"
#include "ThreadSafeQueue.hpp"
#include "ThreadSafeEventMap.hpp"
#include "WordBundle.hpp"
#include "Packet.hpp"

//! Class which manages overall processing of data
/*!
	Is setup based on values stores in Config
*/
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
	//! Initilises a packet buffer for each TDC ID
	void initializePacketBuffers(
		const std::list<unsigned int>& tdcIDs
	);

	//! Make Word Bundles (has ROC value and words found preceding it)
	void processFile(
		const std::string fileName
	);

	//! Reads Header Line, storing information in readoutBoardNumber and nDataBytes
	void readHeaderLine(
		std::ifstream& inputData,
		unsigned int& readoutBoardNumber,
		unsigned int& nDataBytes
	);

	//! Reads a data block, returning the words in a std::array object
	std::array<unsigned int,4> readDataBlock(
		std::ifstream& inputData
	);

	//! Process word bundles into packets
	void makePackets();

	//! Build Events from Packets
	void makeEvents();

	//! Write Events to ROOT file

private:
	const RunMode m_mode; //!< Run mode (set by config)

	const std::list<unsigned int> m_tdcIDs; //!< List of TDC IDs (set by config)
	// const std::list<unsigned int> m_tdcIDs { 8, 9, 10, 11, 12, 13, 14, 15 };

	const std::set<unsigned int> fillerWords = { 0xA0A0A0A0, 0xB0B0B0B0, 0xC0C0C0C0, 0xD0D0D0D0 }; //! Set containing filler words (move to config?)

	ThreadSafeQueue< std::unique_ptr<WordBundle> > m_wordBundleBuffer; //!< WordBundle Buffers

	std::unordered_map< unsigned int, packetBuffer> m_packetBuffers; //!< Packet Buffers

	ThreadSafeEventMap m_eventBuffer; //!< Event Buffer
};

#endif /* PROCESSOR_H */
