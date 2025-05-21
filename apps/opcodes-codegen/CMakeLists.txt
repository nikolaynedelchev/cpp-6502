file(GLOB_RECURSE OPCODE_CODEGEN_SOURCE_FILES   ./*.cpp
                                                ./*.c)


add_executable(opcodes-codegen ${OPCODE_CODEGEN_SOURCE_FILES})
target_link_libraries(opcodes-codegen PRIVATE fmt::fmt common)
