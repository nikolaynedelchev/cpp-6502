file(GLOB_RECURSE TGT_SOURCE_FILES  ./*.cpp
                                    ./*.c)

add_library(ram STATIC ${TGT_SOURCE_FILES})

target_link_libraries(ram PUBLIC common membus)
target_include_directories(ram PUBLIC ./../)
