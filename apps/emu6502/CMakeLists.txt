file(GLOB_RECURSE JP_DEMO_SOURCE_FILES  ./*.cpp
                                        ./*.c)


add_executable(emul6502 main.cpp)
target_link_libraries(emul6502 PRIVATE fmt::fmt membus cpu6502 ram rom)
