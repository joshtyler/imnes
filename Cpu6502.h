//
// Created by josh on 25/07/2020.
//

#ifndef NESEMU_CPU6502_H
#define NESEMU_CPU6502_H

#include <cstdint>

class Cpu6502 {
public:


    // Resisters
    // https://wiki.nesdev.com/w/index.php/CPU_registers
    uint8_t a,x,y,s,p;
    uint16_t pc;


};


#endif //NESEMU_CPU6502_H
