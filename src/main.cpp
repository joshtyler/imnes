#include <iostream>
#include <istream>
#include <fstream>
#include <vector>

#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include <fmt/core.h>

#include <imgui_memory_editor.h>
#include "disassembly_view.h"

#include "Cpu6502_instructions.h"
#include "ines.h"

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

    if(prog.size() == 0)
    {
        std::cerr << "Could not open file\n";
        exit(1);
    }

    std::cout << prog.size() << std::endl;

    /*
    for(size_t i=0x400; i<prog.size(); )
    {
        auto instr = instructions[prog[i]];

        uint16_t data{0};
        for(size_t j=1; j< instr.bytes; j++)
        {
            data <<= 8u;
            data |= static_cast<uint16_t>(prog[i+j]);
        }

        fmt::print("(0x{:02X} 0x{:04X}) {} \n", prog[i], data, disassemble_instruction(instr, data));

        if(instr.code == operation::ILL)
            break;

        i+= instr.bytes;
    }
     */

    Ines ines("test_image.nes");

    // imGUI SFML Example
    sf::RenderWindow window(sf::VideoMode(1900, 1100), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        //ImGui::Begin("Test");
        ImGui::ShowDemoWindow();
        //ImGui::End();

        static MemoryEditor mem_edit_1;
        mem_edit_1.DrawWindow("Memory Editor", ines.getPrgRom().data(), ines.getPrgRom().size(), 0x0000);

        static disassembly_view disasm_view;
        disasm_view.DrawWindow("Disassembly view", ines.getPrgRom().data(), ines.getPrgRom().size(), 0x0000);


        window.clear();
        window.draw(shape);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
