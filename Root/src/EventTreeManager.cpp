#include "EventTreeManager.hpp"

// STD
#include <algorithm>

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
	RootManager(config,outfileName,"event_tree"),
	m_edgeMatchingExclusions(config->getEdgeMatchingExclusions()),
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

	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	const unsigned int eventID = event->getEventID();

	// Must check is complete before packets are removed!
	b_isComplete = event->isComplete();
	b_wasDumped = event->wasDumped();

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
		ASSERT(nullptr != b_tdcID);
		b_tdcID[b_nTDCs] = packet->getTDCID();
		ASSERT(nullptr != b_eventID);
		b_eventID[b_nTDCs] = eventID;
		ASSERT(nullptr != b_bunchID);
		b_bunchID[b_nTDCs] = packet->getBunchID();
		ASSERT(nullptr != b_rocTime);
		b_rocTime[b_nTDCs] = packet->getROCValue();
		ASSERT(nullptr != b_nHeaders);
		b_nHeaders[b_nTDCs] = 1;
		ASSERT(nullptr != b_nLeadingEdges);
		b_nLeadingEdges[b_nTDCs] = packet->getNLeadingEdges();
		ASSERT(nullptr != b_nTrailingEdges);
		b_nTrailingEdges[b_nTDCs] = packet->getNTrailingEdges();
		ASSERT(nullptr != b_nTrailers);
		b_nTrailers[b_nTDCs] = 1;
		ASSERT(nullptr != b_wordCount);
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

			if (m_edgeMatchingExclusions.end() != m_edgeMatchingExclusions.find(channelID)) {
				// If channel ID is excluded from edge matching, only write leading edges
				for (auto& edgeTime : leadingTimes) {
					ASSERT(b_nHits < s_hitsMax);
					ASSERT(nullptr != b_channelID);
					b_channelID[b_nHits] = channelID;
					ASSERT(nullptr != b_leadingTime);
					b_leadingTime[b_nHits] = edgeTime;
					ASSERT(nullptr != b_trailingTime);
					b_trailingTime[b_nHits] = 0;
					ASSERT(nullptr != b_leadingTimeFine);
					b_leadingTimeFine[b_nHits] = edgeTime%256;
					ASSERT(nullptr != b_trailingTimeFine);
					b_trailingTimeFine[b_nHits] = 0;
					ASSERT(nullptr != b_width);
					b_width[b_nHits] = 0;
					b_nHits++;
				}
			} else {
				// Else find pairs of edges and only write matching pairs
				// Get size of smallest vector
				const unsigned int nPairs = (leadingTimes.size() > trailingTimes.size()) ? trailingTimes.size() : leadingTimes.size();

				// Write times until one vector is exhausted
				for (unsigned int iPair = 0; iPair < nPairs; iPair++) {
					ASSERT(b_nHits < s_hitsMax);
					ASSERT(nullptr != b_channelID);
					b_channelID[b_nHits] = channelID;
					ASSERT(nullptr != b_leadingTime);
					b_leadingTime[b_nHits] = leadingTimes.at(iPair);
					ASSERT(nullptr != b_trailingTime);
					b_trailingTime[b_nHits] = trailingTimes.at(iPair);
					ASSERT(nullptr != b_leadingTimeFine);
					b_leadingTimeFine[b_nHits] = leadingTimes.at(iPair)%256;
					ASSERT(nullptr != b_trailingTimeFine);
					b_trailingTimeFine[b_nHits] = trailingTimes.at(iPair)%256;
					ASSERT(nullptr != b_width);
					b_width[b_nHits] = static_cast<int>(trailingTimes.at(iPair)) - static_cast<int>(leadingTimes.at(iPair));
					b_nHits++;
				}
			}
		}
	}

	ASSERT(nullptr != m_tree);
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
	m_tree->Branch("isComplete", &b_isComplete, "isComplete/O");
	m_tree->Branch("wasDumped", &b_wasDumped, "wasDumped/O");

	m_tree->Branch("nTDCs", &b_nTDCs, "nTDCs/i");
	setupArrBranch<UInt_t>("tdcID", b_tdcID, "[nTDCs]/i", m_nTDCs);
	setupArrBranch<UInt_t>("eventID", b_eventID, "[nTDCs]/i", m_nTDCs);
	setupArrBranch<UInt_t>("tdcTime", b_bunchID, "[nTDCs]/i", m_nTDCs);
	setupArrBranch<UInt_t>("rocTime", b_rocTime, "[nTDCs]/i", m_nTDCs);
	setupArrBranch<UInt_t>("nHeaders", b_nHeaders, "[nTDCs]/i", m_nTDCs);
	setupArrBranch<UInt_t>("nLeadingEdges", b_nLeadingEdges, "[nTDCs]/i", m_nTDCs);
	setupArrBranch<UInt_t>("nTrailingEdges", b_nTrailingEdges, "[nTDCs]/i", m_nTDCs);
	setupArrBranch<UInt_t>("nTrailers", b_nTrailers, "[nTDCs]/i", m_nTDCs);
	setupArrBranch<UInt_t>("wordCount", b_wordCount, "[nTDCs]/i", m_nTDCs);

	m_tree->Branch("nEdgesTotal", &b_nEdges, "num_edges_total/i");

	m_tree->Branch("nHits", &b_nHits, "nHits/i");
	setupArrBranch<UInt_t>("channelID", b_channelID, "[nHits]/i", s_hitsMax);
	setupArrBranch<UInt_t>("leadingTime", b_leadingTime, "[nHits]/i", s_hitsMax);
	setupArrBranch<UInt_t>("trailingTime", b_trailingTime, "[nHits]/i", s_hitsMax);
	setupArrBranch<UInt_t>("leadingTimeTDCBin", b_leadingTimeFine, "[nHits]/i", s_hitsMax);
	setupArrBranch<UInt_t>("trailingTimeTDCBin", b_trailingTimeFine, "[nHits]/i", s_hitsMax);
	setupArrBranch<Int_t>("width", b_width, "[nHits]/I", s_hitsMax);
}
