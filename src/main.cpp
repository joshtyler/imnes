#include <iostream>
#include <istream>
#include <fstream>
#include <vector>

#include "Cpu6502_instructions.h"
#include "magic_enum.hpp"

size_t getSize(const std::string &filename)
{
    std::ifstream is(filename, std::ios::binary);
    if(!is)
        return 0;
    is.seekg(0, std::ios::end);
    auto size = is.tellg();
    return size<0? 0 : static_cast<size_t>(size);
}

std::vector<uint8_t> readToVector(const std::string &filename)
{
    std::vector<uint8_t> dat;
    size_t filesize = getSize(filename);
    if(filesize > 0) {
        std::ifstream is(filename, std::ios::binary);
        dat.resize(filesize);
        is.read(reinterpret_cast<char *>(dat.data()), static_cast<std::streamsize>(filesize));
    }
    return dat;
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto prog = readToVector("6502_functional_test.bin");

    std::cout << prog.size() << std::endl;

    for(size_t i=0x400; i<prog.size(); )
    {
        auto instr = instructions[prog[i]];

        uint16_t data{0};
        for(size_t j=1; j< instr.bytes; j++)
        {
            data <<= 8u;
            data |= static_cast<uint16_t>(prog[i+j]);
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
