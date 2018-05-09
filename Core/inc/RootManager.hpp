#ifndef ROOTMANAGER_H
#define ROOTMANAGER_H

#include <memory>

#include "TFile.h"
#include "TTree.h"
#include "TNamed.h"

#include "Debug.hpp"
#include "Event.hpp"
// #include "SlotBuffer.hpp"

#define MAX_TDCS (16)
#define MAX_HITS (500)

//! Class which manages the ROOT output of the program
/*!
	Handles setting up the TTrees and writing information to them.

	Ensure all the ROOT code is kept in one place.
*/
class RootManager {
public:
	//! Constructor
	RootManager(
		std::string outputRootFile
	);
	//! Deconstructor
	~RootManager();

	//! Adds the information from an Event to the TTrees
	void addEvent(
		Event* event //!< Pointer to the event to be added
	);

	//! Writes the stored TTrees
	void writeTrees();

	// void writeMonitoringInformation(
	// 	std::shared_ptr<SlotBuffer> slotBuffer
	// );

private:
	//! Sets up the TTrees
	void setUpTrees();

	//! Resets the variables corresponding to branches to initialised values
	void resetTreeVariables();

private:
	Int_t b_nTDCs;                      //!< Branch Variable: The number of TDCs
	Int_t b_tdcID[MAX_TDCS];            //!< Branch Variable [nTDCs]: TCD ID
	Int_t b_eventID[MAX_TDCS];          //!< Branch Variable [nTDCs]: Event ID for each TDC
	Int_t b_bunchID[MAX_TDCS];          //!< Branch Variable [nTDCs]: Bunch ID for each TDC
	Int_t b_rocTime[MAX_TDCS];          //!< Branch Variable [nTDCs]: ROC value for each TDC
	Int_t b_nHeaders[MAX_TDCS];         //!< Branch Variable [nTDCs]: Number for headers for each TDC
	Int_t b_nLeadingEdges[MAX_TDCS];    //!< Branch Variable [nTDCs]: Number for leading edges for each TDC
	Int_t b_nTrailingEdges[MAX_TDCS];   //!< Branch Variable [nTDCs]: Number for trailing edges for each TDC
	Int_t b_nTrailers[MAX_TDCS];        //!< Branch Variable [nTDCs]: Number for trailers for each TDC
	Int_t b_wordCount[MAX_TDCS];        //!< Branch Variable [nTDCs]: Word count for each TDC
	Int_t b_nSignals;                   //!< Branch Variable: Total number of leading and trailing edges
	Int_t b_nHits;                      //!< Branch Variable: Total number of hits
	Int_t b_channelID[MAX_HITS];        //!< Branch Variable [nHits]: Channel IDs for each hit
	Int_t b_leadingTime[MAX_HITS];      //!< Branch Variable [nHits]: Leading time for each hit
	Int_t b_trailingTime[MAX_HITS];     //!< Branch Variable [nHits]: Trailing time for each hit
	Int_t b_leadingTimeFine[MAX_HITS];  //!< Branch Variable [nHits]: Fine leading time for each hit
	Int_t b_trailingTimeFine[MAX_HITS]; //!< Branch Variable [nHits]: Fine trailing time for each hit
	Int_t b_width[MAX_HITS];            //!< Branch Variable [nHits]: Width of signal

	Int_t b_bunchIDTrig;  //!< Branch Variable: Bunch ID for the trigger
	Int_t b_rocValueTrig; //!< Branch Variable: ROC value for the trigger

private:
	TFile* m_outputFile; //!< Pointer to store the output file

	TTree* m_tdcTree;     //!< Pointer to store the TDC tree
	TTree* m_triggerTree; //!< Pointer to store the trigger tree

	// const TNamed m_programVersion; //!< TNamed which writes build version to output tree
};

#endif /* ROOTMANAGER_H */
