// STD
#include <iostream>
#include <utility>

// ROOT
#include "TFile.h"
#include "TTree.h"

// Useful Aliases
using uint = unsigned int;
using coor = std::pair<uint, uint>;

//! Returns the xy coordinate for a given channel
coor getCoordinate(
	const uint channelID
) {
	auto row = 64 - (channelID%64);
	auto col = floor(channelID / 64.0) + 1;

	return std::make_pair(col,row);
}

//! Makes a hitmap from the passed file
void MakeHitmap(
	const std::string inputFile
) {
	// Open file
	auto* inFile = TFile::Open(inputFile.c_str(),"READ");
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

	// Setup hitmap
	TH2D hitmap("Hitmap","Hitmap",8,0.5,8.5,64,0.5,64.5);

	// Read hits from tree
	const auto nEntries = inTree->GetEntries();
	std::cout << nEntries << std::endl;
	for (auto i = 0; i < nEntries; i++) {
		inTree->GetEntry(i);
		for (auto iHit = 0; iHit < nHits; iHit++) {
			const auto coor = getCoordinate(channelIDs[iHit]);
			hitmap.Fill(coor.first,coor.second);
		}
	}

	// Save Hitmap
	TCanvas canvas("canvas","canvas");
	hitmap.Draw("colz");
	canvas.SaveAs("Hitmap.pdf");
}
