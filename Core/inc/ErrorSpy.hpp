#ifndef ERRORSPY_H
#define ERRORSPY_H

// STD
#include <unordered_map>
#include <string>
#include <mutex>

// LOCAL
#include "ErrorCounter.hpp"

//! Keeps track of errors detected during program execution
/*!
		Tracks and prints errors detected during process execution.

		To Log an error, call logError with a summary message, readout board ID and tdc ID.
		The message will be tracked, and any additional occurances will be added to the count.
 */
class ErrorSpy {
public:
	//! Constructor
	ErrorSpy();

	// Delete copy and move constructors and assignment operators
	ErrorSpy(const ErrorSpy& other) = delete;
	ErrorSpy& operator=(ErrorSpy other) = delete;
	ErrorSpy(const ErrorSpy&& other) = delete;
	ErrorSpy& operator=(ErrorSpy&& other) = delete;

	//! Logs an error
	void logError(
		const std::string message, //!< Message summarising the error
		const unsigned int readoutBoardID, //!< Readout Board ID where the error occured
		const unsigned int tdcID //!< TDC id where the error occured
	);

	//! Prints a summary of logged error messages
	void print() const;

private:
	mutable std::mutex m_mut; //!< Mutex for thread safety

	std::unordered_map<std::string, ErrorCounter> m_errorMap; //!< Map to store error messages
};

#endif /* ERRORSPY_H */
