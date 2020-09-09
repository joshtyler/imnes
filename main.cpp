#include <iostream>
#include <istream>
#include <fstream>
#include <vector>

#include "Cpu6502.h"
#include "magic_enum.hpp"

size_t getSize(std::string filename)
{
    std::ifstream is(filename, std::ios::binary);
    if(!is)
        return 0;
    is.seekg(0, std::ios::end);
    return is.tellg();
}

std::vector<uint8_t> readToVector(std::string filename)
{
    std::vector<uint8_t> dat;
    size_t filesize = getSize(filename);
    if(filesize > 0) {
        std::ifstream is(filename, std::ios::binary);
        dat.resize(filesize);
        is.read((char *) dat.data(), filesize);
    }
    return dat;
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto prog = readToVector("6502_functional_test.bin");

    for(size_t i=0x400u; i<prog.size(); )
    {
        auto instr = Cpu6502::instructions[prog[i]];
        std::cout << magic_enum::enum_name(instr.code) << " (hex 0x" << std::hex << static_cast<unsigned int>(prog[i]) <<", " << std::dec << static_cast<unsigned int>(instr.bytes) << " bytes)\n";

        if(instr.code == Cpu6502::opcode::ILL)
            break;

        i+= instr.bytes;
    }



    return 0;
}
