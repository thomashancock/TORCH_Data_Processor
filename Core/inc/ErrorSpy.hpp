#ifndef ERRORSPY_H
#define ERRORSPY_H

// STD
#include <map>

// LOCAL
#include "ErrorCounter.hpp"

class ErrorSpy {
public:
	ErrorSpy();

	void logError(
		const std::string message,
		const unsigned int readoutBoardID,
		const unsigned int tdcID
	);

	void print() const;

private:
	std::map<std::string, ErrorCounter> m_errorMap;
};

#endif /* ERRORSPY_H */
