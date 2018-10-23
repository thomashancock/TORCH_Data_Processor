#include "ErrorSpy.hpp"

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
	const ReadoutIdentifier& readoutID
) {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Check if message in already in map
	auto found = m_error3D.find(message);
	if (m_error3D.end() == found) {
		// If message is not in map, add it
		ErrorCounter<3> counter("ChainID", "DeviceID", "TDC");
		counter.addCount(readoutID.getChainID(), readoutID.getDeviceID(), readoutID.getTDCID());
		m_error3D.insert(std::make_pair(message,std::move(counter)));
	} else {
		// Increment counter for given message
		found->second.addCount(readoutID.getChainID(), readoutID.getDeviceID(), readoutID.getTDCID());
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorSpy::logError(
	const std::string message,
	const BoardIdentifier& boardID
) {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Check if message in already in map
	auto found = m_error2D.find(message);
	if (m_error2D.end() == found) {
		// If message is not in map, add it
		ErrorCounter<2> counter("BoardID");
		counter.addCount(boardID.getChainID(), boardID.getDeviceID());
		m_error2D.insert(std::make_pair(message,std::move(counter)));
	} else {
		// Increment counter for given message
		found->second.addCount(boardID.getChainID(), boardID.getDeviceID());
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
	if (m_error3D.empty() && m_error2D.empty() && m_error0D.empty()) {
		std::cout << "  No Errors Detected" << std::endl;
	} else {
		printMap(m_error3D);
		printMap(m_error2D);
		printMap(m_error0D);
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
