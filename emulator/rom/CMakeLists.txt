file(GLOB_RECURSE ROM_SOURCE_FILES  ./*.cpp
                                    ./*.c)

add_library(rom STATIC ${ROM_SOURCE_FILES})

target_link_libraries(rom PUBLIC common membus)
target_include_directories(rom PUBLIC ./../)
