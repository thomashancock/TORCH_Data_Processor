#ifndef ROOTMANAGER_H
#define ROOTMANAGER_H

// STD
#include <mutex>

// ROOT
#include "TFile.h"
#include "TTree.h"

template <class T>
class RootManager {
public:
	RootManager();

	virtual void add(
		T
	) = 0;

	inline void writeTree();

private:
	//! Sets up the TTrees
	// virtual void setUpTrees() = 0;

protected:
	mutable std::mutex m_mut; //!< Mutex for thread safety

private:
	TFile* m_outFile = nullptr;
	TTree* m_ttree = nullptr;
};

// Include function implementations
#include "RootManager.inl"

#endif /* ROOTMANAGER_H */
