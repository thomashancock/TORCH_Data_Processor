#ifndef PACKETTREEMANAGER_H
#define PACKETTREEMANAGER_H

// LOCAL
#include "RootManager.hpp"
#include "Packet.hpp"
#include "Config.hpp"

//! Class which manages storage of Packet data in root file output
class PacketTreeManager : public RootManager<Packet> {
public:
	//! Constructor
	PacketTreeManager(
		std::shared_ptr<const Config> config, //!< Ptr to program configuration
		const std::string outfileName //!< The desired output file name
	);

	//! Adds a packet to the tree, then destroys the packet
	/*!
		Overload of RootManager add.
	 */
	void add(
		std::unique_ptr<Packet> packet
	) final;

private:
	//! Sets up the branches of the tree
	void setUpBranches();

private:
	constexpr static unsigned int s_maxEdges = 100; //!< Array size for edge branches

	// Set branch variables
	UInt_t b_chainID = 0;         //!< Branch Variable: TDC ID
	UInt_t b_deviceID = 0;         //!< Branch Variable: TDC ID
	UInt_t b_tdcID = 0;         //!< Branch Variable: TDC ID
	UInt_t b_eventID = 0;       //!< Branch Variable: Event ID
	UInt_t b_bunchID = 0;       //!< Branch Variable: Bunch ID
	UInt_t b_wordCount = 0;     //!< Branch Variable: Word Count
	UInt_t b_rocTime = 0;       //!< Branch Variable: ROC time
	UInt_t b_nLeadingEdges = 0; //!< Branch Variable: Number of leading edges
	UInt_t* b_leadingChannelID = nullptr; //!< Branch Variable [nLeadingEdges]: Channel ID
	UInt_t* b_leadingTimestamp = nullptr; //!< Branch Variable [nLeadingEdges]: Timestamp
	UInt_t* b_leadingTDCBin = nullptr; //!< Branch Variable [nLeadingEdges]: Timestamp TDC Bin

	UInt_t b_nTrailingEdges = 0; //!< Branch Variable: Number of trailing edges
	UInt_t* b_trailingChannelID = nullptr; //!< Branch Variable [nTrailingEdges]: Channel ID
	UInt_t* b_trailingTimestamp = nullptr; //!< Branch Variable [nTrailingEdges]: Timestamp
	UInt_t* b_trailingTDCBin = nullptr; //!< Branch Variable [nTrailingEdges]: Timestamp TDC Bin
};

#endif /* PACKETTREEMANAGER_H */
