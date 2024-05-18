add_library(logger punkt_logger.cc)
target_link_libraries(logger ${LLVM_LINKER_FLAGS})
