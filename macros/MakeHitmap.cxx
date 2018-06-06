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

//! Returns if a channel is a time reference channel
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

//! Loads either the event or packet tree
bool loadTree(
	TFile* inFile,
	TTree*& inTree,
	bool& isEventTree
) {
	inTree = nullptr;
	isEventTree = false;
	inTree = (TTree*) inFile->Get("event_tree");
	if (nullptr == inTree) {
		inTree = (TTree*) inFile->Get("packet_tree");
		if (nullptr == inTree) {
			return false;
		}
	} else {
		isEventTree = true;
	}
	return true;
}

//! Makes a hitmap from the passed file
void MakeHitmap(
	const std::string inputFile,
	const bool logZ = false,
	const bool ignoreTimeReference = false
) {
	// ---------
	// Open file
	// ---------
	auto* inFile = TFile::Open(inputFile.c_str(),"READ");

	// ----------------
	// Load Input TTree
	// ----------------
	TTree* inTree = nullptr;
	bool isEventTree = false;
	if (!loadTree(inFile, inTree, isEventTree)) {
		std::cout << "Error: Cannot access TTree" << std::endl;
		return;
	}
	assert(nullptr != inTree);
	std::cout << ((true == isEventTree) ? "Event" : "Packet") << " tree detected" << std::endl;

	// --------------------
	// Set Branch Addresses
	// --------------------
	const std::string hitsBranchName = (false == isEventTree) ? "nLeadingEdges" : "nHits";
	const std::string channelBranchName = (false == isEventTree) ? "leadingChannelID" : "channelID";
	uint nHits;
	uint* channelIDs = new uint[500];
	inTree->GetBranch(hitsBranchName.c_str())->SetAddress(&nHits);
	inTree->GetBranch(channelBranchName.c_str())->SetAddress(channelIDs);

	// ------------
	// Setup Hitmap
	// ------------
	TH2D hitmap("Hitmap","Hitmap",8,0.5,8.5,64,0.5,64.5);

	// -------------------
	// Read Hits from Tree
	// -------------------
	const auto nEntries = inTree->GetEntries();
	std::cout << nEntries << std::endl;
	for (auto i = 0; i < nEntries; i++) {
		inTree->GetEntry(i);
		for (auto iHit = 0; iHit < nHits; iHit++) {
			if ((false == ignoreTimeReference)||(!isTimeReference(channelIDs[iHit]))) {
				const auto coor = getCoordinate(channelIDs[iHit]);
				hitmap.Fill(coor.first,coor.second);
			}
		}
	}

	// -----------
	// Save Hitmap
	// -----------
	TCanvas canvas("canvas","canvas");
	if (true == logZ) {
		gPad->SetLogz();
	}
	hitmap.Draw("colz");
	canvas.SaveAs("Hitmap.pdf");
}
