file(GLOB_RECURSE COMMON_SOURCE_FILES  ./*.cpp
                                    ./*.c)

add_library(common STATIC ${COMMON_SOURCE_FILES})

target_link_libraries(common PUBLIC fmt::fmt)
target_include_directories(common PUBLIC ./../)
