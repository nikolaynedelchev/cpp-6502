file(GLOB_RECURSE CPU6502_SOURCE_FILES  ./*.cpp
                                        ./*.c)

add_library(cpu6502 STATIC ${CPU6502_SOURCE_FILES})

target_link_libraries(cpu6502 PUBLIC common membus)
target_include_directories(cpu6502 PUBLIC ./../)
