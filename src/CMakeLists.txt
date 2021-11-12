add_executable(coherence Runner.cpp System.cpp Processor.cpp Runner.cpp CoreMonitor.cpp Cache.cpp MESIBus.cpp DragonBus.cpp Instruction.hpp)
set_target_properties(coherence PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
