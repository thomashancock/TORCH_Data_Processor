#ifndef PACKETTREEMANAGER_H
#define PACKETTREEMANAGER_H

// LOCAL
#include "RootManager.hpp"
#include "Packet.hpp"

class PacketTreeManager : public RootManager<Packet> {
public:
	PacketTreeManager(
		const std::string outfileName
	);

	void add(
		std::unique_ptr<Packet> packet
	) final;

private:
	void setUpBranches();

	void resetTreeVariables();

private:
	constexpr static unsigned int s_maxEdges = 100;

	// Set branch variables
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
