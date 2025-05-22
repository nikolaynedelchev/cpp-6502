#pragma once
#include "common.h"

namespace cpp6502
{
#define SIC static inline constexpr
struct Bitwise
{

    SIC Byte    Bit         (Byte byte, Byte bit)               noexcept {return ((byte & (1 << bit)) >> bit);}
    SIC void    SetBit      (Byte&byte, Byte bit, Byte value)   noexcept {byte = (value ? (byte | (1 << bit)) : (byte & ~(1 << bit)));}
    SIC Byte    Sign        (Byte byte)                         noexcept {return Bit(byte, 7);}
    SIC Word    BytesToWord (auto bytes)                        noexcept {return (Word(bytes[0]) | (Word(bytes[1]) << 8));}
    SIC Byte    HiByte      (Word word)                         noexcept {return Byte(word >> 8);}
    SIC Byte    LoByte      (Word word)                         noexcept {return Byte(word);}
    SIC Word    ClrHiByte   (Word word)                         noexcept {return (word & Word(0xff));}
    SIC Word    ClrLoByte   (Word word)                         noexcept {return (word & (Word(0xff) << 8));}
};

}
