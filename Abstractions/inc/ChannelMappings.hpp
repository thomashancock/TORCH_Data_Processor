#ifndef CHANNELMAPPINGS_H
#define CHANNELMAPPINGS_H

// STD
#include <functional>

// LOCAL
#include "Packet.hpp"

namespace chlmap {

// Useful Alias
using ChannelMapping = std::function<unsigned int(unsigned int, unsigned int, unsigned int)>;

//! Global function to set channel mapping
void setChannelMapping(
	const std::string key //!< Key to identify desired mapping
);

// Channel Mapping delcarations
// extern facilitates declaration in seperate source file
extern ChannelMapping noMapping; //!< Raw Electronics Channel ID
extern ChannelMapping std8x64Mapping; //!< 0 - 511 Channel Mapping for 8x64 MCP
extern ChannelMapping slotInversion8x64Mapping; //!< 0 - 511 Channel Mapping for 8x64 MCP
extern ChannelMapping std4x64Mapping; //!< 0 - 255 4x64 Channel Mapping

}

#endif /* CHANNELMAPPINGS_H */
