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

private:
	template<class T>
	void setupArrBranch(
		const std::string branchName, //!< Desired Branch Name
		T*& arrayPtr,                 //!< Reference to pointer to be set
		const std::string leafType,   //!< Leaf type (e.g. "/I" )
		const unsigned int arraySize  //!< Size of array to be allocated
	);
};

template<class T>
void EventTreeManager::setupArrBranch(
	const std::string branchName, //!< Desired Branch Name
	T*& arrayPtr,                 //!< Reference to pointer to be set
	const std::string leafType,   //!< Leaf type (e.g. "/I" )
	const unsigned int arraySize  //!< Size of array to be allocated
) {
	const auto leafList = branchName + leafType;
	arrayPtr = new T[arraySize] { 0 };
	m_tree->Branch(branchName.c_str(),arrayPtr,leafList.c_str());
}

#endif /* EVENTTREEMANAGER_H */
