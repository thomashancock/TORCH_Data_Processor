MESSAGE( STATUS "Adding Abstractions Sources")

set(SOURCE_FILES ${SOURCE_FILES}
	Abstractions/src/Edge.cpp
	Abstractions/src/Event.cpp
	Abstractions/src/Packet.cpp
	Abstractions/src/WordBundle.cpp
)

include_directories( Abstractions/inc )
