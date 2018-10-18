#include "ErrorSpy.hpp"

// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Statics:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ErrorSpy& ErrorSpy::getInstance() {
	static ErrorSpy instance;
	return instance;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ErrorSpy::~ErrorSpy() {
	STD_LOG("ErrorSpy Deconstructor Called");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorSpy::logError(
	const std::string message,
	const unsigned int readoutBoardID,
	const unsigned int tdcID
) {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Check if message in already in map
	auto found = m_error2D.find(message);
	if (m_error2D.end() == found) {
		// If message is not in map, add it
		ErrorCounter<2> counter("BoardID", "TDC");
		counter.addCount(readoutBoardID, tdcID);
		m_error2D.insert(std::make_pair(message,std::move(counter)));
	} else {
		// Increment counter for given message
		found->second.addCount(readoutBoardID, tdcID);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorSpy::logError(
	const std::string message,
	const unsigned int readoutBoardID
) {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Check if message in already in map
	auto found = m_error1D.find(message);
	if (m_error1D.end() == found) {
		// If message is not in map, add it
		ErrorCounter<1> counter("BoardID");
		counter.addCount(readoutBoardID);
		m_error1D.insert(std::make_pair(message,std::move(counter)));
	} else {
		// Increment counter for given message
		found->second.addCount(readoutBoardID);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorSpy::logError(
	const std::string message
) {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Check if message in already in map
	auto found = m_error0D.find(message);
	if (m_error0D.end() == found) {
		// If message is not in map, add it
		ErrorCounter<0> counter;
		counter.addCount();
		m_error0D.insert(std::make_pair(message,std::move(counter)));
	} else {
		// Increment counter for given message
		found->second.addCount();
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorSpy::print() const {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	std::cout << "=== Errors Summary ===" << std::endl;
	printMap(m_error2D);
	printMap(m_error1D);
	printMap(m_error0D);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ErrorSpy::ErrorSpy() {
	STD_LOG("ErrorSpy Constructor Called");
}
