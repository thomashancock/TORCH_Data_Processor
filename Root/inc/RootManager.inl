// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <class T>
RootManager<T>::RootManager(
	std::shared_ptr<const Config> config,
	const std::string outFileName,
	const std::string treeName
) :
	m_programVersion("Build_Version",GITTAG),
	m_channelMapping("Channel_Mapping",config->getChannelMappingKey().c_str()),
	m_edgeModifier("Edge_Modifier",config->getEdgeModifierKey().c_str())
{
	STD_LOG("RootManager Constructor Called");

	m_outFile = TFile::Open(outFileName.c_str(),"RECREATE");
	m_tree = new TTree(treeName.c_str(),treeName.c_str());

	// Write program version
	m_programVersion.Write();
	m_channelMapping.Write();
	m_edgeModifier.Write();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <class T>
RootManager<T>::~RootManager() {
	ASSERT(nullptr != m_outFile);
	// Lock Mutex
	// Added for safety. Deconstructor should not be called while m_outFile is being modified
	m_mut.lock();

	// Close output file
	m_outFile->Close();

	// Explicitly unlock mutex
	m_mut.unlock();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <class T>
inline void RootManager<T>::writeTree() {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	//  Write Tree
	ASSERT(nullptr != m_tree);
	m_tree->Write();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<class T>
template<class U>
void RootManager<T>::setupArrBranch(
	const std::string branchName, //!< Desired Branch Name
	U*& arrayPtr,                 //!< Reference to pointer to be set
	const std::string leafType,   //!< Leaf type (e.g. "/I" )
	const unsigned int arraySize  //!< Size of array to be allocated
) {
	const auto leafList = branchName + leafType;
	arrayPtr = new U[arraySize] { 0 };
	m_tree->Branch(branchName.c_str(),arrayPtr,leafList.c_str());
}
