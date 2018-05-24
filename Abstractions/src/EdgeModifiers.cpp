#include "EdgeModifiers.hpp"

// LOCAL
#include "BinaryDecoding.hpp"

void edgmod::setEdgeModifier(
	const std::string key
) {
	if ("NoChange" == key) {
		Packet::setEdgeModifier(edgmod::noChange);
	} else if ("FlipEven" == key) {
		Packet::setEdgeModifier(edgmod::flipEven);
	} else if ("FlipOdd" == key) {
		Packet::setEdgeModifier(edgmod::flipOdd);
	} else {
		STD_ERR("Unrecognised Polarity Modifier Key: " << key);
		exit(0);
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
edgmod::EdgeModifier edgmod::noChange = [] (
	unsigned int& word
) -> void { };
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
edgmod::EdgeModifier edgmod::flipEven = [] (
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
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
edgmod::EdgeModifier edgmod::flipOdd = [] (
	unsigned int& word
) -> void {
	const auto dataType = bindec::getDataType(word);
	ASSERT(4 == dataType || 5 == dataType);

	// If channel is even
	const auto channelID = bindec::getChannelID(word);
	if (1 == channelID%2) {
		// Flip polarity
		if (dataType == 4) {
			word += 0x10000000;
		} else {
			word -= 0x10000000;
		}
	}
};
