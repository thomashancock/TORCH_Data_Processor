#ifndef ERRORCOUNTER_H
#define ERRORCOUNTER_H

// STD
#include <map>
#include <array>
#include <iostream>

// LOCAL
#include "Debug.hpp"

//! Used by ErrorSpy to track which readout board/tdc combinations errors have occured on
template <unsigned int N>
class ErrorCounter {
public:
	//! Constructor
	template <typename... Args>
	ErrorCounter(
		Args... args
	);

	//! Incrememnts the count for the readout board/tdc combination
	template <typename... Args>
	inline void addCount(
		Args... args
	);

	//! Prints the counts stored
	void print() const;

private:
	//! Returns a map key for the given readout board/tdc combination
	/*!
		Returns auto to facilitate easy modifiaction of map key type
	 */
	template <typename... Args>
	inline std::array<unsigned int, N> makeKey(
		Args... args
	) const;

private:
	const std::array<std::string, N> m_labels;
	std::map< std::array<unsigned int, N> , unsigned int > m_counts; //!< Map to store counts in
};

// Class specialisation for N = 0
template <>
class ErrorCounter<0> {
public:
	//! Constructor
	ErrorCounter() { };

	//! Incrememnts the count for the readout board/tdc combination
	inline void addCount() { m_count++; };

	//! Prints the counts stored
	void print() const {
		std::cout << " (x " << m_count << ")" << std::endl;
	};

private:

private:
	unsigned int m_count = 0;
};


#endif /* ERRORCOUNTER_H */

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Inlines:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <unsigned int N>
template <typename... Args>
ErrorCounter<N>::ErrorCounter(
	Args... args
) :
	m_labels {{ std::forward<Args>(args)... }}
{
	ASSERT(N == m_labels.size());
};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <unsigned int N>
template <typename... Args>
inline std::array<unsigned int, N> ErrorCounter<N>::makeKey(
	Args... args
) const {
	std::array<unsigned int, N> tmp {{ std::forward<Args>(args)... }};
	return tmp;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <unsigned int N>
template <typename... Args>
inline void ErrorCounter<N>::addCount(
	Args... args
) {
	const auto key = makeKey(std::forward<Args>(args)...);

	auto found = m_counts.find(key);
	if (m_counts.end() == found) {
		m_counts.insert(std::make_pair(key,1));
	} else {
		found->second++;
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <unsigned int N>
void ErrorCounter<N>::print() const {
	for (const auto& entry : m_counts) {
		std::cout << "\t";
		for (unsigned int i = 0; i < m_labels.size(); i++) {
			std::cout << m_labels[i] << ": " << entry.first[i];
			if (i < m_labels.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << " (x " << entry.second << ")" << std::endl;
	}
}
