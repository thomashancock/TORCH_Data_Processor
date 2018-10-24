
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <unsigned int N>
template <typename... Args>
ErrorCounter<N>::ErrorCounter(
	Args... args
) :
	m_labels {{ std::forward<Args>(args)... }}
{ }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <unsigned int N>
template <typename... Args>
auto ErrorCounter<N>::makeKey(
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
void ErrorCounter<N>::addCount(
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
