#include "Packet.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Packet::Packet(
	const unsigned int rocValue
) :
m_rocValue(rocValue),
m_tdcIDHeader(0),
m_eventIDHeader(0),
m_bunchID(0),
m_tdcIDTrailer(0),
m_eventIDTrailer(0),
m_wordCount(0)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Packet::~Packet() {

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool Packet::isComplete() {
	const bool complete = m_headerAdded*m_trailerAdded;

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
bool Packet::isConsistent() {
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
void Packet::addHeader(
	const unsigned int word
) {
	// Check initial digits are 02
	m_tdcIDHeader = word << 4 >> 28;
	m_eventIDHeader = word << 8 >> 20;
	m_bunchID = word << 20 >> 20;

	m_headerAdded = true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Packet::addTrailer(
	const unsigned int word
) {
	// Check initial digits are 03
	m_tdcIDTrailer = word << 4 >> 28;
	m_eventIDTrailer = word << 8 >> 20;
	m_wordCount = word << 20 >> 20;

	m_trailerAdded = true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Packet::addDataline(
	const unsigned int word
) {
	const unsigned int tdcID = word << 4 >> 28;
	unsigned int channelID = word << 8 >> 27;
	// channelID += 256 * <readout_board_ID> // Eventually need to be implemented

	// Encode TDC ID in channel ID
	// Note 11 is += 0 so is not included
	if (8 == tdcID) {
		channelID += 160;
	} else if (9 == tdcID) {
		channelID += 128;
	} else if (10 == tdcID) {
		channelID += 32;
	} else if (12 == tdcID) {
		channelID += 96;
	} else if (13 == tdcID) {
		channelID += 64;
	} else if (14 == tdcID) {
		channelID += 224;
	} else if (15 == tdcID) {
		channelID += 192;
	}

	const unsigned int timestamp = word << 13 >> 13;

	const unsigned int dataType = word >> 28; // Store final 4 bits
	if (4 == dataType) {
		m_leadingEdges.emplace_back(tdcID,channelID,timestamp);
	} else if (5 == dataType) {
		m_trailingEdges.emplace_back(tdcID,channelID,timestamp);
	} else {
		STD_ERR("Incorrect Data type passed to addDataline()");
	}

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Edge Packet::getEdge(
	const bool leading,
	const unsigned int edgeNo
) {
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
) {
	if (true == leading) {
		ASSERT(edgeNo < m_leadingEdges.size());
		return m_leadingEdges.at(edgeNo).getChannelID();
	} else {
		ASSERT(edgeNo < m_trailingEdges.size());
		return m_trailingEdges.at(edgeNo).getChannelID();
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int Packet::getTimestamp(
	const bool leading,
	const unsigned int edgeNo
) {
	if (true == leading) {
		ASSERT(edgeNo < m_leadingEdges.size());
		return m_leadingEdges.at(edgeNo).getTimestamp();
	} else {
		ASSERT(edgeNo < m_trailingEdges.size());
		return m_trailingEdges.at(edgeNo).getTimestamp();
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int Packet::getFineTimestamp(
	const bool leading,
	const unsigned int edgeNo
) {
	if (true == leading) {
		ASSERT(edgeNo < m_leadingEdges.size());
		return m_leadingEdges.at(edgeNo).getFineTimestamp();
	} else {
		ASSERT(edgeNo < m_trailingEdges.size());
		return m_trailingEdges.at(edgeNo).getFineTimestamp();
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// void Packet::writeToOutfile(
// 	std::shared_ptr<DebugStreamManager> dsm,
// 	const bool eventForm
// ) {
// 	std::string streamKey = "packetLevel";
// 	if (true == eventForm) {
// 		streamKey = "eventLevel";
// 	}
// 	// Write Header Line
// 	dsm->getStream(streamKey) << "00."; // Readout Board number
// 	if (m_tdcIDHeader < 10) dsm->getStream(streamKey) << 0; // Print padding
// 	dsm->getStream(streamKey) << m_tdcIDHeader;
// 	dsm->getStream(streamKey) << " " << m_eventIDHeader;
// 	dsm->getStream(streamKey) << " " << m_bunchID;
// 	dsm->getStream(streamKey) << " " << m_rocValue;
// 	dsm->getStream(streamKey) << std::endl;
//
// 	// Write Status Line
// 	dsm->getStream(streamKey) << "00."; // Readout Board number
// 	if (m_tdcIDHeader < 10) dsm->getStream(streamKey) << 0; // Print padding
// 	dsm->getStream(streamKey) << m_tdcIDHeader;
// 	dsm->getStream(streamKey) << " 1";
// 	dsm->getStream(streamKey) << " " << m_leadingEdges.size();
// 	dsm->getStream(streamKey) << " " << m_trailingEdges.size();
// 	dsm->getStream(streamKey) << " 1";
// 	dsm->getStream(streamKey) << " " << m_wordCount;
// 	dsm->getStream(streamKey) << std::endl;
//
// 	// Write Leading Edges
// 	for (auto& edge : m_leadingEdges) {
// 		if (true == eventForm) {
// 			dsm->getStream(streamKey) << "00."; // Readout Board number
// 			if (edge.getTDCID() < 10) dsm->getStream(streamKey) << 0; // Print padding
// 			dsm->getStream(streamKey) << edge.getTDCID() << " ";
// 		} else {
// 			dsm->getStream(streamKey) << edge.getTDCID() << " ";
// 		}
// 		dsm->getStream(streamKey) << "1 ";
// 		dsm->getStream(streamKey) << edge.getChannelID() << " ";
// 		dsm->getStream(streamKey) << edge.getTimestamp() << " ";
// 		dsm->getStream(streamKey) << edge.getFineTimestamp();
// 		dsm->getStream(streamKey) << std::endl;
// 	}
//
// 	// Write Trailing Edges
// 	for (auto& edge : m_trailingEdges) {
// 		if (true == eventForm) {
// 			dsm->getStream(streamKey) << "00."; // Readout Board number
// 			if (edge.getTDCID() < 10) dsm->getStream(streamKey) << 0; // Print padding
// 			dsm->getStream(streamKey) << edge.getTDCID() << " ";
// 		} else {
// 			dsm->getStream(streamKey) << edge.getTDCID() << " ";
// 		}
// 		dsm->getStream(streamKey) << "0 ";
// 		dsm->getStream(streamKey) << edge.getChannelID() << " ";
// 		dsm->getStream(streamKey) << edge.getTimestamp() << " ";
// 		dsm->getStream(streamKey) << edge.getFineTimestamp();
// 		dsm->getStream(streamKey) << std::endl;
// 	}
// }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
