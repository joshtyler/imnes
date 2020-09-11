#include <iostream>
#include <istream>
#include <fstream>
#include <vector>

#include "Cpu6502_instructions.h"
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

    std::cout << prog.size() << std::endl;

    for(size_t i=0x400u; i<prog.size(); )
    {
        auto instr = instructions[prog[i]];

        uint16_t data{0};
        for(auto j=1u; j< instr.bytes; j++)
        {
            data << 8;
            data |= prog[i+j];
        }

        std::cout << magic_enum::enum_name(instr.code) << " ";
        print_operand(instr, data, std::cout);
        std::cout << '\n';

        if(instr.code == operation::ILL)
            break;

        i+= instr.bytes;
    }



    return 0;
}
