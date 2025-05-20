file(GLOB_RECURSE TGT_SOURCE_FILES  ./*.cpp
                                    ./*.c)

add_library(membus STATIC ${TGT_SOURCE_FILES})

target_link_libraries(membus PUBLIC common)
target_include_directories(membus PUBLIC ./../)
