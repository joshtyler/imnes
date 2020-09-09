//
// Created by josh on 25/07/2020.
//

#ifndef NESEMU_CPU6502_H
#define NESEMU_CPU6502_H


#include <cstdint>
#include <array>

class Cpu6502 {
public:
    // http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
    // https://wiki.nesdev.com/w/index.php/CPU
    // http://www.oxyron.de/html/opcodes02.html
    enum class opcode
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
        opcode code;
        addressing_mode mode;
        uint8_t bytes;
        uint8_t cycles;
        special_duration special;
    };

    // The parameters of an illegal instruction are somewhat arbitrary since we don't support them
    // This is more of a placeholder in case we ever do implement illegal instructions
    static constexpr instruction illegal_instruction{opcode::ILL, addressing_mode::ACCUM, 1, 1, special_duration::NONE};

    // Array can be derived directly from http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf p.10
    static constexpr std::array<instruction,256> instructions =
    {{


             // 0x0X
             {opcode::BRK, addressing_mode::IMPL, 1, 7, special_duration::NONE},
             {opcode::ORA, addressing_mode::INDX, 2, 6, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::ORA, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::ASL, addressing_mode::ZP, 2, 5, special_duration::NONE},
             illegal_instruction,
             {opcode::PHP, addressing_mode::IMPL, 1, 3, special_duration::NONE},
             {opcode::ORA, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::ASL, addressing_mode::ACCUM, 1, 2, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             {opcode::ORA, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::ASL, addressing_mode::ABS, 3, 6, special_duration::NONE},
             illegal_instruction,

             // 0x1X
             {opcode::BPL, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
             {opcode::ORA, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::ORA, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::ASL, addressing_mode::ZPX, 2, 6, special_duration::NONE},
             illegal_instruction,
             {opcode::CLC, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::ORA, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::ORA, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             {opcode::ASL, addressing_mode::ABSX, 3, 7, special_duration::NONE},
             illegal_instruction,

             // 0x2X
             {opcode::JSR, addressing_mode::ABS, 3, 6, special_duration::NONE},
             {opcode::AND, addressing_mode::INDX, 2, 6, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             {opcode::BIT, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::AND, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::ROL, addressing_mode::ZP, 2, 5, special_duration::NONE},
             illegal_instruction,
             {opcode::PLP, addressing_mode::IMPL, 1, 4, special_duration::NONE},
             {opcode::AND, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::ROL, addressing_mode::ACCUM, 1, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::BIT, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::AND, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::ROL, addressing_mode::ABS, 3, 6, special_duration::NONE},
             illegal_instruction,

             // 0x3X
             {opcode::BMI, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
             {opcode::AND, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::AND, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::ROL, addressing_mode::ZPX, 2, 6, special_duration::NONE},
             illegal_instruction,
             {opcode::SEC, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::AND, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::AND, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             {opcode::ROL, addressing_mode::ABSX, 3, 7, special_duration::NONE},
             illegal_instruction,

             // 0x4X
             {opcode::RTI, addressing_mode::IMPL, 1, 6, special_duration::NONE},
             {opcode::EOR, addressing_mode::INDX, 2, 6, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::EOR, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::LSR, addressing_mode::ZP, 2, 5, special_duration::NONE},
             illegal_instruction,
             {opcode::PHA, addressing_mode::IMPL, 1, 3, special_duration::NONE},
             {opcode::EOR, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::LSR, addressing_mode::ACCUM, 1, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::JMP, addressing_mode::ABS, 3, 3, special_duration::NONE},
             {opcode::EOR, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::LSR, addressing_mode::ABS, 3, 6, special_duration::NONE},
             illegal_instruction,

             // 0x5X
             {opcode::BVC, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
             {opcode::EOR, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::EOR, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::LSR, addressing_mode::ZPX, 2, 6, special_duration::NONE},
             illegal_instruction,
             {opcode::CLI, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::EOR, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::EOR, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             {opcode::LSR, addressing_mode::ABSX, 3, 7, special_duration::NONE},
             illegal_instruction,

             // 0x6x
             {opcode::RTS, addressing_mode::IMPL, 1, 6, special_duration::NONE},
             {opcode::ADC, addressing_mode::INDX, 2, 6, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::ADC, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::ROR, addressing_mode::ZP, 2, 5, special_duration::NONE},
             illegal_instruction,
             {opcode::PLA, addressing_mode::IMPL, 1, 4, special_duration::NONE},
             {opcode::ADC, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::ROR, addressing_mode::ACCUM, 1, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::JMP, addressing_mode::IND, 3, 5, special_duration::NONE},
             {opcode::ADC, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::ROR, addressing_mode::ABS, 3, 6, special_duration::NONE},
             illegal_instruction,

             // 0x7x
             {opcode::BVS, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
             {opcode::ADC, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::ADC, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::ROR, addressing_mode::ZPX, 2, 6, special_duration::NONE},
             illegal_instruction,
             {opcode::SEI, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::ADC, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::ADC, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             {opcode::ROR, addressing_mode::ABSX, 3, 7, special_duration::NONE},
             illegal_instruction,

             // 0x8x
             illegal_instruction,
             {opcode::STA, addressing_mode::INDX, 2, 6, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             {opcode::STY, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::STA, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::STX, addressing_mode::ZP, 2, 3, special_duration::NONE},
             illegal_instruction,
             {opcode::DEY, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::TXA, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::STY, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::STA, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::STX, addressing_mode::ABS, 3, 4, special_duration::NONE},
             illegal_instruction,

             // 0x9x
             {opcode::BCC, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
             {opcode::STA, addressing_mode::INDY, 2, 6, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             {opcode::STY, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::STA, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::STX, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             illegal_instruction,
             {opcode::TYA, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::STA, addressing_mode::ABSY, 3, 5, special_duration::NONE},
             {opcode::TXS, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             {opcode::STA, addressing_mode::ABSX, 3, 5, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,

             // 0xAx
             {opcode::LDY, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::LDA, addressing_mode::INDX, 2, 6, special_duration::NONE},
             {opcode::LDX, addressing_mode::IMM, 2, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::LDY, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::LDA, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::LDX, addressing_mode::ZP, 2, 3, special_duration::NONE},
             illegal_instruction,
             {opcode::TAY, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::LDA, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::TAX, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::LDY, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::LDA, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::LDX, addressing_mode::ABS, 3, 4, special_duration::NONE},
             illegal_instruction,

             // 0xBx
             {opcode::BCS, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
             {opcode::LDA, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             {opcode::LDY, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::LDA, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::LDX, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             illegal_instruction,
             {opcode::CLV, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::LDA, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             {opcode::TSX, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::LDY, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             {opcode::LDA, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             {opcode::LDX, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,

             // 0xCx
             {opcode::CPY, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::CMP, addressing_mode::INDX, 2, 6, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             {opcode::CPY, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::CMP, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::DEC, addressing_mode::ZP, 2, 5, special_duration::NONE},
             illegal_instruction,
             {opcode::INY, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::CMP, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::DEX, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::CPY, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::CMP, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::DEC, addressing_mode::ABS, 3, 6, special_duration::NONE},
             illegal_instruction,

             // 0xDx
             {opcode::BNE, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
             {opcode::CMP, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::CMP, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::DEC, addressing_mode::ZPX, 2, 6, special_duration::NONE},
             illegal_instruction,
             {opcode::CLD, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::CMP, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::CMP, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             {opcode::DEC, addressing_mode::ABSX, 3, 7, special_duration::NONE},
             illegal_instruction,

             // 0xEx
             {opcode::CPX, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::SBC, addressing_mode::INDX, 2, 6, special_duration::NONE},
             illegal_instruction,
             illegal_instruction,
             {opcode::CPX, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::SBC, addressing_mode::ZP, 2, 3, special_duration::NONE},
             {opcode::INC, addressing_mode::ZP, 2, 5, special_duration::NONE},
             illegal_instruction,
             {opcode::INX, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::SBC, addressing_mode::IMM, 2, 2, special_duration::NONE},
             {opcode::NOP, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             illegal_instruction,
             {opcode::CPX, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::SBC, addressing_mode::ABS, 3, 4, special_duration::NONE},
             {opcode::INC, addressing_mode::ABS, 3, 6, special_duration::NONE},
             illegal_instruction,

             // 0xFx
             {opcode::BEQ, addressing_mode::REL, 2, 2, special_duration::ADD_ONE_IF_BRANCH_SAME_TWO_IF_BRANCH_DIFF},
             {opcode::SBC, addressing_mode::INDY, 2, 5, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::SBC, addressing_mode::ZPX, 2, 4, special_duration::NONE},
             {opcode::INC, addressing_mode::ZPX, 2, 6, special_duration::NONE},
             illegal_instruction,
             {opcode::SED, addressing_mode::IMPL, 1, 2, special_duration::NONE},
             {opcode::SBC, addressing_mode::ABSY, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             illegal_instruction,
             illegal_instruction,
             illegal_instruction,
             {opcode::SBC, addressing_mode::ABSX, 3, 4, special_duration::ADD_ONE_IF_CROSS},
             {opcode::INC, addressing_mode::ABSX, 3, 7, special_duration::NONE},
             illegal_instruction
     }};

};


#endif //NESEMU_CPU6502_H
