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
	std::shared_ptr<const Config> config,
	const std::string outfileName,
	const unsigned int nTDCs
) :
	RootManager(std::move(config),outfileName,"event_tree"),
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

	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	const unsigned int eventID = event->getEventID();

	// Reset count variables
	b_nTDCs = 0;
	b_nEdges = 0;
	b_nHits = 0;

	// Add packets to tree
	const auto packets = event->removePackets();
	for (const auto& packet : packets) {
		// Check packet pointer is valid
		ASSERT(packet != nullptr);
		// Only good packets should reach this point
		ASSERT(packet->isGood());

		// Fill tree branches from packet
		ASSERT(b_nTDCs < m_nTDCs);
		b_tdcID[b_nTDCs] = packet->getTDCID();
		b_eventID[b_nTDCs] = eventID;
		b_bunchID[b_nTDCs] = packet->getBunchID();
		b_rocTime[b_nTDCs] = packet->getROCValue();
		b_nHeaders[b_nTDCs] = 1;
		b_nLeadingEdges[b_nTDCs] = packet->getNLeadingEdges();
		b_nTrailingEdges[b_nTDCs] = packet->getNTrailingEdges();
		b_nTrailers[b_nTDCs] = 1;
		b_wordCount[b_nTDCs] = packet->getWordCount();

		// Add edges to total sum
		b_nEdges += b_nLeadingEdges[b_nTDCs];
		b_nEdges += b_nTrailingEdges[b_nTDCs];
		b_nTDCs++;

		// Write edge information to file
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
				b_width[b_nHits] = static_cast<int>(trailingTimes.at(iPair)) - static_cast<int>(leadingTimes.at(iPair));
				b_nHits++;
			}
		}
	}

	// Fill Tree
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
	m_tree->Branch("num_tdcs", &b_nTDCs, "num_tdcs/i");
	setupArrBranch<UInt_t>("tdc_id", b_tdcID, "[num_tdcs]/i", s_hitsMax);
	setupArrBranch<UInt_t>("event_id", b_eventID, "[num_tdcs]/i", s_hitsMax);
	setupArrBranch<UInt_t>("tdc_time", b_bunchID, "[num_tdcs]/i", s_hitsMax);
	setupArrBranch<UInt_t>("roc_time", b_rocTime, "[num_tdcs]/i", s_hitsMax);
	setupArrBranch<UInt_t>("num_headers", b_nHeaders, "[num_tdcs]/i", s_hitsMax);
	setupArrBranch<UInt_t>("num_leading_edges", b_nLeadingEdges, "[num_tdcs]/i", s_hitsMax);
	setupArrBranch<UInt_t>("num_trailing_edges", b_nTrailingEdges, "[num_tdcs]/i", s_hitsMax);
	setupArrBranch<UInt_t>("num_trailers", b_nTrailers, "[num_tdcs]/i", s_hitsMax);
	setupArrBranch<UInt_t>("word_count", b_wordCount, "[num_tdcs]/i", s_hitsMax);

	m_tree->Branch("num_edges_total", &b_nEdges, "num_edges_total/i");

	m_tree->Branch("num_hits", &b_nHits, "num_hits/i");
	setupArrBranch<UInt_t>("channel_id", b_channelID, "[num_hits]/i", m_nTDCs);
	setupArrBranch<UInt_t>("leading_time", b_leadingTime, "[num_hits]/i", m_nTDCs);
	setupArrBranch<UInt_t>("trailing_time", b_trailingTime, "[num_hits]/i", m_nTDCs);
	setupArrBranch<UInt_t>("leading_time_tdc_bin", b_leadingTimeFine, "[num_hits]/i", m_nTDCs);
	setupArrBranch<UInt_t>("trailing_time_tdc_bin", b_trailingTimeFine, "[num_hits]/i", m_nTDCs);
	setupArrBranch<Int_t>("width", b_width, "[num_hits]/I", m_nTDCs);
}
