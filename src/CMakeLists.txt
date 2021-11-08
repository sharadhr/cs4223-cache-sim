add_executable(coherence Runner.cpp ProcessorPool.cpp Processor.cpp Runner.cpp CoreMonitor.cpp)
set_target_properties(coherence PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
