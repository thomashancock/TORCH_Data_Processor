#ifndef ROOTMANAGER_H
#define ROOTMANAGER_H

// STD
#include <string>
#include <memory>
#include <mutex>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TNamed.h"

template <class T>
class RootManager {
public:
	RootManager(
		const std::string outFileName,
		const std::string treeName
	);

	//! Explicit deconstructor. Implicitly deletes move and copy constructors.
	~RootManager();

	virtual void add(
		std::unique_ptr<T> //!< Argument required for function
	) = 0;

	inline void writeTree();

protected:
	template<class U>
	void setupArrBranch(
		const std::string branchName, //!< Desired Branch Name
		U*& arrayPtr,                 //!< Reference to pointer to be set
		const std::string leafType,   //!< Leaf type (e.g. "/I" )
		const unsigned int arraySize  //!< Size of array to be allocated
	);

protected:
	mutable std::mutex m_mut; //!< Mutex for thread safety

	TTree* m_tree = nullptr; //!< Output TTree

private:
	TFile* m_outFile = nullptr; //!< Output file pointer

	const TNamed m_programVersion; //!< TNamed which writes build version to output tree
};

// Include function implementations
#include "RootManager.inl"

#endif /* ROOTMANAGER_H */
