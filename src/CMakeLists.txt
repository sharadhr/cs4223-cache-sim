add_executable(
		coherence
		Cache.cpp
		DragonBus.cpp
		MESIBus.cpp
		MOESIBus.cpp
		Processor.cpp
		Runner.cpp
		Runner.cpp
		System.cpp)

set_target_properties(coherence PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
		${PROJECT_SOURCE_DIR})
