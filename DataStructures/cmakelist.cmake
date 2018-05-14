MESSAGE( STATUS "Adding DataStructures")

set(SOURCE_FILES ${SOURCE_FILES}
	DataStructures/src/ThreadSafeEventMap.cpp
)

include_directories( DataStructures/inc )
