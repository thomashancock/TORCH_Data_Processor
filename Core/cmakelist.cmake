MESSAGE( STATUS "Adding Core Sources")

set(SOURCE_FILES ${SOURCE_FILES}
	Core/src/main.cpp
	Core/src/Config.cpp
	Core/src/Processor.cpp
	Core/src/RootManager.cpp
)

include_directories( Core/inc )
