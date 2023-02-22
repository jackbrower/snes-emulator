#include "cartridge.hpp"

#include<string>
#include<fstream>
#include<vector>
#include<variant>

namespace sneslite
{
    cartridge::cartridge() :
        MAPPER_NUMBER(0),
        EXTENDED_RAM(0),
        MIRROR_TYPE(FOUR_SCREEN)
    {}

    bool cartridge::load_dump_file(std::string path)
    {
        // Attempt to open file
        std::ifstream rom_dump(path, std::ios_base::binary | std::ios_base::in);
        if(!rom_dump)
        {
            std::cerr << "Unable to open ROM dump from path: " << path << std::endl;
            return false;
        }

        // Check if dump has header
        std::vector<uint8_t> HEADER;
        std::cout << "Reading ROM dump from path: " << path << std::endl;
    
        // Standard iNES file has a HEADER of 16 bytes
        HEADER.resize(0x10);
        if (!rom_dump.read(reinterpret_cast<char*>(&HEADER[0]), 0x10))
        {
            std::cerr << "Unable to read iNES header." << std::endl;
            return false;
        }

        if(std::string{&HEADER[0], &HEADER[4]} != NES_HEADER)
        {
            std::cerr << "File is not in a valid iNES format." << std::endl;
            return false;
        }

        std::cout << "Reading HEADER:" << std::endl;

        uint8_t file_prg_rom = HEADER[4];
        if(!file_prg_rom)
        {
            std::cerr << "ROM has no program ROM. Loading failed." << std::endl;
            return false;
        }
        std::cout << "16KB PRG_ROM: " << +file_prg_rom << std::endl;

        uint8_t file_char_rom = HEADER[5];
        if(!file_char_rom)
        {
            std::cerr << "ROM has no character ROM. Loading failed." << std::endl;
            return false;
        }
        std::cout << "8KB CHAR_ROM: " << +file_char_rom << std::endl;

        /*
            Check mirror type given byte in header
            Byte 6, ROM control byte 1, maps memory as such:

            7 - mapper lower bit
            6 - mapper lower bit
            5 - mapper lower bit
            4 - mapper lower bit
            3 - four-screen bit
            2 - 512-byte trainer bit
            1 - battery-backed RAM identifier
            0 - vertical mirroring bit

            If both bit 3 and bit 0 are false, then it implies horizontal mirroring.
        */
        std::cout << "ROM Mirror Type: ";
        if(HEADER[6] & 0x8)
        {
            MIRROR_TYPE = FOUR_SCREEN;
            std::cout << "FOUR SCREEN" << std::endl;
        }
        else
        {
            MIRROR_TYPE = (mirroring)(HEADER[6] & 0x1);
            std::cout << (MIRROR_TYPE == 0 ? "Horizontal" : "Vertical") << std::endl;
        }

        /*
            Check mapper number given byte 6 and byte 7
            Byte 7 maps memory as such:

            7 - mapper upper bit
            6 - mapper upper bit
            5 - mapper upper bit
            4 - mapper upper bit
            3 - iNES format identifier
            2 - iNES format identifier
            1 - iNES format identifier
            0 - iNES format identifier

            The four upper and lower bits in byte 6 and 7 are used to identify the mapper type.
            These are also used to identify if the cartridge used extended RAM banks.
        */
        MAPPER_NUMBER = ((HEADER[6] >> 4) & 0xf) | (HEADER[7] & 0xf0);
        std::cout << "ROM Mapper Number: " << +MAPPER_NUMBER << std::endl;

        EXTENDED_RAM = HEADER[6] & 0x2;
        std::cout << "ROM Extended Ram: " << EXTENDED_RAM << std::endl;

        // Some carts have trainers, attempt to disregard this.
        // TODO: See about trainer integration
        if(HEADER[6] & 0x4)
        {
            std::cerr << "ROM uses trainer, attempting to bypass." << std::endl;
            PRG_ROM_START = 16 + 512;
        }
        else
        {
            PRG_ROM_START = 16;
        }

        // TODO: Add PAL compatability
        if((HEADER[0xA] & 0x3) == 0x2 || (HEADER[0xA] & 0x1))
        {
            std::cerr << "PAL is not supported." << std::endl;
            return false;
        }

        PRG_ROM.resize(0x4000 * file_prg_rom);
        if (!rom_dump.read(reinterpret_cast<char*>(&PRG_ROM[0]), 0x4000 * file_prg_rom))
        {
            std::cerr << "Unable to read PRG_ROM." << std::endl;
            return false;
        }

        CHAR_ROM.resize(0x2000 * file_char_rom);
        if (!rom_dump.read(reinterpret_cast<char*>(&CHAR_ROM[0]), 0x2000 * file_char_rom))
        {
            std::cerr << "Unable to read CHAR_ROM." << std::endl;
            return false;
        }
    }

    const std::vector<uint8_t>& cartridge::get_prg_rom()
    {
        return PRG_ROM;
    }

    const uint8_t cartridge::get_prg_rom_start()
    {
        return PRG_ROM_START;
    }

    const std::vector<uint8_t>& cartridge::get_char_rom()
    {
        return CHAR_ROM;
    }

    const uint8_t cartridge::get_mapper_number()
    {
        return MAPPER_NUMBER;
    }

    const uint8_t cartridge::get_mirror_type()
    {
        return static_cast<uint8_t>(MIRROR_TYPE);
    }
}