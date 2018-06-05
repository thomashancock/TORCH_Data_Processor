// STD
#include <set>
#include <string>
#include <iostream>
#include <cassert>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TPad.h"

// Useful alias
using uint = unsigned int;

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

// Makes a histogram of channel IDs
void MakeChannelHisto(
	const std::string inputFile,
	const bool logY = false
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

	// ------------------
	// Create histrograms
	// ------------------
	// Stored as pointers so they are accessible from the root interactive
	//   session after the macro finishes execution
	TH1D* timeRefHits = new TH1D("timeRefHits","timeRefHits",512,0,512);
	TH1D* otherHits = new TH1D("otherHits","otherHits",512,0,512);

	// ---------------
	// Fill histograms
	// ---------------
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

	// -----------------
	// Format histograms
	// -----------------
	auto formatPlot = [] (
		TH1D* plot,
		const int lineColor
	) -> void {
		plot->SetLineColor(lineColor);
		plot->GetXaxis()->SetTitle("Channel ID");
		plot->GetYaxis()->SetTitle("Hit Count");
	};

	formatPlot(timeRefHits, kRed);
	formatPlot(otherHits, kBlue);

	// ------------------
	// Make and save plot
	// ------------------
	TCanvas canvas("canvas","canvas",10,10,1780,1000);

	if (true == logY) {
		gPad->SetLogy();
		// Ensure axis range extends to ~0
		timeRefHits->SetAxisRange(0.7,timeRefHits->GetMaximum()*1.3,"Y");
		otherHits->SetAxisRange(0.7,otherHits->GetMaximum()*1.3,"Y");
	}

	// If time reference plot has no entries, only draw other histogram
	if (timeRefHits->GetEntries() < 1) {
		otherHits->Draw();
	// Otherwise draw plot with greatest maximum first
	} else if (timeRefHits->GetMaximum() > otherHits->GetMaximum()) {
		timeRefHits->Draw();
		otherHits->Draw("same");
	} else {
		otherHits->Draw();
		timeRefHits->Draw("same");
	}

	// Save Canvas
	canvas.SaveAs("ChannelIDHistogram.pdf");

	return;
}
