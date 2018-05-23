#include "Packet.hpp"

// LOCAL
#include "BinaryDecoding.hpp"
#include "ErrorSpy.hpp"

// Useful alias
using uint = unsigned int;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Statics:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Packet::setChannelMapping(
	std::function<uint(uint, uint, uint)> mappingFunction
) {
	m_channelMapper = mappingFunction;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Packet::setPolarityModifier(
	std::function<void(uint&)> polarityFunction
) {
	m_polarityFixer = polarityFunction;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::function<uint(uint, uint, uint)> Packet::m_channelMapper = [] (
	uint ,
	uint ,
	uint channelID
) {
	return channelID;
};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::function<void(uint&)> Packet::m_polarityFixer = [] (uint&) { };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Packet::Packet(
	const unsigned int readoutBoardID,
	const unsigned int rocValue,
	const unsigned int headerWord
) :
	m_readoutBoardID(readoutBoardID),
	m_rocValue(rocValue)
{
	this->addHeader(headerWord);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool Packet::isComplete() const {
	const bool complete = m_headerAdded * m_trailerAdded;

	// Additional checks (e.g. wordcount) here
	// bool wordCountCheck = false;
	// if (2 + m_leadingEdges.size() + m_trailingEdges.size() == m_wordCount) {
	// 	wordCountCheck = true;
	// } else {
	// 	STD_ERR("Word count not consistent");
	// }

	// return (complete*wordCountCheck);

	return complete;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool Packet::isConsistent() const {
	if (m_tdcIDHeader != m_tdcIDTrailer) {
		return false;
	}

	if (m_eventIDHeader != m_eventIDTrailer) {
		return false;
	}

	if (m_leadingEdges.size() > 0) {
		for (auto& edge : m_leadingEdges) {
			if (edge.getTDCID() != m_tdcIDHeader) {
				return false;
			}
		}
	}

	if (m_trailingEdges.size() > 0) {
		for (auto& edge : m_trailingEdges) {
			if (edge.getTDCID() != m_tdcIDHeader) {
				return false;
			}
		}
	}

	return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Packet::addTrailer(
	const unsigned int word
) {
	ASSERT(3 == bindec::getDataType(word));

	if (false == m_trailerAdded) {
		m_tdcIDTrailer = bindec::getTDCID(word);
		m_eventIDTrailer = bindec::getEventID(word);
		m_wordCount = bindec::getWordCount(word);
		m_trailerAdded = true;
	} else {
		// Log Error
		ErrorSpy::getInstance().logError("Duplicate Packet Trailer",m_readoutBoardID,m_tdcIDHeader);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Packet::addDataline(
	unsigned int word
) {
	const unsigned int dataType = bindec::getDataType(word);
	ASSERT(4 == dataType || 5 == dataType);

	// Apply polarity corrections
	m_polarityFixer(word);

	const unsigned int tdcID = bindec::getTDCID(word);

	// Apply Channel Mapping
	unsigned int channelID = bindec::getChannelID(word);
	channelID = m_channelMapper(m_readoutBoardID, tdcID, channelID);

	const unsigned int timestamp = bindec::getTimestamp(word);

	if (4 == dataType) {
		m_leadingEdges.emplace_back(tdcID,channelID,timestamp);
	} else if (5 == dataType) {
		m_trailingEdges.emplace_back(tdcID,channelID,timestamp);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Edge Packet::getEdge(
	const bool leading,
	const unsigned int edgeNo
) const {
	if (true == leading) {
		ASSERT(edgeNo < m_leadingEdges.size());
		return m_leadingEdges.at(edgeNo);
	} else {
		ASSERT(edgeNo < m_trailingEdges.size());
		return m_trailingEdges.at(edgeNo);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int Packet::getChannelID(
	const bool leading,
	const unsigned int edgeNo
) const {
	// if (true == leading) {
	// 	ASSERT(edgeNo < m_leadingEdges.size());
	// 	return m_leadingEdges.at(edgeNo).getChannelID();
	// } else {
	// 	ASSERT(edgeNo < m_trailingEdges.size());
	// 	return m_trailingEdges.at(edgeNo).getChannelID();
	// }
	return getEdgeValue(leading, edgeNo, &Edge::getChannelID);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int Packet::getTimestamp(
	const bool leading,
	const unsigned int edgeNo
) const {
	// if (true == leading) {
	// 	ASSERT(edgeNo < m_leadingEdges.size());
	// 	return m_leadingEdges.at(edgeNo).getTimestamp();
	// } else {
	// 	ASSERT(edgeNo < m_trailingEdges.size());
	// 	return m_trailingEdges.at(edgeNo).getTimestamp();
	// }
	return getEdgeValue(leading, edgeNo, &Edge::getTimestamp);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int Packet::getFineTimestamp(
	const bool leading,
	const unsigned int edgeNo
) const {
	// if (true == leading) {
	// 	ASSERT(edgeNo < m_leadingEdges.size());
	// 	return m_leadingEdges.at(edgeNo).getFineTimestamp();
	// } else {
	// 	ASSERT(edgeNo < m_trailingEdges.size());
	// 	return m_trailingEdges.at(edgeNo).getFineTimestamp();
	// }
	return getEdgeValue(leading, edgeNo, &Edge::getFineTimestamp);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Packet::print() const {
	// Print Header Information
	std::cout << "Packet Info: " << ((!this->isConsistent()) ? "Not Consistent!" : "") << std::endl;
	std::cout << "\tTDC: " << m_tdcIDHeader << std::endl;
	std::cout << "\tEvt: " << m_tdcIDHeader << std::endl;
	std::cout << "\tBch: " << m_bunchID << std::endl;

	if (0 == m_leadingEdges.size() && 0 == m_trailingEdges.size()) {
		std::cout << "\tEmpty" << std::endl;
	} else {
		// Print Leading Edges
		for (auto& edge : m_leadingEdges) {
			std::cout << "\tL Edge: " << edge.getChannelID() << "\tTime: " << edge.getTimestamp() << std::endl;
		}

		// Print Trailing Edges
		for (auto& edge : m_trailingEdges) {
			std::cout << "\tT Edge: " << edge.getChannelID() << "\tTime: " << edge.getTimestamp() << std::endl;
		}
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Packet::addHeader(
	const unsigned int word
) {
	ASSERT(2 == bindec::getDataType(word));

	// As header is added by constructor, should never be called with a header already in place
	ASSERT(false == m_headerAdded);

	m_tdcIDHeader = bindec::getTDCID(word);
	m_eventIDHeader = bindec::getEventID(word);
	m_bunchID = bindec::getBunchID(word);
	m_headerAdded = true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int Packet::getEdgeValue(
	const bool leading,
	const unsigned int edgeNo,
	unsigned int (Edge::*getter)() const
) const {
	if (true == leading) {
		ASSERT(edgeNo < m_leadingEdges.size());
		return (m_leadingEdges.at(edgeNo).*getter)();
	} else {
		ASSERT(edgeNo < m_trailingEdges.size());
		return (m_trailingEdges.at(edgeNo).*getter)();
	}
}
