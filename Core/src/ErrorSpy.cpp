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
	auto found = m_errorMap.find(message);
	if (m_errorMap.end() == found) {
		// If message is not in map, add it
		ErrorCounter<2> counter("BoardID", "TDC");
		counter.addCount(readoutBoardID, tdcID);
		m_errorMap.insert(std::make_pair(message,std::move(counter)));
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
	auto found = m_errorMapReadouts.find(message);
	if (m_errorMapReadouts.end() == found) {
		// If message is not in map, add it
		ErrorCounter<1> counter("BoardID");
		counter.addCount(readoutBoardID);
		m_errorMapReadouts.insert(std::make_pair(message,std::move(counter)));
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
	auto found = m_errorMapGeneral.find(message);
	if (m_errorMapGeneral.end() == found) {
		// If message is not in map, add it
		ErrorCounter<0> counter;
		counter.addCount();
		m_errorMapGeneral.insert(std::make_pair(message,std::move(counter)));
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
	for (const auto& entry : m_errorMap) {
		std::cout << "Error: " << entry.first << std::endl;
		entry.second.print();
	}
	for (const auto& entry : m_errorMapReadouts) {
		std::cout << "Error: " << entry.first << std::endl;
		entry.second.print();
	}
	for (const auto& entry : m_errorMapGeneral) {
		std::cout << "Error: " << entry.first << std::endl;
		entry.second.print();
	}
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ErrorSpy::ErrorSpy() {
	STD_LOG("ErrorSpy Constructor Called");
}
