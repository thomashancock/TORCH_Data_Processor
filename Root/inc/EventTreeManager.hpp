#ifndef EVENTTREEMANAGER_H
#define EVENTTREEMANAGER_H

// LOCAL
#include "RootManager.hpp"
#include "Event.hpp"

//! Class which manages storage of Event data in root file output
class EventTreeManager : public RootManager<Event> {
public:
	//! Constructor
	EventTreeManager(
		const std::string outfileName, //!< The desired output file name
		const unsigned int nTDCs //!< Number of TDCs present from config
	);

	//! Adds an event to the tree, then destroys the event
	/*!
		Overload of RootManager add.
	 */
	void add(
		std::unique_ptr<Event> event
	) final;

private:
	//! Sets up the branches of the tree
	void setUpBranches();

	//! Resets the coutner variables in the tree to their default values
	void resetTreeVariables();

private:
	const unsigned int m_nTDCs; //!< The maximum number of TDCs
	constexpr static unsigned int s_hitsMax = 500; //!< Array size for hit branches

	// Set branch variables
	Int_t b_nTDCs;             //!< Branch Variable: The number of TDCs
	Int_t* b_tdcID = nullptr;            //!< Branch Variable [nTDCs]: TCD ID
	Int_t* b_eventID = nullptr;          //!< Branch Variable [nTDCs]: Event ID for each TDC
	Int_t* b_bunchID = nullptr;          //!< Branch Variable [nTDCs]: Bunch ID for each TDC
	Int_t* b_rocTime = nullptr;          //!< Branch Variable [nTDCs]: ROC value for each TDC
	Int_t* b_nHeaders = nullptr;         //!< Branch Variable [nTDCs]: Number for headers for each TDC
	Int_t* b_nLeadingEdges = nullptr;    //!< Branch Variable [nTDCs]: Number for leading edges for each TDC
	Int_t* b_nTrailingEdges = nullptr;   //!< Branch Variable [nTDCs]: Number for trailing edges for each TDC
	Int_t* b_nTrailers = nullptr;        //!< Branch Variable [nTDCs]: Number for trailers for each TDC
	Int_t* b_wordCount = nullptr;        //!< Branch Variable [nTDCs]: Word count for each TDC
	Int_t b_nEdges;            //!< Branch Variable: Total number of  edges
	Int_t b_nHits;             //!< Branch Variable: Total number of hits
	Int_t* b_channelID = nullptr;        //!< Branch Variable [nHits]: Channel IDs for each hit
	Int_t* b_leadingTime = nullptr;      //!< Branch Variable [nHits]: Leading time for each hit
	Int_t* b_trailingTime = nullptr;     //!< Branch Variable [nHits]: Trailing time for each hit
	Int_t* b_leadingTimeFine = nullptr;  //!< Branch Variable [nHits]: Fine leading time for each hit
	Int_t* b_trailingTimeFine = nullptr; //!< Branch Variable [nHits]: Fine trailing time for each hit
	Int_t* b_width = nullptr;            //!< Branch Variable [nHits]: Width of signal
};

#endif /* EVENTTREEMANAGER_H */
