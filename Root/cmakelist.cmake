MESSAGE( STATUS "Adding Root-Based Sources")

set(SOURCE_FILES ${SOURCE_FILES}
	Root/src/EventTreeManager.cpp
	Root/src/PacketTreeManager.cpp
)

include_directories( Root/inc )
