file(GLOB_RECURSE TGT_SOURCE_FILES  ./*.cpp
                                    ./*.c)

add_library(common STATIC ${TGT_SOURCE_FILES})

target_link_libraries(common PUBLIC fmt::fmt)
target_include_directories(common PUBLIC ./../)
