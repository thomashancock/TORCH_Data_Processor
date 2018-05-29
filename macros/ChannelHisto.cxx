#include "TFile.h"
#include "TTree.h"

#include <set>
#include <string>
#include <iostream>
#include <cassert>

using uint = unsigned int;

bool isTimeReference(
	const uint channelID
) {
	const static std::set<uint> channelIDs { 31, 63, 159, 191, 287, 319, 415, 447 };
	if (channelIDs.end() == channelIDs.find(channelID)) {
		return false;
	} else {
		return true;
	}
}

void ChannelHisto (
	const std::string inputFile
) {
	// Open file
	auto* inFile = TFile::Open(inputFile.c_str());
	TTree* inTree = nullptr;

	bool isEventTree = false;
	inTree = (TTree*) inFile->Get("event_tree");
	if (nullptr == inTree) {
		inTree = (TTree*) inFile->Get("packet_tree");
		if (nullptr == inTree) {
			std::cout << "Error: Cannot access TTree" << std::endl;
			return;
		}
	} else {
		isEventTree = true;
	}
	assert(nullptr != inTree);

	std::cout << ((true == isEventTree) ? "Event" : "Packet") << " tree detected" << std::endl;

	// Set branch addresses
	const std::string hitsBranchName = (false == isEventTree) ? "nLeadingEdges" : "nHits";
	const std::string channelBranchName = (false == isEventTree) ? "leadingChannelID" : "channelID";
	uint nHits;
	uint* channelIDs = new uint[500];
	inTree->GetBranch(hitsBranchName.c_str())->SetAddress(&nHits);
	inTree->GetBranch(channelBranchName.c_str())->SetAddress(channelIDs);

	// Create histrograms
	TH1D* timeRefHits = new TH1D("timeRefHits","timeRefHits",512,0,512);
	TH1D* otherHits = new TH1D("otherHits","otherHits",512,0,512);

	// Fill histograms
	const auto nEntries = inTree->GetEntries();
	for (auto i = 0; i < nEntries; i++) {
		inTree->GetEntry(i);
		for (uint j = 0; j < nHits; j++) {
			auto& channelID = channelIDs[j];
			if (isTimeReference(channelID)) {
				timeRefHits->Fill(channelID);
			} else {
				otherHits->Fill(channelID);
			}
		}
	}

	// Make and save plot
	timeRefHits->SetLineColor(kRed);
	otherHits->SetLineColor(kBlue);

	TCanvas canvas("canvas","canvas",10,10,1780,1000);
	if (timeRefHits->GetMaximum() > otherHits->GetMaximum()) {
		timeRefHits->Draw();
		otherHits->Draw("same");
	} else {
		otherHits->Draw();
		timeRefHits->Draw("same");
	}
	canvas.SaveAs("ChannelIDHistogram.pdf");

	return;
}
