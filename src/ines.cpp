//
// Created by josh9 on 24/09/2020.
//

#include <fstream>
#include <bitset>
#include "ines.h"

Ines::Ines(const std::filesystem::path &p) {
    if(!std::filesystem::exists(p))
    {
        throw std::runtime_error("File does not exist");
    }


    std::ifstream file(p.string(), std::ios::binary);
    if(!file)
    {
        throw std::runtime_error("Could not open file");
    }
    // Throw if anything bad happens to save us having to check if file is okay all the time
    file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

    // I don't think we really need to keep track of file size, it seems quite error prone
    // It is easier to just read what we want and let the file throw if we try and read too much?
    //auto remaining_file_size = std::filesystem::file_size(p);
    //if(remaining_file_size < 16)
    //{
    //    throw std::runtime_error("File is not large enough for iNES header");
    //}

    // Format http://wiki.nesdev.com/w/index.php/INES#Trainer

    // Check iNES identififier
    {
        constexpr size_t ident_size = 4;
        char ident_buf[ident_size];
        file.read(ident_buf, ident_size);
        if (std::string(ident_buf, ident_size) != "NES\x1A")
        {
            throw std::runtime_error("iNES identifier is not correct");
        }
    }

    // Read byte 4
    uint8_t prg_rom_size_16k_pages;
    file.read(reinterpret_cast<char *>(&prg_rom_size_16k_pages), 1);

    // Read byte 5
    uint8_t chr_rom_size_8k_pages;
    file.read(reinterpret_cast<char *>(&chr_rom_size_8k_pages), 1);

    // Read Flags 6
    bool trainer_present;
    {
        uint8_t flags;
        file.read(reinterpret_cast<char *>(&flags), 1);

        std::bitset<8> flag_bits(flags);
        mirroring = flag_bits[0] ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;
        // TODO: Implement battery backed RAM support
        trainer_present = flag_bits[2];
        if(flag_bits[3])
        {
            mirroring = Mirroring::FOUR_SCREEN;
        }
        mapper_num = static_cast<uint8_t>((flag_bits >> 4).to_ulong());

    }

    // Read Flags 7
    {
        uint8_t flags;
        file.read(reinterpret_cast<char *>(&flags), 1);

        std::bitset<8> flag_bits(flags);
        // TODO: Implement unisystem
        // TODO: Implement playchoice 10
        // TODO: Implement nes 2.0
        // TODO: Implement support for upper bits of mapper
        // N.B. need to check that padding is zero || nes 2.0 before trusting upper bits of mapper
        // See note on http://wiki.nesdev.com/w/index.php/INES#Flags_10
    }

    // Read Flags 8
    {
        uint8_t flags;
        file.read(reinterpret_cast<char *>(&flags), 1);
        // TODO: Implement flags
    }

    // Read Flags 9
    {
        uint8_t flags;
        file.read(reinterpret_cast<char *>(&flags), 1);
        // TODO: Implement flags

    }

    // Read Flags 10
    {
        uint8_t flags;
        file.read(reinterpret_cast<char *>(&flags), 1);
        // TODO: Implement flags
    }

    // Skip padding (bytes 11 to 15)
    {
        constexpr size_t padding_size = 5;
        file.seekg(padding_size, std::ios_base::cur);
    }

    // Discard the trainer if present(it is a legacy thing to help old emulators)
    if(trainer_present)
    {
        constexpr size_t trainer_size = 512;
        file.seekg(trainer_size, std::ios_base::cur);
    }

    // Next in the image is the PRG ROM
    {
        const size_t size_bytes = prg_rom_size_16k_pages * 16 * 1024;
        prg_rom.resize(size_bytes);
        file.read(reinterpret_cast<char *>(prg_rom.data()), static_cast<std::streamsize>(size_bytes));
    }

    // Next in the image is the CHR ROM
    {
        const size_t size_bytes = chr_rom_size_8k_pages * 8 * 1024;
        prg_rom.resize(size_bytes);
        file.read(reinterpret_cast<char *>(prg_rom.data()), static_cast<std::streamsize>(size_bytes));
    }

    // TODO: Implement support for the play choice PROMs etc.
}
