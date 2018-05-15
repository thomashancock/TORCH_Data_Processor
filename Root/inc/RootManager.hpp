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

//! Class which manages root output
/*!
	@tparam The class which will be used to provide information for the root tree
*/
template <class T>
class RootManager {
public:
	//! Constructor
	RootManager(
		const std::string outFileName, //!< The output root files name
		const std::string treeName //!< The desired ttree name
	);

	//! Explicit deconstructor
	/*!
		Closes output file on deconstruction.
		Implicitly deletes move and copy constructors.
	*/
	~RootManager();

	//! Pure virtual method for adding information to the tree
	virtual void add(
		std::unique_ptr<T> //!< Argument required for function
	) = 0;

	//! Writes the tree to the output file
	inline void writeTree();

protected:
	//! Helpful function to allocate memory for an array-style branch
	/*!
		@tparam The data type of the branch
	*/
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
