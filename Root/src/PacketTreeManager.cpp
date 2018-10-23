#include "PacketTreeManager.hpp"

// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PacketTreeManager::PacketTreeManager(
	std::shared_ptr<const Config> config,
	const std::string outfileName
) :
	RootManager(std::move(config),outfileName,"packet_tree")
{
	STD_LOG("PacketTreeManager Constructor Called");

	setUpBranches();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PacketTreeManager::add(
	std::unique_ptr<Packet> packet
) {
	ASSERT(nullptr != packet);
	ASSERT(nullptr != m_tree);

	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Write basic information
	b_tdcID = packet->getReadoutID().getTDCID();
	b_eventID = packet->getEventID();
	b_bunchID = packet->getBunchID();
	b_wordCount = packet->getWordCount();
	b_rocTime = packet->getROCValue();

	// Write leading edges
	const auto nLeadingEdges = packet->getNLeadingEdges();
	b_nLeadingEdges = 0;
	for (unsigned int i = 0; i < nLeadingEdges; i++) {
		b_leadingChannelID[b_nLeadingEdges] = packet->getChannelID(true,i);
		b_leadingTimestamp[b_nLeadingEdges] = packet->getTimestamp(true,i);
		b_leadingTDCBin[b_nLeadingEdges] = packet->getFineTimestamp(true,i);
		b_nLeadingEdges++;
	}
	ASSERT(nLeadingEdges == b_nLeadingEdges);

	// Write trailing edges
	const auto nTrailingEdges = packet->getNTrailingEdges();
	b_nTrailingEdges = 0;
	for (unsigned int i = 0; i < nTrailingEdges; i++) {
		b_trailingChannelID[b_nTrailingEdges] = packet->getChannelID(false,i);
		b_trailingTimestamp[b_nTrailingEdges] = packet->getTimestamp(false,i);
		b_trailingTDCBin[b_nTrailingEdges] = packet->getFineTimestamp(false,i);
		b_nTrailingEdges++;
	}
	ASSERT(nTrailingEdges == b_nTrailingEdges);

	// Fill Tree
	m_tree->Fill();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PacketTreeManager::setUpBranches() {
	ASSERT(nullptr != m_tree);

	// Lock Mutex as a precaution
	std::lock_guard<std::mutex> lk(m_mut);

	// Setup branches
	m_tree->Branch("tdcID", &b_tdcID, "tdcID/i");
	m_tree->Branch("eventID", &b_eventID, "eventID/i");
	m_tree->Branch("bunchID", &b_bunchID, "bunchID/i");
	m_tree->Branch("wordCount", &b_wordCount, "wordCount/i");
	m_tree->Branch("rocTime", &b_rocTime, "rocTime/i");

	m_tree->Branch("nLeadingEdges", &b_nLeadingEdges, "nLeadingEdges/i");
	setupArrBranch<UInt_t>("leadingChannelID", b_leadingChannelID, "[nLeadingEdges]/i", s_maxEdges);
	setupArrBranch<UInt_t>("leadingTimestamp", b_leadingTimestamp, "[nLeadingEdges]/i", s_maxEdges);
	setupArrBranch<UInt_t>("leadingTDCBin", b_leadingTDCBin, "[nLeadingEdges]/i", s_maxEdges);

	m_tree->Branch("nTrailingEdges", &b_nTrailingEdges, "nTrailingEdges/i");
	setupArrBranch<UInt_t>("trailingChannelID", b_trailingChannelID, "[nTrailingEdges]/i", s_maxEdges);
	setupArrBranch<UInt_t>("trailingTimestamp", b_trailingTimestamp, "[nTrailingEdges]/i", s_maxEdges);
	setupArrBranch<UInt_t>("trailingTDCBin", b_trailingTDCBin, "[nTrailingEdges]/i", s_maxEdges);
}
