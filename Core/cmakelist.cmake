MESSAGE( STATUS "Adding Core Sources")

set(SOURCE_FILES ${SOURCE_FILES}
	Core/src/main.cpp
	Core/src/Config.cpp
	Core/src/Processor.cpp
	Core/src/FileReader.cpp
	Core/src/ErrorSpy.cpp
	Core/src/ErrorCounter.cpp
)

include_directories( Core/inc )
