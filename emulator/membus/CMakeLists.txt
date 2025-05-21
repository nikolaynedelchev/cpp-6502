file(GLOB_RECURSE MEMBUS_SOURCE_FILES  ./*.cpp
                                        ./*.c)

add_library(membus STATIC ${MEMBUS_SOURCE_FILES})

target_link_libraries(membus PUBLIC common)
target_include_directories(membus PUBLIC ./../)
