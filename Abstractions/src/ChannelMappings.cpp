#include "ChannelMappings.hpp"

void chlmap::setChannelMapping(
	const std::string key
) {
	if ("NoMapping" == key) {
		Packet::setChannelMapping(chlmap::noMapping);
	} else if ("Std8x64Mappingfor2MCPs" == key) {
		Packet::setChannelMapping(chlmap::std8x64Mappingfor2MCPs);
	} else if ("Std8x64Mapping" == key) {
		Packet::setChannelMapping(chlmap::std8x64Mapping);
	} else if ("Inv8x64Mapping" == key) {
		Packet::setChannelMapping(chlmap::slotInversion8x64Mapping);
	} else if ("Std4x64Mapping" == key) {
		Packet::setChannelMapping(chlmap::std4x64Mapping);
	} else {
		STD_ERR("Unrecognised Channel Mapping Key: " << key);
		exit(0);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
chlmap::ChannelMapping chlmap::noMapping = [] (
	const BoardIdentifier& ,
	const uint ,
	const uint channelID
) {
	return channelID;
};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
chlmap::ChannelMapping chlmap::std8x64Mapping = [] (
	const BoardIdentifier& ,
	const uint tdcID,
	const uint channelID
) {
	uint localID = (tdcID%4)*16 + 128 * static_cast<int>(tdcID / 4.0);
	if (0 == channelID%2) {
		localID += 15 - static_cast<int>(channelID / 2); // Integer division
	} else {
		localID += 79 - static_cast<int>((channelID - 1) / 2); // Integer division
	}
	return localID;
};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
chlmap::ChannelMapping chlmap::std8x64Mappingfor2MCPs = [] (
	const BoardIdentifier& identifier,
	const uint tdcID,
	const uint channelID
) {
	uint localID = (tdcID%4)*16 + 128 * static_cast<int>(tdcID / 4.0);
	if (0 == channelID%2) {
		localID += 15 - static_cast<int>(channelID / 2); // Integer division
	} else {
		localID += 79 - static_cast<int>((channelID - 1) / 2); // Integer division
	}
	localID += 256*identifier.getChainID();
	return localID;
};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
chlmap::ChannelMapping chlmap::slotInversion8x64Mapping = [] (
	const BoardIdentifier& identifier,
	const uint tdcID,
	const uint channelID
) {
	// Swap localTDCID to account for inversion
	const auto localTDCID = (0 == tdcID%2) ? tdcID + 1 : tdcID - 1;

	// Calculate channel ID using standard mapping
	return std8x64Mapping(identifier, localTDCID, channelID);
};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
chlmap::ChannelMapping chlmap::std4x64Mapping = [] (
	const BoardIdentifier& identifier,
	const uint tdcID,
	const uint channelID
) {
	// Encode TDC ID in channel ID
	// Note 11 is += 0 so is not included
	auto localID = channelID;
	if (8 == tdcID) {
		localID += 160;
	} else if (9 == tdcID) {
		localID += 128;
	} else if (10 == tdcID) {
		localID += 32;
	} else if (12 == tdcID) {
		localID += 96;
	} else if (13 == tdcID) {
		localID += 64;
	} else if (14 == tdcID) {
		localID += 224;
	} else if (15 == tdcID) {
		localID += 192;
	}

	localID += 256 * identifier.getDeviceID();

	return localID;
};
