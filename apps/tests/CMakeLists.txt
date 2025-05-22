file(GLOB_RECURSE TESTS_SOURCE_FILES   ./*.cpp
                                       ./*.c)


add_executable(tests ${TESTS_SOURCE_FILES})
target_link_libraries(tests PRIVATE fmt::fmt common membus cpu6502 ram rom)

