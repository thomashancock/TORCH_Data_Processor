#ifndef CHANNELMAPPINGS_H
#define CHANNELMAPPINGS_H

// STD
#include <functional>

namespace ChlMap {

using uint = unsigned int;

//! Raw Electronics Channel ID
std::function<uint(uint, uint, uint)> noMapping = [] (
	uint readoutBoardID,
	uint tdcID,
	uint channelID
) {
	return channelID;
};

//! 0 - 511 Channel Mapping for 8x64 MCP
std::function<uint(uint, uint, uint)> std8x64Mapping = [] (
	uint readoutBoardID,
	uint tdcID,
	uint channelID
) {
	uint localID = (tdcID%4)*16 + 128 * static_cast<int>(tdcID / 4.0);
	if (0 == channelID%2) {
		localID += 15 - static_cast<int>(channelID / 2); // Integer division
	} else {
		localID += 79 - static_cast<int>((channelID - 1) / 2); // Integer division
	}
	return localID;
};

//! 0 - 255 4x64 Channel Mapping
std::function<uint(uint, uint, uint)> std4x64Mapping = [] (
	uint readoutBoardID,
	uint tdcID,
	uint channelID
) {
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

	channelID += 256 * readoutBoardID;

	return channelID;
};

};

#endif /* CHANNELMAPPINGS_H */
