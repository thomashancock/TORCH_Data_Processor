MESSAGE( STATUS "Adding DataStructures Sources")

set(SOURCE_FILES ${SOURCE_FILES}
	DataStructures/src/ThreadSafeQueue.cpp
)

include_directories( DataStructures/inc )
