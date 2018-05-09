MESSAGE( STATUS "Adding Abstractions Sources")

set(SOURCE_FILES ${SOURCE_FILES}
	Abstractions/src/Edge.cpp
	Abstractions/src/Event.cpp
	Abstractions/src/Packet.cpp
)

include_directories( Abstractions/inc )
