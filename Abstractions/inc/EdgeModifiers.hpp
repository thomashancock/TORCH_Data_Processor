#ifndef EDGEMODIFIERS_H
#define EDGEMODIFIERS_H

// STD
#include <functional>

// LOCAL
#include "Packet.hpp"

namespace edgmod {

// Useful Alias
using EdgeModifier = std::function<void(unsigned int&)>;

//! Global function to set edge modifier
void setEdgeModifier(
	const std::string key //!< Key to identify desired edge modifier
);

// Edge Modifier delcarations
// extern facilitates declaration in seperate source file
extern EdgeModifier noChange; //!< Don't apply any changes
extern EdgeModifier flipEven; //!< Even channel edge flip

};

#endif /* EDGEMODIFIERS_H */
