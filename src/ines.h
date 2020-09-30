//
// Created by josh9 on 24/09/2020.
//

#ifndef IMNES_INES_H
#define IMNES_INES_H


#include <filesystem>
#include <vector>

class Ines {
public:
        // N.B. constructor may throw runtime_error
        explicit Ines(const std::filesystem::path& p);

        enum class Mirroring
        {
            HORIZONTAL,
            VERTICAL,
            FOUR_SCREEN
        };

    std::vector<uint8_t> &getPrgRom() {
        return prg_rom;
    }

    std::vector<uint8_t> &getChrRom() {
        return chr_rom;
    }

private:
    Mirroring mirroring;
    uint8_t mapper_num;
    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
};


#endif //IMNES_INES_H
