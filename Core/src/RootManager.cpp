#include "RootManager.hpp"

#include <algorithm>

typedef std::pair< std::vector< unsigned int >, std::vector< unsigned int > > pairVectors;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
RootManager::RootManager(
	std::string outputRootFile
) :
b_nTDCs(MAX_TDCS),
m_outputFile(nullptr),
m_tdcTree(nullptr),
m_triggerTree(nullptr)
// m_programVersion("Build_Version",GITVERSION),
// m_config(config)
{
	STD_LOG("RootManager(): Constructor Called");

	// Root objects do not like being stored in unique_ptrs due to the global root state
	// Best to store these in raw pointers and be careful with the memory
	m_outputFile = TFile::Open(outputRootFile.c_str(),"RECREATE");

	m_tdcTree = new TTree("tdc_event","TDC Tree");
	m_triggerTree = new TTree("trigger_event","Trigger Tree");

	setUpTrees();

	// m_programVersion.Write();

	// Write whether 2017 Channel Fix was applied to the output file
	// const std::string channelFix2017Str = (true == m_config->channelFix2017()) ? "True" : "False";
	// TNamed channelFix2017Check("ChannelFix2017=",channelFix2017Str.c_str());
	// channelFix2017Check.Write();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RootManager::~RootManager() {
	STD_LOG("RootManager(): Deconstructor Called");

	m_outputFile->Close();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RootManager::addEvent(
	Event* event
) {
	// ASSERT(nullptr != event);
	// ASSERT(nullptr != m_tdcTree);
	// ASSERT(nullptr != m_triggerTree);
  //
	// const unsigned int eventID = event->getEventID();
	// b_nHits = 0; // Reset number of hits
  //
	// ASSERT(nullptr != m_config);
	// b_nTDCs = (int) m_config->getTDCList()->size();
	// ASSERT(b_nTDCs <= MAX_TDCS);
	// for (int i = 0; i < b_nTDCs; i++) {
	// 	const unsigned int tdcID = m_config->getTDCList()->at(i);
	// 	std::shared_ptr<Packet> packet = event->getPacket(tdcID);
  //
	// 	b_tdcID[i] = tdcID;
	// 	b_eventID[i] = eventID;
	// 	if ((nullptr == packet)||(!packet->isConsistent())) {
	// 		b_bunchID[i] = 0;
	// 		b_rocTime[i] = 0;
	// 		b_nHeaders[i] = 1;
	// 		b_nLeadingEdges[i] = 0;
	// 		b_nTrailingEdges[i] = 0;
	// 		b_nTrailers[i] = 1;
	// 		b_wordCount[i] = 2;
	// 	} else {
	// 		b_bunchID[i] = packet->getBunchID();
	// 		b_bunchIDTrig = b_bunchID[i]; // Set Trigger Tree Bunch ID (need to check it's consistent in the Event)
	// 		b_rocTime[i] = packet->getROCValue();
	// 		b_rocValueTrig = b_rocTime[i]; // Set Trigger Tree ROC time (need to check it's consistent in the Event)
	// 		b_nHeaders[i] = 1;
	// 		b_nLeadingEdges[i] = packet->getNLeadingEdges();
	// 		b_nTrailingEdges[i] = packet->getNTrailingEdges();
	// 		b_nTrailers[i] = 1;
	// 		b_wordCount[i] = packet->getWordCount();
  //
	// 		// Write edge information to file
	// 		std::map< unsigned int, pairVectors > edgeMap;
  //
	// 		auto addEdgesToMap = [&] (
	// 			const bool leading
	// 		) {
	// 			ASSERT(packet != nullptr);
	// 			const unsigned int nEdges = (true == leading) ?
	// 				packet->getNLeadingEdges() :
	// 				packet->getNTrailingEdges() ;
	// 			for (unsigned int iEdge = 0; iEdge < nEdges; iEdge++) {
	// 				const unsigned int channelID = packet->getChannelID(leading,iEdge);
	// 				const unsigned int timestamp = packet->getTimestamp(leading,iEdge);
  //
	// 				auto itr = edgeMap.find(channelID);
	// 				if (itr == edgeMap.end()) {
	// 					pairVectors newPair;
	// 					if (true == leading) {
	// 						newPair.first.push_back(timestamp);
	// 					} else {
	// 						newPair.second.push_back(timestamp);
	// 					}
	// 					if (!edgeMap.insert(
	// 						std::pair< unsigned int, pairVectors > (channelID,newPair)
	// 						).second) {
	// 						STD_ERR("Unable to insert new vector pair");
	// 					}
	// 				} else {
	// 					if (true == leading) {
	// 						itr->second.first.push_back(timestamp);
	// 					} else {
	// 						itr->second.second.push_back(timestamp);
	// 					}
	// 				}
	// 			}
	// 		};
  //
	// 		addEdgesToMap(true);
	// 		addEdgesToMap(false);
  //
	// 		for (auto& entry : edgeMap) {
	// 			const unsigned int channelID = entry.first;
  //
	// 			// lvalue = entry.pairVector.vectorInPair
	// 			std::vector<unsigned int>& leadingTimes = entry.second.first;
	// 			std::vector<unsigned int>& trailingTimes = entry.second.second;
  //
	// 			// Sort Edges
	// 			std::sort(leadingTimes.begin(),leadingTimes.end());
	// 			std::sort(trailingTimes.begin(),trailingTimes.end());
  //
	// 			// Get size of smallest vector
	// 			const unsigned int nPairs = (leadingTimes.size() > trailingTimes.size()) ? trailingTimes.size() : leadingTimes.size();
  //
	// 			// Write times until one vector is exhausted
	// 			for (unsigned int iPair = 0; iPair < nPairs; iPair++) {
	// 				b_channelID[b_nHits] = channelID;
	// 				b_leadingTime[b_nHits] = leadingTimes.at(iPair);
	// 				b_trailingTime[b_nHits] = trailingTimes.at(iPair);
	// 				b_leadingTimeFine[b_nHits] = leadingTimes.at(iPair)%256;
	// 				b_trailingTimeFine[b_nHits] = trailingTimes.at(iPair)%256;
	// 				b_width[b_nHits] = trailingTimes.at(iPair) - leadingTimes.at(iPair);
	// 				b_nHits++;
	// 			}
	// 		}
	// 	}
  //
	// 	b_nSignals += b_nLeadingEdges[i];
	// 	b_nSignals += b_nTrailingEdges[i];
	// }
  //
	// // Fill Trees
	// m_tdcTree->Fill();
	// m_triggerTree->Fill();
  //
	// resetTreeVariables();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RootManager::writeTrees() {
	ASSERT(nullptr != m_tdcTree);
	ASSERT(nullptr != m_triggerTree);

	m_tdcTree->Write();
	m_triggerTree->Write();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// void RootManager::writeMonitoringInformation(
// 	std::shared_ptr<SlotBuffer> slotBuffer
// ) {
// 	ASSERT(nullptr != slotBuffer);
// 	slotBuffer->writeMonitoring(m_outputFile);
// }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void RootManager::setUpTrees() {
	ASSERT(nullptr != m_tdcTree);
	ASSERT(nullptr != m_triggerTree);

	m_tdcTree->Branch("num_tdcs", &b_nTDCs, "num_tdcs/I");
	m_tdcTree->Branch("tdc_id", &b_tdcID, "tdc_id[num_tdcs]/I");
	m_tdcTree->Branch("event_id", b_eventID, "event_id[num_tdcs]/I");
	m_tdcTree->Branch("tdc_time", b_bunchID, "tdc_time[num_tdcs]/I");
	m_tdcTree->Branch("roc_time", b_rocTime, "roc_time[num_tdcs]/I");
	m_tdcTree->Branch("num_headers", b_nHeaders, "num_headers[num_tdcs]/I");
	m_tdcTree->Branch("num_leading_edges", b_nLeadingEdges, "num_leading_edges[num_tdcs]/I");
	m_tdcTree->Branch("num_trailing_edges", b_nTrailingEdges, "num_trailing_edges[num_tdcs]/I");
	m_tdcTree->Branch("num_trailers", b_nTrailers, "num_trailers[num_tdcs]/I");
	m_tdcTree->Branch("word_count", b_wordCount, "word_count[num_tdcs]/I");
	m_tdcTree->Branch("num_edges_total", &b_nSignals, "num_edges_total/I");
	m_tdcTree->Branch("num_hits", &b_nHits, "num_hits/I");
	m_tdcTree->Branch("channel_id", b_channelID, "channel_id[num_hits]/I");
	m_tdcTree->Branch("leading_time", b_leadingTime, "leading_time[num_hits]/I");
	m_tdcTree->Branch("trailing_time", b_trailingTime, "trailing_time[num_hits]/I");
	m_tdcTree->Branch("leading_time_tdc_bin", b_leadingTimeFine, "leading_time_tdc_bin[num_hits]/I");
	m_tdcTree->Branch("trailing_time_tdc_bin", b_trailingTimeFine, "trailing_time_tdc_bin[num_hits]/I");
	m_tdcTree->Branch("width", b_width, "width[num_hits]/I");

	m_triggerTree->Branch("bunch_id", &b_bunchIDTrig);
	m_triggerTree->Branch("roc_value", &b_rocValueTrig);

	resetTreeVariables();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RootManager::resetTreeVariables() {
	b_nTDCs = 8;
	b_nSignals = 0;
	b_nHits = 0;
}
