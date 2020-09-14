#include <iostream>
#include <istream>
#include <fstream>
#include <vector>

#include <fmt/core.h>

#include "Cpu6502_instructions.h"

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

    fmt::print("Hello from fmt\n");

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

        fmt::print("(0x{:02X} 0x{:04X}) {} \n",prog[i], data, disassemble_instruction(instr, data));

        if(instr.code == operation::ILL)
            break;

        i+= instr.bytes;
    }



    return 0;
}
