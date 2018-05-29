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

	// Get TTree
	enum treetype {
		packet,
		event
	};

	treetype treeType;

	if (nullptr == (inTree = (TTree*) inFile->Get("event_tree"))) {
		if (nullptr == (inTree = (TTree*) inFile->Get("packet_tree"))) {
			std::cout << "Error: Cannot access TTree" << std::endl;
			return;
		} else {
			treeType == treetype::event;
		}
	} else {
		treeType = treetype::packet;
	}
	assert(nullptr != inTree);
	std::cout << inTree << std::endl; // Cannot access branch is pointer is not printed???

	// Set branch addresses
	const std::string hitsBranchName = (treetype::packet == treeType) ? "nLeadingEdges" : "nHits";
	const std::string channelBranchName = (treetype::packet == treeType) ? "leadingChannelID" : "channelID";
	uint nHits;
	uint* channelIDs = new uint[500];
	inTree->GetBranch(hitsBranchName.c_str())->SetAddress(&nHits);
	inTree->GetBranch(channelBranchName.c_str())->SetAddress(channelIDs);

	// Create histrograms
	TH1D timeRefHits("timeRefHits","timeRefHits",512,0,512);
	TH1D otherHits("otherHits","otherHits",512,0,512);

	// Fill histograms
	const auto nEntries = inTree->GetEntries();
	for (auto i = 0; i < nEntries; i++) {
		inTree->GetEntry(i);
		for (uint j = 0; j < nHits; j++) {
			auto& channelID = channelIDs[j];
			if (isTimeReference(channelID)) {
				timeRefHits.Fill(channelID);
			} else {
				otherHits.Fill(channelID);
			}
		}
	}

	// Make and save plot
	TCanvas canvas("canvas","canvas",10,10,1780,1000);
	timeRefHits.SetLineColor(kRed);
	timeRefHits.Draw();
	otherHits.SetLineColor(kBlue);
	otherHits.Draw("same");
	canvas.SaveAs("ChannelIDHistogram.pdf");

	return;
}
