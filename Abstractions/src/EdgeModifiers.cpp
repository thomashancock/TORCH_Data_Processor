#include "EdgeModifiers.hpp"

// LOCAL
#include "BinaryDecoding.hpp"

void PolMod::setEdgeModifier(
	const std::string key
) {
	if ("NoChange" == key) {
		Packet::setEdgeModifier(PolMod::noChange);
	} else if ("FlipEven" == key) {
		Packet::setEdgeModifier(PolMod::flipEven);
	} else {
		STD_ERR("Unrecognised Polarity Modifier Key: " << key);
		exit(0);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PolMod::EdgeModifier PolMod::noChange = [] (
	unsigned int& word
) -> void { };
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PolMod::EdgeModifier PolMod::flipEven = [] (
	unsigned int& word
) -> void {
	const auto dataType = bindec::getDataType(word);
	ASSERT(4 == dataType || 5 == dataType);

	// If channel is even
	const auto channelID = bindec::getChannelID(word);
	if (0 == channelID%2) {
		// Flip polarity
		if (dataType == 4) {
			word += 0x10000000;
		} else {
			word -= 0x10000000;
		}
	}
};
