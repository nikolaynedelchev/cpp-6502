#pragma once
#include "common.h"

namespace cpp6502
{
#define SIC static inline constexpr
struct Bitwise
{

    SIC Byte    Bit8        (Byte byte, Byte bit)               noexcept {return ((byte >> bit) & 1);}
    SIC Byte    TestBit8    (Byte byte, Byte bit)               noexcept {return (byte & (1 << bit));}
    SIC void    SetBit8     (Byte&byte, Byte bit, Byte value)   noexcept {byte = (value ? (byte | (1 << bit)) : (byte & ~(1 << bit)));}
    SIC Byte    Sign8       (Byte byte)                         noexcept {return Bit8(byte, 7);}
    SIC Word    BytesToWord (auto bytes)                        noexcept {return (Word(bytes[0]) | (Word(bytes[1]) << 8));}
    SIC Byte    HiByte16    (Word word)                         noexcept {return Byte(word >> 8);}
    SIC Byte    LoByte16    (Word word)                         noexcept {return Byte(word);}
    SIC void    SetHiByte16 (Word&word, Byte byte)              noexcept {word &= Word(0xff); word |= (Word(byte) << 8);}
    SIC void    SetLoByte16 (Word&word, Byte byte)              noexcept {word &= (Word(0xff) << 8); word |= Word(byte);}
    SIC Word    ClrHiByte16 (Word word)                         noexcept {return (word & Word(0xff));}
    SIC Word    ClrLoByte16 (Word word)                         noexcept {return (word & (Word(0xff) << 8));}
};

}
