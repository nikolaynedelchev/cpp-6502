file(GLOB_RECURSE EMU6502_SOURCE_FILES  ./*.cpp
                                        ./*.c)


add_executable(emul6502 ${EMU6502_SOURCE_FILES})
target_link_libraries(emul6502 PRIVATE fmt::fmt membus cpu6502 ram rom)
