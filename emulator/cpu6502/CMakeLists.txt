file(GLOB_RECURSE TGT_SOURCE_FILES  ./*.cpp
                                    ./*.c)

add_library(cpu6502 STATIC ${TGT_SOURCE_FILES})

target_link_libraries(cpu6502 PUBLIC common membus)
target_include_directories(cpu6502 PUBLIC ./../)
