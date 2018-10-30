#ifndef EVENTTREEMANAGER_H
#define EVENTTREEMANAGER_H

// LOCAL
#include "RootManager.hpp"
#include "Event.hpp"
#include "Config.hpp"

//! Class which manages storage of Event data in root file output
class EventTreeManager : public RootManager<Event> {
public:
	//! Constructor
	EventTreeManager(
		std::shared_ptr<const Config> config, //!< Ptr to program configuration
		const std::string outfileName, //!< The desired output file name
		const unsigned int nReadouts //!< Number of TDCs present from config
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

private:
	const unsigned int m_nReadouts; //!< The maximum number of TDCs
	constexpr static unsigned int s_hitsMax = 600; //!< Array size for hit branches

	const std::set<unsigned int> m_edgeMatchingExclusions; //!< Set to store edge matching exclusions

	// Set branch variables
	Bool_t b_isComplete = false; //!< Branch Variable: Is event complete?
	Bool_t b_wasDumped = false; //!< Branch Variable: Was the event dumped?

	UInt_t b_nReadouts = 0;               //!< Branch Variable: The number of Readouts (= nTDC chips used)
	UInt_t* b_tdcID = nullptr;            //!< Branch Variable [nReadouts]: TCD ID
	UInt_t* b_eventID = nullptr;          //!< Branch Variable [nReadouts]: Event ID for each TDC
	UInt_t* b_bunchID = nullptr;          //!< Branch Variable [nReadouts]: Bunch ID for each TDC
	UInt_t* b_rocTime = nullptr;          //!< Branch Variable [nReadouts]: ROC value for each TDC
	UInt_t* b_nHeaders = nullptr;         //!< Branch Variable [nReadouts]: Number for headers for each TDC
	UInt_t* b_nLeadingEdges = nullptr;    //!< Branch Variable [nReadouts]: Number for leading edges for each TDC
	UInt_t* b_nTrailingEdges = nullptr;   //!< Branch Variable [nReadouts]: Number for trailing edges for each TDC
	UInt_t* b_nTrailers = nullptr;        //!< Branch Variable [nReadouts]: Number for trailers for each TDC
	UInt_t* b_wordCount = nullptr;        //!< Branch Variable [nReadouts]: Word count for each TDC

	UInt_t b_nEdges = 0;                  //!< Branch Variable: Total number of  edges
	UInt_t b_nHits = 0;                   //!< Branch Variable: Total number of hits
	UInt_t* b_channelID = nullptr;        //!< Branch Variable [nHits]: Channel IDs for each hit
	UInt_t* b_leadingTime = nullptr;      //!< Branch Variable [nHits]: Leading time for each hit
	UInt_t* b_trailingTime = nullptr;     //!< Branch Variable [nHits]: Trailing time for each hit
	UInt_t* b_leadingTimeFine = nullptr;  //!< Branch Variable [nHits]: Fine leading time for each hit
	UInt_t* b_trailingTimeFine = nullptr; //!< Branch Variable [nHits]: Fine trailing time for each hit
	Int_t* b_width = nullptr;             //!< Branch Variable [nHits]: Width of signal
};

#endif /* EVENTTREEMANAGER_H */
