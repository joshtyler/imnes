//
// Created by josh on 09/09/2020.
//

#ifndef NESEMU_CPU6502_INSTRUCTIONS_H
#define NESEMU_CPU6502_INSTRUCTIONS_H

//https://en.wikibooks.org/wiki/6502_Assembly
//http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
//http://archive.6502.org/datasheets/rockwell_r65c00_microprocessors.pdf
//https://wiki.nesdev.com/w/index.php/CPU
//https://github.com/OneLoneCoder/olcNES/blob/master/Part%232%20-%20CPU/olc6502.cpp
//http://www.thealmightyguru.com/Games/Hacking/Wiki/index.php/Addressing_Modes

#include <cstdint>
#include <array>
#include <ios>
#include <iomanip>

// http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
// https://wiki.nesdev.com/w/index.php/CPU
// http://www.oxyron.de/html/opcodes02.html
enum class operation
{
    ADC,
    AND,
    ASL,
    BCC,
    BCS,
    BEQ,
    BIT,
    BMI,
    BNE,
    BPL,
    BRK,
    BVC,
    BVS,
    CLC,
    CLD,
    CLI,
    CLV,
    CMP,
    CPX,
    CPY,
    DEC,
    DEX,
    DEY,
    EOR,
    INC,
    INX,
    INY,
    JMP,
    JSR,
    LDA,
    LDX,
    LDY,
    LSR,
    NOP,
    ORA,
    PHA,
    PHP,
    PLA,
    PLP,
    ROL,
    ROR,
    RTI,
    RTS,
    SBC,
    SEC,
    SED,
    SEI,
    STA,
    STX,
    STY,
    TAX,
    TAY,
    TSX,
    TXA,
    TXS,
    TYA,
    ILL, // Denote illegal instruction. We currently don't implement
};

enum class addressing_mode
{
    ACCUM, //Accumulator addressing
    IMM, // Immediate address
    ABS, // Absolute addressing
    ZP, // Zero page addressing
    ZPX, // Indexed zero page addressing X
    ZPY, // Indexed zero page addressing Y
    ABSX, // Indexed absolute addressing X
    ABSY, // Indexed absolute addressing Y
    IMPL, // Implied addressing
    REL, // Relative addressing
    INDX, // Indexed direct addressing X
    INDY, // Indexed direct addressing Y
    IND, // Absolute indirect
};



enum class special_duration
{
    NONE,
    ADD_ONE_IF_CROSS,
    ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF
};

struct instruction
{
    operation code;
    addressing_mode mode;
    uint8_t bytes;
    uint8_t cycles;
    special_duration special;
};

void print_operand(instruction instr, uint16_t operand, std::ostream & os)
{
    if(instr.mode == addressing_mode::ACCUM || instr.mode == addressing_mode::IMPL)
    {
        // No operand to print
        return;
    }

    if(instr.mode == addressing_mode::IMM)
    {
        // '#' before operand indicates an immediate value, not an address
        os << '#';
    }

    if(instr.mode == addressing_mode::INDX || instr.mode == addressing_mode::INDY || instr.mode == addressing_mode::IND)
    {
        // Indirect addressing is in brackets
        os << '(';
    }

    os << '$'; // Hex

    os << std::hex;

    if(  instr.mode == addressing_mode::IMM ||
            instr.mode == addressing_mode::ZP ||
    instr.mode == addressing_mode::ZPX ||
    instr.mode == addressing_mode::ZPY ||
    instr.mode == addressing_mode::REL ||
    instr.mode == addressing_mode::INDX ||
    instr.mode == addressing_mode::INDY)
    {
        os << std::setw(2) << std::setfill('0') << (operand & 0xFF);
    } else {
        os << std::setw(4) << std::setfill('0') << operand;
    }

    #warning "Hacky. Should preserve properly"
    os << std::dec;

    if( instr.mode == addressing_mode::ZPX ||
        instr.mode == addressing_mode::INDX )
    {
        os << ",X";
    }

    if( instr.mode == addressing_mode::ZPY ||
        instr.mode == addressing_mode::INDY )
    {
        os << ",Y";
    }

    if(instr.mode == addressing_mode::INDX || instr.mode == addressing_mode::INDY || instr.mode == addressing_mode::IND)
    {
        // Indirect addressing is in brackets
        os << ")";
    }
}

// The parameters of an illegal instruction are somewhat arbitrary since we don't support them
// This is more of a placeholder in case we ever do implement illegal instructions
static constexpr instruction illegal_instruction{operation::ILL, addressing_mode::IMPL, 1, 1, special_duration::NONE};

// Array can be derived directly from http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf p.10
static constexpr std::array<instruction,256> instructions =
        {{


                 // 0x0X
                 {operation::BRK, addressing_mode::IMPL, 1, 7, special_duration::NONE},
                 {operation::ORA, addressing_mode::INDX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::ORA, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::ASL, addressing_mode::ZP, 2, 5, special_duration::NONE},
                 illegal_instruction,
                 {operation::PHP, addressing_mode::IMPL, 1, 3, special_duration::NONE},
                 {operation::ORA, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::ASL, addressing_mode::ACCUM, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 {operation::ORA, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::ASL, addressing_mode::ABS, 3, 6, special_duration::NONE},
                 illegal_instruction,

                 // 0x1X
                 {operation::BPL, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
                 {operation::ORA, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::ORA, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::ASL, addressing_mode::ZPX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 {operation::CLC, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::ORA, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::ORA, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 {operation::ASL, addressing_mode::ABSX, 3, 7, special_duration::NONE},
                 illegal_instruction,

                 // 0x2X
                 {operation::JSR, addressing_mode::ABS, 3, 6, special_duration::NONE},
                 {operation::AND, addressing_mode::INDX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 {operation::BIT, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::AND, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::ROL, addressing_mode::ZP, 2, 5, special_duration::NONE},
                 illegal_instruction,
                 {operation::PLP, addressing_mode::IMPL, 1, 4, special_duration::NONE},
                 {operation::AND, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::ROL, addressing_mode::ACCUM, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::BIT, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::AND, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::ROL, addressing_mode::ABS, 3, 6, special_duration::NONE},
                 illegal_instruction,

                 // 0x3X
                 {operation::BMI, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
                 {operation::AND, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::AND, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::ROL, addressing_mode::ZPX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 {operation::SEC, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::AND, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::AND, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 {operation::ROL, addressing_mode::ABSX, 3, 7, special_duration::NONE},
                 illegal_instruction,

                 // 0x4X
                 {operation::RTI, addressing_mode::IMPL, 1, 6, special_duration::NONE},
                 {operation::EOR, addressing_mode::INDX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::EOR, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::LSR, addressing_mode::ZP, 2, 5, special_duration::NONE},
                 illegal_instruction,
                 {operation::PHA, addressing_mode::IMPL, 1, 3, special_duration::NONE},
                 {operation::EOR, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::LSR, addressing_mode::ACCUM, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::JMP, addressing_mode::ABS, 3, 3, special_duration::NONE},
                 {operation::EOR, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::LSR, addressing_mode::ABS, 3, 6, special_duration::NONE},
                 illegal_instruction,

                 // 0x5X
                 {operation::BVC, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
                 {operation::EOR, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::EOR, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::LSR, addressing_mode::ZPX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 {operation::CLI, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::EOR, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::EOR, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 {operation::LSR, addressing_mode::ABSX, 3, 7, special_duration::NONE},
                 illegal_instruction,

                 // 0x6x
                 {operation::RTS, addressing_mode::IMPL, 1, 6, special_duration::NONE},
                 {operation::ADC, addressing_mode::INDX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::ADC, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::ROR, addressing_mode::ZP, 2, 5, special_duration::NONE},
                 illegal_instruction,
                 {operation::PLA, addressing_mode::IMPL, 1, 4, special_duration::NONE},
                 {operation::ADC, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::ROR, addressing_mode::ACCUM, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::JMP, addressing_mode::IND, 3, 5, special_duration::NONE},
                 {operation::ADC, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::ROR, addressing_mode::ABS, 3, 6, special_duration::NONE},
                 illegal_instruction,

                 // 0x7x
                 {operation::BVS, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
                 {operation::ADC, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::ADC, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::ROR, addressing_mode::ZPX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 {operation::SEI, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::ADC, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::ADC, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 {operation::ROR, addressing_mode::ABSX, 3, 7, special_duration::NONE},
                 illegal_instruction,

                 // 0x8x
                 illegal_instruction,
                 {operation::STA, addressing_mode::INDX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 {operation::STY, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::STA, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::STX, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 illegal_instruction,
                 {operation::DEY, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::TXA, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::STY, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::STA, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::STX, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 illegal_instruction,

                 // 0x9x
                 {operation::BCC, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
                 {operation::STA, addressing_mode::INDY, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 {operation::STY, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::STA, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::STX, addressing_mode::ZPY, 2, 4, special_duration::NONE},
                 illegal_instruction,
                 {operation::TYA, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::STA, addressing_mode::ABSY, 3, 5, special_duration::NONE},
                 {operation::TXS, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 {operation::STA, addressing_mode::ABSX, 3, 5, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,

                 // 0xAx
                 {operation::LDY, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::LDA, addressing_mode::INDX, 2, 6, special_duration::NONE},
                 {operation::LDX, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::LDY, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::LDA, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::LDX, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 illegal_instruction,
                 {operation::TAY, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::LDA, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::TAX, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::LDY, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::LDA, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::LDX, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 illegal_instruction,

                 // 0xBx
                 {operation::BCS, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
                 {operation::LDA, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 {operation::LDY, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::LDA, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::LDX, addressing_mode::ZPY, 2, 4, special_duration::NONE},
                 illegal_instruction,
                 {operation::CLV, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::LDA, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 {operation::TSX, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::LDY, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 {operation::LDA, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 {operation::LDX, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,

                 // 0xCx
                 {operation::CPY, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::CMP, addressing_mode::INDX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 {operation::CPY, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::CMP, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::DEC, addressing_mode::ZP, 2, 5, special_duration::NONE},
                 illegal_instruction,
                 {operation::INY, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::CMP, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::DEX, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::CPY, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::CMP, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::DEC, addressing_mode::ABS, 3, 6, special_duration::NONE},
                 illegal_instruction,

                 // 0xDx
                 {operation::BNE, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
                 {operation::CMP, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::CMP, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::DEC, addressing_mode::ZPX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 {operation::CLD, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::CMP, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::CMP, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 {operation::DEC, addressing_mode::ABSX, 3, 7, special_duration::NONE},
                 illegal_instruction,

                 // 0xEx
                 {operation::CPX, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::SBC, addressing_mode::INDX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 illegal_instruction,
                 {operation::CPX, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::SBC, addressing_mode::ZP, 2, 3, special_duration::NONE},
                 {operation::INC, addressing_mode::ZP, 2, 5, special_duration::NONE},
                 illegal_instruction,
                 {operation::INX, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::SBC, addressing_mode::IMM, 2, 2, special_duration::NONE},
                 {operation::NOP, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 illegal_instruction,
                 {operation::CPX, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::SBC, addressing_mode::ABS, 3, 4, special_duration::NONE},
                 {operation::INC, addressing_mode::ABS, 3, 6, special_duration::NONE},
                 illegal_instruction,

                 // 0xFx
                 {operation::BEQ, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
                 {operation::SBC, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::SBC, addressing_mode::ZPX, 2, 4, special_duration::NONE},
                 {operation::INC, addressing_mode::ZPX, 2, 6, special_duration::NONE},
                 illegal_instruction,
                 {operation::SED, addressing_mode::IMPL, 1, 2, special_duration::NONE},
                 {operation::SBC, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 illegal_instruction,
                 illegal_instruction,
                 illegal_instruction,
                 {operation::SBC, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
                 {operation::INC, addressing_mode::ABSX, 3, 7, special_duration::NONE},
                 illegal_instruction
         }};


#endif //NESEMU_CPU6502_INSTRUCTIONS_H
