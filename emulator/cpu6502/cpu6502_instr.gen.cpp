
// AUTO GENERATED FILE
// CHECK 'opcodes-codegen'

#include "cpu6502Impl.h"

namespace cpp6502
{

const Cpu6502::Impl::Meta::InstructionsLookup &Cpu6502::Impl::Meta::Instructions()
{
    static InstructionsLookup s_lookup = {

        {
            .opcode = 0x00,
            .name = "BRK",
            .memoryMode = MemAcc_Implied,
            .description = "Force Interrupt",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_BRK
        },

        {
            .opcode = 0x01,
            .name = "ORA",
            .memoryMode = MemAcc_Indexed_X,
            .description = "Logical Inclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ORA
        },

        {
            .opcode = 0x02,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x03,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x04,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x05,
            .name = "ORA",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Logical Inclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ORA
        },

        {
            .opcode = 0x06,
            .name = "ASL",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Arithmetic Shift Left",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ASL
        },

        {
            .opcode = 0x07,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x08,
            .name = "PHP",
            .memoryMode = MemAcc_Implied,
            .description = "Push Processor Status",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_PHP
        },

        {
            .opcode = 0x09,
            .name = "ORA",
            .memoryMode = MemAcc_Immediate,
            .description = "Logical Inclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ORA
        },

        {
            .opcode = 0x0A,
            .name = "ASL",
            .memoryMode = MemAcc_Accumulator,
            .description = "Arithmetic Shift Left",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_ASL
        },

        {
            .opcode = 0x0B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x0C,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x0D,
            .name = "ORA",
            .memoryMode = MemAcc_Absolute,
            .description = "Logical Inclusive OR",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ORA
        },

        {
            .opcode = 0x0E,
            .name = "ASL",
            .memoryMode = MemAcc_Absolute,
            .description = "Arithmetic Shift Left",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ASL
        },

        {
            .opcode = 0x0F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x10,
            .name = "BPL",
            .memoryMode = MemAcc_Relative,
            .description = "Branch if Positive",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_BPL
        },

        {
            .opcode = 0x11,
            .name = "ORA",
            .memoryMode = MemAcc_Indexed_Y,
            .description = "Logical Inclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ORA
        },

        {
            .opcode = 0x12,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x13,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x14,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x15,
            .name = "ORA",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Logical Inclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ORA
        },

        {
            .opcode = 0x16,
            .name = "ASL",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Arithmetic Shift Left",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ASL
        },

        {
            .opcode = 0x17,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x18,
            .name = "CLC",
            .memoryMode = MemAcc_Implied,
            .description = "Clear Carry Flag",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_CLC
        },

        {
            .opcode = 0x19,
            .name = "ORA",
            .memoryMode = MemAcc_Absolute_Y,
            .description = "Logical Inclusive OR",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ORA
        },

        {
            .opcode = 0x1A,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x1B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x1C,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x1D,
            .name = "ORA",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Logical Inclusive OR",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ORA
        },

        {
            .opcode = 0x1E,
            .name = "ASL",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Arithmetic Shift Left",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ASL
        },

        {
            .opcode = 0x1F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x20,
            .name = "JSR",
            .memoryMode = MemAcc_Absolute,
            .description = "Jump to Subroutine",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_JSR
        },

        {
            .opcode = 0x21,
            .name = "AND",
            .memoryMode = MemAcc_Indexed_X,
            .description = "Logical AND",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_AND
        },

        {
            .opcode = 0x22,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x23,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x24,
            .name = "BIT",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Bit Test",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_BIT
        },

        {
            .opcode = 0x25,
            .name = "AND",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Logical AND",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_AND
        },

        {
            .opcode = 0x26,
            .name = "ROL",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Rotate Left",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ROL
        },

        {
            .opcode = 0x27,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x28,
            .name = "PLP",
            .memoryMode = MemAcc_Implied,
            .description = "Pull Processor Status",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_PLP
        },

        {
            .opcode = 0x29,
            .name = "AND",
            .memoryMode = MemAcc_Immediate,
            .description = "Logical AND",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_AND
        },

        {
            .opcode = 0x2A,
            .name = "ROL",
            .memoryMode = MemAcc_Accumulator,
            .description = "Rotate Left",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_ROL
        },

        {
            .opcode = 0x2B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x2C,
            .name = "BIT",
            .memoryMode = MemAcc_Absolute,
            .description = "Bit Test",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_BIT
        },

        {
            .opcode = 0x2D,
            .name = "AND",
            .memoryMode = MemAcc_Absolute,
            .description = "Logical AND",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_AND
        },

        {
            .opcode = 0x2E,
            .name = "ROL",
            .memoryMode = MemAcc_Absolute,
            .description = "Rotate Left",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ROL
        },

        {
            .opcode = 0x2F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x30,
            .name = "BMI",
            .memoryMode = MemAcc_Relative,
            .description = "Branch if Minus",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_BMI
        },

        {
            .opcode = 0x31,
            .name = "AND",
            .memoryMode = MemAcc_Indexed_Y,
            .description = "Logical AND",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_AND
        },

        {
            .opcode = 0x32,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x33,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x34,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x35,
            .name = "AND",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Logical AND",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_AND
        },

        {
            .opcode = 0x36,
            .name = "ROL",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Rotate Left",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ROL
        },

        {
            .opcode = 0x37,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x38,
            .name = "SEC",
            .memoryMode = MemAcc_Implied,
            .description = "Set Carry Flag",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_SEC
        },

        {
            .opcode = 0x39,
            .name = "AND",
            .memoryMode = MemAcc_Absolute_Y,
            .description = "Logical AND",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_AND
        },

        {
            .opcode = 0x3A,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x3B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x3C,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x3D,
            .name = "AND",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Logical AND",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_AND
        },

        {
            .opcode = 0x3E,
            .name = "ROL",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Rotate Left",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ROL
        },

        {
            .opcode = 0x3F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x40,
            .name = "RTI",
            .memoryMode = MemAcc_Implied,
            .description = "Return from Interrupt",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_RTI
        },

        {
            .opcode = 0x41,
            .name = "EOR",
            .memoryMode = MemAcc_Indexed_X,
            .description = "Exclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_EOR
        },

        {
            .opcode = 0x42,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x43,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x44,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x45,
            .name = "EOR",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Exclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_EOR
        },

        {
            .opcode = 0x46,
            .name = "LSR",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Logical Shift Right",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LSR
        },

        {
            .opcode = 0x47,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x48,
            .name = "PHA",
            .memoryMode = MemAcc_Implied,
            .description = "Push Accumulator",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_PHA
        },

        {
            .opcode = 0x49,
            .name = "EOR",
            .memoryMode = MemAcc_Immediate,
            .description = "Exclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_EOR
        },

        {
            .opcode = 0x4A,
            .name = "LSR",
            .memoryMode = MemAcc_Accumulator,
            .description = "Logical Shift Right",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_LSR
        },

        {
            .opcode = 0x4B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x4C,
            .name = "JMP",
            .memoryMode = MemAcc_Absolute,
            .description = "Jump",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_JMP
        },

        {
            .opcode = 0x4D,
            .name = "EOR",
            .memoryMode = MemAcc_Absolute,
            .description = "Exclusive OR",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_EOR
        },

        {
            .opcode = 0x4E,
            .name = "LSR",
            .memoryMode = MemAcc_Absolute,
            .description = "Logical Shift Right",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_LSR
        },

        {
            .opcode = 0x4F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x50,
            .name = "BVC",
            .memoryMode = MemAcc_Relative,
            .description = "Branch if Overflow Clear",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_BVC
        },

        {
            .opcode = 0x51,
            .name = "EOR",
            .memoryMode = MemAcc_Indexed_Y,
            .description = "Exclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_EOR
        },

        {
            .opcode = 0x52,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x53,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x54,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x55,
            .name = "EOR",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Exclusive OR",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_EOR
        },

        {
            .opcode = 0x56,
            .name = "LSR",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Logical Shift Right",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LSR
        },

        {
            .opcode = 0x57,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x58,
            .name = "CLI",
            .memoryMode = MemAcc_Implied,
            .description = "Clear Interrupt Disable",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_CLI
        },

        {
            .opcode = 0x59,
            .name = "EOR",
            .memoryMode = MemAcc_Absolute_Y,
            .description = "Exclusive OR",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_EOR
        },

        {
            .opcode = 0x5A,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x5B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x5C,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x5D,
            .name = "EOR",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Exclusive OR",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_EOR
        },

        {
            .opcode = 0x5E,
            .name = "LSR",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Logical Shift Right",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_LSR
        },

        {
            .opcode = 0x5F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x60,
            .name = "RTS",
            .memoryMode = MemAcc_Implied,
            .description = "Return from Subroutine",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_RTS
        },

        {
            .opcode = 0x61,
            .name = "ADC",
            .memoryMode = MemAcc_Indexed_X,
            .description = "Add with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ADC
        },

        {
            .opcode = 0x62,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x63,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x64,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x65,
            .name = "ADC",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Add with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ADC
        },

        {
            .opcode = 0x66,
            .name = "ROR",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Rotate Right",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ROR
        },

        {
            .opcode = 0x67,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x68,
            .name = "PLA",
            .memoryMode = MemAcc_Implied,
            .description = "Pull Accumulator",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_PLA
        },

        {
            .opcode = 0x69,
            .name = "ADC",
            .memoryMode = MemAcc_Immediate,
            .description = "Add with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ADC
        },

        {
            .opcode = 0x6A,
            .name = "ROR",
            .memoryMode = MemAcc_Accumulator,
            .description = "Rotate Right",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_ROR
        },

        {
            .opcode = 0x6B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x6C,
            .name = "JMP",
            .memoryMode = MemAcc_Indirect,
            .description = "Jump",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_JMP
        },

        {
            .opcode = 0x6D,
            .name = "ADC",
            .memoryMode = MemAcc_Absolute,
            .description = "Add with Carry",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ADC
        },

        {
            .opcode = 0x6E,
            .name = "ROR",
            .memoryMode = MemAcc_Absolute,
            .description = "Rotate Right",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ROR
        },

        {
            .opcode = 0x6F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x70,
            .name = "BVS",
            .memoryMode = MemAcc_Relative,
            .description = "Branch if Overflow Set",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_BVS
        },

        {
            .opcode = 0x71,
            .name = "ADC",
            .memoryMode = MemAcc_Indexed_Y,
            .description = "Add with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ADC
        },

        {
            .opcode = 0x72,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x73,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x74,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x75,
            .name = "ADC",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Add with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ADC
        },

        {
            .opcode = 0x76,
            .name = "ROR",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Rotate Right",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_ROR
        },

        {
            .opcode = 0x77,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x78,
            .name = "SEI",
            .memoryMode = MemAcc_Implied,
            .description = "Set Interrupt Disable",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_SEI
        },

        {
            .opcode = 0x79,
            .name = "ADC",
            .memoryMode = MemAcc_Absolute_Y,
            .description = "Add with Carry",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ADC
        },

        {
            .opcode = 0x7A,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x7B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x7C,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x7D,
            .name = "ADC",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Add with Carry",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ADC
        },

        {
            .opcode = 0x7E,
            .name = "ROR",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Rotate Right",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_ROR
        },

        {
            .opcode = 0x7F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x80,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x81,
            .name = "STA",
            .memoryMode = MemAcc_Indexed_X,
            .description = "Store Accumulator",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_STA
        },

        {
            .opcode = 0x82,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x83,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x84,
            .name = "STY",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Store Y Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_STY
        },

        {
            .opcode = 0x85,
            .name = "STA",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Store Accumulator",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_STA
        },

        {
            .opcode = 0x86,
            .name = "STX",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Store X Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_STX
        },

        {
            .opcode = 0x87,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x88,
            .name = "DEY",
            .memoryMode = MemAcc_Implied,
            .description = "Decrement Y Register",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_DEY
        },

        {
            .opcode = 0x89,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x8A,
            .name = "TXA",
            .memoryMode = MemAcc_Implied,
            .description = "Transfer X to Accumulator",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_TXA
        },

        {
            .opcode = 0x8B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x8C,
            .name = "STY",
            .memoryMode = MemAcc_Absolute,
            .description = "Store Y Register",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_STY
        },

        {
            .opcode = 0x8D,
            .name = "STA",
            .memoryMode = MemAcc_Absolute,
            .description = "Store Accumulator",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_STA
        },

        {
            .opcode = 0x8E,
            .name = "STX",
            .memoryMode = MemAcc_Absolute,
            .description = "Store X Register",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_STX
        },

        {
            .opcode = 0x8F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x90,
            .name = "BCC",
            .memoryMode = MemAcc_Relative,
            .description = "Branch if Carry Clear",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_BCC
        },

        {
            .opcode = 0x91,
            .name = "STA",
            .memoryMode = MemAcc_Indexed_Y,
            .description = "Store Accumulator",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_STA
        },

        {
            .opcode = 0x92,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x93,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x94,
            .name = "STY",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Store Y Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_STY
        },

        {
            .opcode = 0x95,
            .name = "STA",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Store Accumulator",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_STA
        },

        {
            .opcode = 0x96,
            .name = "STX",
            .memoryMode = MemAcc_ZeroPage_Y,
            .description = "Store X Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_STX
        },

        {
            .opcode = 0x97,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x98,
            .name = "TYA",
            .memoryMode = MemAcc_Implied,
            .description = "Transfer Y to Accumulator",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_TYA
        },

        {
            .opcode = 0x99,
            .name = "STA",
            .memoryMode = MemAcc_Absolute_Y,
            .description = "Store Accumulator",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_STA
        },

        {
            .opcode = 0x9A,
            .name = "TXS",
            .memoryMode = MemAcc_Implied,
            .description = "Transfer X to Stack Pointer",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_TXS
        },

        {
            .opcode = 0x9B,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x9C,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x9D,
            .name = "STA",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Store Accumulator",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_STA
        },

        {
            .opcode = 0x9E,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0x9F,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xA0,
            .name = "LDY",
            .memoryMode = MemAcc_Immediate,
            .description = "Load Y Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDY
        },

        {
            .opcode = 0xA1,
            .name = "LDA",
            .memoryMode = MemAcc_Indexed_X,
            .description = "Load Accumulator",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDA
        },

        {
            .opcode = 0xA2,
            .name = "LDX",
            .memoryMode = MemAcc_Immediate,
            .description = "Load X Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDX
        },

        {
            .opcode = 0xA3,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xA4,
            .name = "LDY",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Load Y Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDY
        },

        {
            .opcode = 0xA5,
            .name = "LDA",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Load Accumulator",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDA
        },

        {
            .opcode = 0xA6,
            .name = "LDX",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Load X Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDX
        },

        {
            .opcode = 0xA7,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xA8,
            .name = "TAY",
            .memoryMode = MemAcc_Implied,
            .description = "Transfer Accumulator to Y",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_TAY
        },

        {
            .opcode = 0xA9,
            .name = "LDA",
            .memoryMode = MemAcc_Immediate,
            .description = "Load Accumulator",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDA
        },

        {
            .opcode = 0xAA,
            .name = "TAX",
            .memoryMode = MemAcc_Implied,
            .description = "Transfer Accumulator to X",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_TAX
        },

        {
            .opcode = 0xAB,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xAC,
            .name = "LDY",
            .memoryMode = MemAcc_Absolute,
            .description = "Load Y Register",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_LDY
        },

        {
            .opcode = 0xAD,
            .name = "LDA",
            .memoryMode = MemAcc_Absolute,
            .description = "Load Accumulator",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_LDA
        },

        {
            .opcode = 0xAE,
            .name = "LDX",
            .memoryMode = MemAcc_Absolute,
            .description = "Load X Register",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_LDX
        },

        {
            .opcode = 0xAF,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xB0,
            .name = "BCS",
            .memoryMode = MemAcc_Relative,
            .description = "Branch if Carry Set",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_BCS
        },

        {
            .opcode = 0xB1,
            .name = "LDA",
            .memoryMode = MemAcc_Indexed_Y,
            .description = "Load Accumulator",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDA
        },

        {
            .opcode = 0xB2,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xB3,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xB4,
            .name = "LDY",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Load Y Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDY
        },

        {
            .opcode = 0xB5,
            .name = "LDA",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Load Accumulator",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDA
        },

        {
            .opcode = 0xB6,
            .name = "LDX",
            .memoryMode = MemAcc_ZeroPage_Y,
            .description = "Load X Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_LDX
        },

        {
            .opcode = 0xB7,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xB8,
            .name = "CLV",
            .memoryMode = MemAcc_Implied,
            .description = "Clear Overflow Flag",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_CLV
        },

        {
            .opcode = 0xB9,
            .name = "LDA",
            .memoryMode = MemAcc_Absolute_Y,
            .description = "Load Accumulator",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_LDA
        },

        {
            .opcode = 0xBA,
            .name = "TSX",
            .memoryMode = MemAcc_Implied,
            .description = "Transfer Stack Pointer to X",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_TSX
        },

        {
            .opcode = 0xBB,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xBC,
            .name = "LDY",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Load Y Register",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_LDY
        },

        {
            .opcode = 0xBD,
            .name = "LDA",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Load Accumulator",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_LDA
        },

        {
            .opcode = 0xBE,
            .name = "LDX",
            .memoryMode = MemAcc_Absolute_Y,
            .description = "Load X Register",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_LDX
        },

        {
            .opcode = 0xBF,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xC0,
            .name = "CPY",
            .memoryMode = MemAcc_Immediate,
            .description = "Compare Y Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_CPY
        },

        {
            .opcode = 0xC1,
            .name = "CMP",
            .memoryMode = MemAcc_Indexed_X,
            .description = "Compare",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_CMP
        },

        {
            .opcode = 0xC2,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xC3,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xC4,
            .name = "CPY",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Compare Y Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_CPY
        },

        {
            .opcode = 0xC5,
            .name = "CMP",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Compare",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_CMP
        },

        {
            .opcode = 0xC6,
            .name = "DEC",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Decrement Memory",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_DEC
        },

        {
            .opcode = 0xC7,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xC8,
            .name = "INY",
            .memoryMode = MemAcc_Implied,
            .description = "Increment Y Register",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_INY
        },

        {
            .opcode = 0xC9,
            .name = "CMP",
            .memoryMode = MemAcc_Immediate,
            .description = "Compare",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_CMP
        },

        {
            .opcode = 0xCA,
            .name = "DEX",
            .memoryMode = MemAcc_Implied,
            .description = "Decrement X Register",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_DEX
        },

        {
            .opcode = 0xCB,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xCC,
            .name = "CPY",
            .memoryMode = MemAcc_Absolute,
            .description = "Compare Y Register",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_CPY
        },

        {
            .opcode = 0xCD,
            .name = "CMP",
            .memoryMode = MemAcc_Absolute,
            .description = "Compare",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_CMP
        },

        {
            .opcode = 0xCE,
            .name = "DEC",
            .memoryMode = MemAcc_Absolute,
            .description = "Decrement Memory",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_DEC
        },

        {
            .opcode = 0xCF,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xD0,
            .name = "BNE",
            .memoryMode = MemAcc_Relative,
            .description = "Branch if Not Equal",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_BNE
        },

        {
            .opcode = 0xD1,
            .name = "CMP",
            .memoryMode = MemAcc_Indexed_Y,
            .description = "Compare",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_CMP
        },

        {
            .opcode = 0xD2,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xD3,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xD4,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xD5,
            .name = "CMP",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Compare",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_CMP
        },

        {
            .opcode = 0xD6,
            .name = "DEC",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Decrement Memory",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_DEC
        },

        {
            .opcode = 0xD7,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xD8,
            .name = "CLD",
            .memoryMode = MemAcc_Implied,
            .description = "Clear Decimal Mode",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_CLD
        },

        {
            .opcode = 0xD9,
            .name = "CMP",
            .memoryMode = MemAcc_Absolute_Y,
            .description = "Compare",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_CMP
        },

        {
            .opcode = 0xDA,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xDB,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xDC,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xDD,
            .name = "CMP",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Compare",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_CMP
        },

        {
            .opcode = 0xDE,
            .name = "DEC",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Decrement Memory",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_DEC
        },

        {
            .opcode = 0xDF,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xE0,
            .name = "CPX",
            .memoryMode = MemAcc_Immediate,
            .description = "Compare X Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_CPX
        },

        {
            .opcode = 0xE1,
            .name = "SBC",
            .memoryMode = MemAcc_Indexed_X,
            .description = "Subtract with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_SBC
        },

        {
            .opcode = 0xE2,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xE3,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xE4,
            .name = "CPX",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Compare X Register",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_CPX
        },

        {
            .opcode = 0xE5,
            .name = "SBC",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Subtract with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_SBC
        },

        {
            .opcode = 0xE6,
            .name = "INC",
            .memoryMode = MemAcc_ZeroPage,
            .description = "Increment Memory",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_INC
        },

        {
            .opcode = 0xE7,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xE8,
            .name = "INX",
            .memoryMode = MemAcc_Implied,
            .description = "Increment X Register",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_INX
        },

        {
            .opcode = 0xE9,
            .name = "SBC",
            .memoryMode = MemAcc_Immediate,
            .description = "Subtract with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_SBC
        },

        {
            .opcode = 0xEA,
            .name = "NOP",
            .memoryMode = MemAcc_Implied,
            .description = "No Operation",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_NOP
        },

        {
            .opcode = 0xEB,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xEC,
            .name = "CPX",
            .memoryMode = MemAcc_Absolute,
            .description = "Compare X Register",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_CPX
        },

        {
            .opcode = 0xED,
            .name = "SBC",
            .memoryMode = MemAcc_Absolute,
            .description = "Subtract with Carry",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_SBC
        },

        {
            .opcode = 0xEE,
            .name = "INC",
            .memoryMode = MemAcc_Absolute,
            .description = "Increment Memory",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_INC
        },

        {
            .opcode = 0xEF,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xF0,
            .name = "BEQ",
            .memoryMode = MemAcc_Relative,
            .description = "Branch if Equal",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_BEQ
        },

        {
            .opcode = 0xF1,
            .name = "SBC",
            .memoryMode = MemAcc_Indexed_Y,
            .description = "Subtract with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_SBC
        },

        {
            .opcode = 0xF2,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xF3,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xF4,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xF5,
            .name = "SBC",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Subtract with Carry",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_SBC
        },

        {
            .opcode = 0xF6,
            .name = "INC",
            .memoryMode = MemAcc_ZeroPage_X,
            .description = "Increment Memory",
            .size = 2,
            .instruction = &Cpu6502::Impl::Instr_INC
        },

        {
            .opcode = 0xF7,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xF8,
            .name = "SED",
            .memoryMode = MemAcc_Implied,
            .description = "Set Decimal Flag",
            .size = 1,
            .instruction = &Cpu6502::Impl::Instr_SED
        },

        {
            .opcode = 0xF9,
            .name = "SBC",
            .memoryMode = MemAcc_Absolute_Y,
            .description = "Subtract with Carry",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_SBC
        },

        {
            .opcode = 0xFA,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xFB,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xFC,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        },

        {
            .opcode = 0xFD,
            .name = "SBC",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Subtract with Carry",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_SBC
        },

        {
            .opcode = 0xFE,
            .name = "INC",
            .memoryMode = MemAcc_Absolute_X,
            .description = "Increment Memory",
            .size = 3,
            .instruction = &Cpu6502::Impl::Instr_INC
        },

        {
            .opcode = 0xFF,
            .name = "INVALID",
            .memoryMode = MemAcc_INVALID,
            .description = "Opcode not in use",
            .size = 0,
            .instruction = nullptr
        }

    };
    return s_lookup;
}

}
