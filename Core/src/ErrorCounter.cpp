#include "ErrorCounter.hpp"

// STD
#include <iostream>

// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ErrorCounter::ErrorCounter() {

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErrorCounter::print() const {
	for (const auto& entry : m_counts) {
		std::cout
			<< "\tBoard: " << entry.first.first
			<< ", TDC: " << entry.first.second
			<< " (x " << entry.second << ")"
			<< std::endl;
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
