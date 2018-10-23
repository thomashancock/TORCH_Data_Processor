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
void Packet::setEdgeModifier(
	std::function<void(uint&)> modifierFunction
) {
	m_edgeModifier = modifierFunction;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// Default initialisation is to return channel ID
std::function<uint(uint, uint, uint)> Packet::m_channelMapper = [] (
	uint ,
	uint ,
	uint channelID
) {
	ASSERT(false); // This function should never be called
	return channelID;
};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// Default initialization is to do nothing
std::function<void(uint&)> Packet::m_edgeModifier = [] (uint&) {
	ASSERT(false); // This function should never be called
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Packet::Packet(
	const BoardIdentifier& boardID,
	const unsigned int rocValue,
	const unsigned int headerWord
) :
	m_boardID(boardID),
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
	// if (!complete) std::cout << "Incomplete packet " << m_tdcIDHeader << std::endl;
	return complete;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool Packet::isConsistent() const {
	if (m_tdcIDHeader != m_tdcIDTrailer) {
		// std::cout << "Mismatched Header & Trailer TDC ID " << m_tdcIDHeader << std::endl;
		return false;
	}

	if (m_eventIDHeader != m_eventIDTrailer) {
		// std::cout << "Mismatched Header & Trailer Event ID " << m_tdcIDHeader << std::endl;
		return false;
	}

	if (m_leadingEdges.size() > 0) {
		for (auto& edge : m_leadingEdges) {
			if (edge.getTDCID() != m_tdcIDHeader) {
				// std::cout << "Mismatched Edge TDC ID " << m_tdcIDHeader << std::endl;
				return false;
			}
		}
	}

	if (m_trailingEdges.size() > 0) {
		for (auto& edge : m_trailingEdges) {
			if (edge.getTDCID() != m_tdcIDHeader) {
				// std::cout << "Mismatched Edge TDC ID " << m_tdcIDHeader << " " << edge.getTDCID() << std::endl;
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
		const auto wordTDCID = bindec::getTDCID(word);
		if (wordTDCID == m_tdcIDTrailer) {
			ErrorSpy::getInstance().logError("Duplicate Packet Trailer Found",ReadoutIdentifier(m_boardID, m_tdcIDTrailer));
		} else {
			ErrorSpy::getInstance().logError("Erroneous Trailer Found",ReadoutIdentifier(m_boardID, wordTDCID));
		}
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Packet::addDataline(
	unsigned int word
) {
	ASSERT(4 == bindec::getDataType(word) || 5 == bindec::getDataType(word));

	// Apply edge polarity correction
	m_edgeModifier(word);

	const unsigned int dataType = bindec::getDataType(word);
	const unsigned int tdcID = bindec::getTDCID(word);

	// Apply Channel Mapping
	unsigned int channelID = bindec::getChannelID(word);
	channelID = m_channelMapper(m_boardID.getDeviceID(), tdcID, channelID);

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
	return getEdgeValue(leading, edgeNo, &Edge::getChannelID);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int Packet::getTimestamp(
	const bool leading,
	const unsigned int edgeNo
) const {
	return getEdgeValue(leading, edgeNo, &Edge::getTimestamp);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int Packet::getFineTimestamp(
	const bool leading,
	const unsigned int edgeNo
) const {
	return getEdgeValue(leading, edgeNo, &Edge::getFineTimestamp);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Packet::print() const {
	// Print Header Information
	std::cout << "Packet Info: " << ((!this->isConsistent()) ? "Not Consistent!" : "") << std::endl;
	std::cout << "\tTDC: " << m_tdcIDHeader << std::endl;
	std::cout << "\tEvt: " << m_eventIDHeader << std::endl;
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
