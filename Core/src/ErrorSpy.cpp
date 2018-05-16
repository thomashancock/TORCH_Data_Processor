#include "ErrorSpy.hpp"

// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ErrorSpy::ErrorSpy() {
	STD_LOG("ErrorSpy Constructor Called");

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
		ErrorCounter counter;
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
void ErrorSpy::print() const {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	std::cout << "=== Errors Summary ===" << std::endl;
	for (const auto& entry : m_errorMap) {
		std::cout << "Error: " << entry.first << std::endl;
		entry.second.print();
	}
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
