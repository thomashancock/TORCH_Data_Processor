#include "EventTreeManager.hpp"

// LOCAL
#include "Debug.hpp"

// Useful alias
using pairVectors = std::pair< std::vector< unsigned int >, std::vector< unsigned int > >;

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
	ASSERT(nullptr != event);
	ASSERT(nullptr != m_tree);

	const unsigned int eventID = event->getEventID();

	resetTreeVariables(); // Reset count variables
	unsigned int tdcEntryNo = 0;
	const auto packets = event->removePackets();
	for (const auto& packet : packets) {
		// STD_LOG("LOOP TRIGGER");
		// Check packet pointer is valid
		ASSERT(packet != nullptr);
		// Only good packets should reach this point
		ASSERT(packet->isGood());

		// Fill tree branches from packet
		// STD_LOG("FILLING TDC BRANCHES");
		ASSERT(tdcEntryNo < m_nTDCs);
		ASSERT(nullptr != b_tdcID);
		b_tdcID[tdcEntryNo] = packet->getTDCID();
		// STD_LOG("FILLING TDC BRANCHES");
		b_eventID[tdcEntryNo] = eventID;
		b_bunchID[tdcEntryNo] = packet->getBunchID();
		b_rocTime[tdcEntryNo] = packet->getROCValue();
		b_nHeaders[tdcEntryNo] = 1;
		b_nLeadingEdges[tdcEntryNo] = packet->getNLeadingEdges();
		b_nTrailingEdges[tdcEntryNo] = packet->getNTrailingEdges();
		b_nTrailers[tdcEntryNo] = 1;
		b_wordCount[tdcEntryNo] = packet->getWordCount();

		// Add edges to total sum
		// STD_LOG("ADDING EDGE COUNT");
		b_nEdges += b_nLeadingEdges[tdcEntryNo];
		b_nEdges += b_nTrailingEdges[tdcEntryNo];
		tdcEntryNo++;

		// Write edge information to file
		// STD_LOG("Writing Edge Information");
		std::map< unsigned int, pairVectors > edgeMap;

		auto addEdgesToMap = [&packet, &edgeMap] (
			const bool leading
		) {
			ASSERT(packet != nullptr);
			const unsigned int nEdges = (true == leading) ?
				packet->getNLeadingEdges() :
				packet->getNTrailingEdges() ;
			for (unsigned int iEdge = 0; iEdge < nEdges; iEdge++) {
				const unsigned int channelID = packet->getChannelID(leading,iEdge);
				const unsigned int timestamp = packet->getTimestamp(leading,iEdge);

				auto itr = edgeMap.find(channelID);
				if (itr == edgeMap.end()) {
					pairVectors newPair;
					if (true == leading) {
						newPair.first.push_back(timestamp);
					} else {
						newPair.second.push_back(timestamp);
					}
					if (!edgeMap.insert(
						std::pair< unsigned int, pairVectors > (channelID,newPair)
						).second) {
						STD_ERR("Unable to insert new vector pair");
					}
				} else {
					if (true == leading) {
						itr->second.first.push_back(timestamp);
					} else {
						itr->second.second.push_back(timestamp);
					}
				}
			}
		};

		addEdgesToMap(true);
		addEdgesToMap(false);

		for (auto& entry : edgeMap) {
			const unsigned int channelID = entry.first;

			// lvalue = entry.pairVector.vectorInPair
			std::vector<unsigned int>& leadingTimes = entry.second.first;
			std::vector<unsigned int>& trailingTimes = entry.second.second;

			// Sort Edges
			std::sort(leadingTimes.begin(),leadingTimes.end());
			std::sort(trailingTimes.begin(),trailingTimes.end());

			// Get size of smallest vector
			const unsigned int nPairs = (leadingTimes.size() > trailingTimes.size()) ? trailingTimes.size() : leadingTimes.size();

			// Write times until one vector is exhausted
			for (unsigned int iPair = 0; iPair < nPairs; iPair++) {
				ASSERT(b_nHits < s_hitsMax);
				b_channelID[b_nHits] = channelID;
				b_leadingTime[b_nHits] = leadingTimes.at(iPair);
				b_trailingTime[b_nHits] = trailingTimes.at(iPair);
				b_leadingTimeFine[b_nHits] = leadingTimes.at(iPair)%256;
				b_trailingTimeFine[b_nHits] = trailingTimes.at(iPair)%256;
				b_width[b_nHits] = trailingTimes.at(iPair) - leadingTimes.at(iPair);
				b_nHits++;
			}
		}
	}

	// Fill Trees
	m_tree->Fill();
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

	// Setup branches
	m_tree->Branch("num_tdcs", &b_nTDCs, "num_tdcs/I");
	setupArrBranch<Int_t>("tdc_id", b_tdcID, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch<Int_t>("event_id", b_eventID, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch<Int_t>("tdc_time", b_bunchID, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch<Int_t>("roc_time", b_rocTime, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch<Int_t>("num_headers", b_nHeaders, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch<Int_t>("num_leading_edges", b_nLeadingEdges, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch<Int_t>("num_trailing_edges", b_nTrailingEdges, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch<Int_t>("num_trailers", b_nTrailers, "[num_tdcs]/I", s_hitsMax);
	setupArrBranch<Int_t>("word_count", b_wordCount, "[num_tdcs]/I", s_hitsMax);

	m_tree->Branch("num_edges_total", &b_nEdges, "num_edges_total/I");

	m_tree->Branch("num_hits", &b_nHits, "num_hits/I");
	setupArrBranch<Int_t>("channel_id", b_channelID, "[num_hits]/I", m_nTDCs);
	setupArrBranch<Int_t>("leading_time", b_leadingTime, "[num_hits]/I", m_nTDCs);
	setupArrBranch<Int_t>("trailing_time", b_trailingTime, "[num_hits]/I", m_nTDCs);
	setupArrBranch<Int_t>("leading_time_tdc_bin", b_leadingTimeFine, "[num_hits]/I", m_nTDCs);
	setupArrBranch<Int_t>("trailing_time_tdc_bin", b_trailingTimeFine, "[num_hits]/I", m_nTDCs);
	setupArrBranch<Int_t>("width", b_width, "[num_hits]/I", m_nTDCs);

	resetTreeVariables();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EventTreeManager::resetTreeVariables() {
	b_nTDCs = static_cast<Int_t>(m_nTDCs);
	b_nEdges = 0;
	b_nHits = 0;
}
