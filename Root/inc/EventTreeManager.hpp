#ifndef EVENTTREEMANAGER_H
#define EVENTTREEMANAGER_H

// LOCAL
#include "RootManager.hpp"
#include "Event.hpp"

class EventTreeManager : public RootManager<Event> {
public:
	EventTreeManager(
		const std::string outfileName,
		const unsigned int nTDCs
	);

	void add(
		std::unique_ptr<Event> event
	) final;

private:
	void setUpBranches();

	void resetTreeVariables();

private:
	const unsigned int m_nTDCs;
	constexpr static unsigned int s_hitsMax = 500;

	// Set branch variables
	Int_t b_nTDCs;             //!< Branch Variable: The number of TDCs
	Int_t* b_tdcID;            //!< Branch Variable [nTDCs]: TCD ID
	Int_t* b_eventID;          //!< Branch Variable [nTDCs]: Event ID for each TDC
	Int_t* b_bunchID;          //!< Branch Variable [nTDCs]: Bunch ID for each TDC
	Int_t* b_rocTime;          //!< Branch Variable [nTDCs]: ROC value for each TDC
	Int_t* b_nHeaders;         //!< Branch Variable [nTDCs]: Number for headers for each TDC
	Int_t* b_nLeadingEdges;    //!< Branch Variable [nTDCs]: Number for leading edges for each TDC
	Int_t* b_nTrailingEdges;   //!< Branch Variable [nTDCs]: Number for trailing edges for each TDC
	Int_t* b_nTrailers;        //!< Branch Variable [nTDCs]: Number for trailers for each TDC
	Int_t* b_wordCount;        //!< Branch Variable [nTDCs]: Word count for each TDC
	Int_t b_nEdges;            //!< Branch Variable: Total number of  edges
	Int_t b_nHits;             //!< Branch Variable: Total number of hits
	Int_t* b_channelID;        //!< Branch Variable [nHits]: Channel IDs for each hit
	Int_t* b_leadingTime;      //!< Branch Variable [nHits]: Leading time for each hit
	Int_t* b_trailingTime;     //!< Branch Variable [nHits]: Trailing time for each hit
	Int_t* b_leadingTimeFine;  //!< Branch Variable [nHits]: Fine leading time for each hit
	Int_t* b_trailingTimeFine; //!< Branch Variable [nHits]: Fine trailing time for each hit
	Int_t* b_width;            //!< Branch Variable [nHits]: Width of signal

private:

};

#endif /* EVENTTREEMANAGER_H */
