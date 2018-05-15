#include "EventTreeManager.hpp"

// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
EventTreeManager::EventTreeManager(
	const std::string outfileName,
	const unsigned int nTDCs
) :
	RootManager(outfileName,"event_tree"),
	m_nTDCs(nTDCs)
{
	STD_LOG("EventTreeManager Constructor Called");

	setUpBranches();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EventTreeManager::add(
	std::unique_ptr<Event> event
) {
	event->print();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void EventTreeManager::setUpBranches() {
	ASSERT(nullptr != m_tree);

	// Lock Mutex as a precaution
	std::lock_guard<std::mutex> lk(m_mut);

	auto setupArrBranch = [this] (
		const std::string branchName,
		Int_t* arrayPtr,
		const std::string branchExtension,
		const unsigned int arraySize
	) -> void {
		const auto leafList = branchName + branchExtension;
		arrayPtr = new Int_t[arraySize] { 0 };
		m_tree->Branch(branchName.c_str(),arrayPtr,leafList.c_str());
	};

	// Setup branches
	m_tree->Branch("num_tdcs", &b_nTDCs, "num_tdcs/I");
	// m_tree->Branch("tdc_id", b_tdcID, "tdc_id[num_tdcs]/I");
	// m_tree->Branch("event_id", b_eventID, "event_id[num_tdcs]/I");
	// m_tree->Branch("tdc_time", b_bunchID, "tdc_time[num_tdcs]/I");
	// m_tree->Branch("roc_time", b_rocTime, "roc_time[num_tdcs]/I");
	// m_tree->Branch("num_headers", b_nHeaders, "num_headers[num_tdcs]/I");
	// m_tree->Branch("num_leading_edges", b_nLeadingEdges, "num_leading_edges[num_tdcs]/I");
	// m_tree->Branch("num_trailing_edges", b_nTrailingEdges, "num_trailing_edges[num_tdcs]/I");
	// m_tree->Branch("num_trailers", b_nTrailers, "num_trailers[num_tdcs]/I");
	// m_tree->Branch("word_count", b_wordCount, "word_count[num_tdcs]/I");
	setupArrBranch("tdc_id", b_tdcID, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch("event_id", b_eventID, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch("tdc_time", b_bunchID, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch("roc_time", b_rocTime, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch("num_headers", b_nHeaders, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch("num_leading_edges", b_nLeadingEdges, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch("num_trailing_edges", b_nTrailingEdges, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch("num_trailers", b_nTrailers, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch("word_count", b_wordCount, "[num_tdcs]/I", s_hitsMax);

	m_tree->Branch("num_edges_total", &b_nEdges, "num_edges_total/I");

	m_tree->Branch("num_hits", &b_nHits, "num_hits/I");
	setupArrBranch("channel_id", b_channelID, "[num_hits]/I", m_nTDCs);
	setupArrBranch("leading_time", b_leadingTime, "[num_hits]/I", m_nTDCs);
	setupArrBranch("trailing_time", b_trailingTime, "[num_hits]/I", m_nTDCs);
	setupArrBranch("leading_time_tdc_bin", b_leadingTimeFine, "[num_hits]/I", m_nTDCs);
	setupArrBranch("trailing_time_tdc_bin", b_trailingTimeFine, "[num_hits]/I", m_nTDCs);
	setupArrBranch("width", b_width, "[num_hits]/I", m_nTDCs);
	// m_tree->Branch("channel_id", b_channelID, "channel_id[num_hits]/I");
	// m_tree->Branch("leading_time", b_leadingTime, "leading_time[num_hits]/I");
	// m_tree->Branch("trailing_time", b_trailingTime, "trailing_time[num_hits]/I");
	// m_tree->Branch("leading_time_tdc_bin", b_leadingTimeFine, "leading_time_tdc_bin[num_hits]/I");
	// m_tree->Branch("trailing_time_tdc_bin", b_trailingTimeFine, "trailing_time_tdc_bin[num_hits]/I");
	// m_tree->Branch("width", b_width, "width[num_hits]/I");

	resetTreeVariables();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EventTreeManager::resetTreeVariables() {
	b_nTDCs = m_nTDCs;
	b_nEdges = 0;
	b_nHits = 0;
}
