// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <class T>
RootManager<T>::RootManager(
	const std::string outFileName,
	const std::string treeName
) :
	m_programVersion("Build_Version",GITTAG)
{
	STD_LOG("RootManager Constructor Called");

	m_outFile = TFile::Open(outFileName.c_str(),"RECREATE");
	m_tree = new TTree(treeName.c_str(),treeName.c_str());

	// Write program version
	m_programVersion.Write();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <class T>
RootManager<T>::~RootManager() {
	ASSERT(nullptr != m_outFile);
	m_outFile->Close();
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
