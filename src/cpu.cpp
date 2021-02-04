//
//  Chip-8
//
//  Created by Cosme Jordan on 12.09.20.
//  Copyright © 2020 Cosme Jordan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <iomanip>

#include <SDL.h>
#include <SDL_audio.h>

#include "cpu.hpp"
#include "sound.hpp"

// #define DEBUGGING

#ifdef DEBUGGING
#define PRINT_DEBUG(a) std::cerr << a << std::endl;
#else
#define PRINT_DEBUG(a)
#endif

#define VIDEO_HEIGHT 32
#define VIDEO_WIDTH 64

#include <chrono>
#include <random>

CPU::CPU(): randomEngine(std::chrono::system_clock::now().time_since_epoch().count()) {
    distribution = std::uniform_int_distribution<uint8_t>(0, 255);
    
    // FIXME: TODO: Transfer that to file and then to the graphics itself
    const uint8_t GRAPHICS[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80
    };
    
    pc = STARTING_ADDRESS;
    
    memset(registers, INIT_VALUE, sizeof(registers));
    memset(ram, INIT_VALUE, sizeof(ram));
    memset(stack, INIT_VALUE, sizeof(stack));
    memset(keyboard, INIT_VALUE, sizeof(keyboard));
    memset(screen, INIT_VALUE, sizeof(screen));
    
    memcpy(&ram[STARTING_ADDRESS_FONTSET], GRAPHICS, NUMBER_FONTSETS);

    // table = new OpcodeFunction[SIZE_TABLE];
    // table0x0 = new OpcodeFunction[SIZE_TABLE0x0];
    // table0x8 = new OpcodeFunction[SIZE_TABLE0x8];
    // table0xE = new OpcodeFunction[SIZE_TABLE0xE];
    // table0xF = new OpcodeFunction[SIZE_TABLE0xF];

    initNopes();
    initOpcodeTables();

    SDL_Init(SDL_INIT_AUDIO);

}

CPU::~CPU() {
    //    delete[] table;
    //    delete[] table0x0;
    //    delete[] table0x8;
    //    delete[] table0xE;
    //    delete[] table0xF;
}

void CPU::loadROM(const char* filename) {
    std::ifstream rom(filename, std::ios::binary | std::ios::ate);

    if(rom.is_open()) {
        // FIXME: TODO: TODO Refactor
        std::streampos tempSize = rom.tellg();
        
        char* temp = new char[tempSize];
        rom.seekg(0, std::ios::beg);
        rom.read(temp, tempSize);
        rom.close();
        
        memcpy(&ram[STARTING_ADDRESS], temp, tempSize*sizeof(char));
        
        delete[] temp;
    } else {
        throw std::runtime_error("ROM Doesn't Exist !");
    }
}

void CPU::initNopes() {
    std::fill_n(table, SIZE_TABLE, &CPU::opcodeNOPE);
    std::fill_n(table0x0, SIZE_TABLE0x0, &CPU::opcodeNOPE);

    /// Strange bug: 0xE+0x1: Invalid suffix '+0x1' on integer constant,
    /// but with 0xF+0x1, it works...
    std::fill_n(table0x8, SIZE_TABLE0x8, &CPU::opcodeNOPE);
    std::fill_n(table0xE, SIZE_TABLE0xE, &CPU::opcodeNOPE);
    std::fill_n(table0xF, SIZE_TABLE0xF, &CPU::opcodeNOPE);
}

void CPU::initOpcodeTables() {
    table[0x0] = &CPU::accessTable0x0;
    table[0x1] = &CPU::opcode1nnn;
    table[0x2] = &CPU::opcode2nnn;
    table[0x3] = &CPU::opcode3xkk;
    table[0x4] = &CPU::opcode4xkk;
    table[0x5] = &CPU::opcode5xy0;
    table[0x6] = &CPU::opcode6xkk;
    table[0x7] = &CPU::opcode7xkk;
    table[0x8] = &CPU::accessTable0x8;
    table[0x9] = &CPU::opcode9xy0;
    table[0xA] = &CPU::opcodeAnnn;
    table[0xB] = &CPU::opcodeBnnn;
    table[0xC] = &CPU::opcodeCxkk;
    table[0xD] = &CPU::opcodeDxyn;
    table[0xE] = &CPU::accessTable0xE;
    table[0xF] = &CPU::accessTable0xF;
    
    table0x0[0x0] = &CPU::opcode00E0;
    table0x0[0xE] = &CPU::opcode00EE;
    
    table0x8[0x0] = &CPU::opcode8xy0;
    table0x8[0x1] = &CPU::opcode8xy1;
    table0x8[0x2] = &CPU::opcode8xy2;
    table0x8[0x3] = &CPU::opcode8xy3;
    table0x8[0x4] = &CPU::opcode8xy4;
    table0x8[0x5] = &CPU::opcode8xy5;
    table0x8[0x6] = &CPU::opcode8xy6;
    table0x8[0x7] = &CPU::opcode8xy7;
    table0x8[0xE] = &CPU::opcode8xyE;
    
    table0xE[0x1] = &CPU::opcodeExA1;
    table0xE[0xE] = &CPU::opcodeEx9E;
    
    table0xF[0x07] = &CPU::opcodeFx07;
    table0xF[0x0A] = &CPU::opcodeFx0A;
    table0xF[0x15] = &CPU::opcodeFx15;
    table0xF[0x18] = &CPU::opcodeFx18;
    table0xF[0x1E] = &CPU::opcodeFx1E;
    table0xF[0x29] = &CPU::opcodeFx29;
    table0xF[0x33] = &CPU::opcodeFx33;
    table0xF[0x55] = &CPU::opcodeFx55;
    table0xF[0x65] = &CPU::opcodeFx65;
}

// =============================================================================
// =============================================================================
// =============================================================================
// Functions for accessing the different tables

void CPU::accessTable0x0() {
    (this->*table0x0[get0xFValue()])();
}

void CPU::accessTable0x8() {
    (this->*table0x8[get0xFValue()])();
}

void CPU::accessTable0xE() {
    (this->*table0xE[get0xFValue()])();
}

void CPU::accessTable0xF() {
    size_t v = get0xFFValue();
    (this->*table0xF[v])();
}

// =============================================================================
// =============================================================================
// =============================================================================
// Utility Functions

size_t CPU::get0xFValue() {
    return opcode & 0x0000Fu;
}

size_t CPU::get0xFFValue() {
    return opcode & 0x000FFu;
}

uint8_t CPU::x() {
    return (opcode >> 8) & 0xFu;
}

uint8_t CPU::y() {
    return (opcode >> 4) & 0xFu;
}

uint8_t CPU::kk() {
    return opcode & 0x00FFu;
}

uint8_t CPU::n() {
    return opcode & 0x000Fu;
}

uint16_t CPU::nnn() {
    return opcode & 0x0FFFu;
}

// =============================================================================
// =============================================================================
// =============================================================================
// Opcodes Functions

void CPU::opcodeNOPE() {
    PRINT_DEBUG("opcode NOPE");
}

void CPU::opcode0nnn() {
    PRINT_DEBUG("opcode 0nnn");
    
    // This opcode is only used on old computers
}

void CPU::opcode00E0() {
    PRINT_DEBUG("opcode 00E0");
    
    std::fill_n(screen, 64 * 32, 0);
}

void CPU::opcode00EE() {
    PRINT_DEBUG("opcode 00EE");
    
    pc = stack[--sp];
}

void CPU::opcode1nnn() {
    PRINT_DEBUG("opcode 1nnn");
    
    pc = nnn();
}

void CPU::opcode2nnn() {
    PRINT_DEBUG("opcode 2nnn");
    
    stack[sp++] = pc;
    pc = nnn();
}

void CPU::opcode3xkk() {
    PRINT_DEBUG("opcode 3xkk");
    
    if (registers[x()] == kk()) {
        pc += 2;
    }
}

void CPU::opcode4xkk() {
    PRINT_DEBUG("opcode 4xkk");
    
    if (registers[x()] != kk()) {
        pc += 2;
    }
}

void CPU::opcode5xy0() {
    PRINT_DEBUG("opcode 5xy0");
    
    if (registers[x()] == registers[y()]) {
        pc += 2;
    }
}

void CPU::opcode6xkk() {
    PRINT_DEBUG("opcode 6xkk");
    
    registers[x()] = kk();
}

void CPU::opcode7xkk() {
    PRINT_DEBUG("opcode 7xkk");
    
    registers[x()] += kk();
}

void CPU::opcode8xy0() {
    PRINT_DEBUG("opcode 8xy0");
    
    registers[x()] = registers[y()];
}

void CPU::opcode8xy1() {
    PRINT_DEBUG("opcode 8xy1");
    
    registers[x()] |= registers[y()];
}

void CPU::opcode8xy2() {
    PRINT_DEBUG("opcode 8xy2");
    
    registers[x()] &= registers[y()];
}

void CPU::opcode8xy3() {
    PRINT_DEBUG("opcode 8xy3");
    
    registers[x()] ^= registers[y()];
}

void CPU::opcode8xy4() {
    PRINT_DEBUG("opcode 8xy4");
    
    uint16_t sum = registers[x()] + registers[y()];
    registers[0xF] = sum > 0x0FFu ? 1 : 0;
    registers[x()] = sum & 0x00FFu;
}

void CPU::opcode8xy5() {
    PRINT_DEBUG("opcode 8xy5");
    
    uint8_t Vx = x();
    uint8_t Vy = y();
    
    registers[0xF] = registers[Vx] > registers[Vy] ? 1 : 0;
    registers[Vx] -= registers[Vy];
}

void CPU::opcode8xy6() {
    PRINT_DEBUG("opcode 8xy6");
    
    registers[0xF] = registers[x()] & 0x1;
    
    // Division by 2
    registers[x()] >>= 1;
}

void CPU::opcode8xy7() {
    PRINT_DEBUG("opcode 8xy7");
    
    uint8_t Vx = x();
    uint8_t Vy = y();
    
    registers[0xF] = registers[Vy] > registers[Vx] ? 1 : 0;
    registers[Vx] = Vy - Vx;
}

void CPU::opcode8xyE() {
    PRINT_DEBUG("opcode 8xyE");
    
    uint8_t Vx = x();
    
    registers[0xF] = (registers[Vx] & 0x80) >> 7;
    registers[Vx] <<= 1;
}

void CPU::opcode9xy0() {
    PRINT_DEBUG("opcode 9xy0");
    
    if(registers[x()] != registers[y()]) {
        pc += 2;
    }
}

void CPU::opcodeAnnn() {
    PRINT_DEBUG("opcode Annn");
    
    I = nnn();
}

void CPU::opcodeBnnn() {
    PRINT_DEBUG("opcode Bnnn");
    
    pc = registers[0] + nnn();
}

void CPU::opcodeCxkk() {
    PRINT_DEBUG("opcode Cxkk");
    
    registers[x()] = distribution(randomEngine) & (opcode & kk());
}

void CPU::opcodeDxyn() {
    PRINT_DEBUG("opcode Dxyn");

    uint8_t xP = registers[x()] % VIDEO_WIDTH;
    uint8_t yP = registers[y()] % VIDEO_HEIGHT;
    
    registers[0xF] = 0;
    
    for(int i = 0; i < n(); ++i) {
        uint8_t sprite = ram[I + i];
        
        for(int j = 0; j < 8; ++j) {
            uint8_t pixel = sprite & (0x80 >> j);
            uint32_t* pixelPtr = &screen[(yP + i) * VIDEO_WIDTH + (xP + j)];
            
            if (pixel) {
                if (*pixelPtr == SCREEN_PIXEL_CONSTANT) {
                    registers[0xF] = 1;
                }
                
                *pixelPtr ^= SCREEN_PIXEL_CONSTANT;
            }
        }
    }
}

void CPU::opcodeEx9E() {
    PRINT_DEBUG("opcode Ex9E");
    
    if(keyboard[registers[x()]]) {
        pc += 2;
    }
}

void CPU::opcodeExA1() {
    PRINT_DEBUG("opcode ExA1");
    
    if(!keyboard[registers[x()]]) {
        pc += 2;
    }
}

void CPU::opcodeFx07() {
    PRINT_DEBUG("opcode Fx07");
    
    registers[x()] = delayTimer;
}

void CPU::opcodeFx0A() {
    PRINT_DEBUG("opcode Fx0A");
    
    for(int i = 0; i < 16; ++i) {
        if (keyboard[i]) {
            registers[x()] = i;
            
            return;
        }
    }
    
    pc -=2;
}

void CPU::opcodeFx15() {
    PRINT_DEBUG("opcode Fx015");
    
    delayTimer = registers[x()];
}

void CPU::opcodeFx18() {
    PRINT_DEBUG("opcode Fx018");
    
    soundTimer = registers[x()];
}

void CPU::opcodeFx1E() {
    PRINT_DEBUG("opcode Fx1DE");
    
    I += registers[x()];
}

void CPU::opcodeFx29() {
    PRINT_DEBUG("opcode Fx29");
    
    I = STARTING_ADDRESS_FONTSET + (5*registers[x()]);
}

void CPU::opcodeFx33() {
    PRINT_DEBUG("opcode Fx33");
    
    uint8_t contentVx = registers[x()];
    
    ram[I] = (contentVx / 100) % 10;
    ram[I + 1] = (contentVx / 10) % 10;
    ram[I + 2] = contentVx % 10;
}

void CPU::opcodeFx55() {
    PRINT_DEBUG("opcode Fx55");
    
    memcpy(&ram[I], registers, (x()+1)*sizeof(uint8_t));
}

void CPU::opcodeFx65() {
    PRINT_DEBUG("opcode Fx65");
    
    memcpy(registers, &ram[I], (x()+1)*sizeof(uint8_t));
}

void CPU::printErrorOnOpcode() {
    std::cerr << "Undefined opcode: " << std::hex << opcode << std::endl;
    throw std::runtime_error("");
}

void CPU::executeOpcode00EStar() {
    PRINT_DEBUG("opcode 00EStar");
    switch(opcode & 0x000F) {
        case 0x0000:
            opcode00E0();
            break;
        case 0x000E:
            opcode00EE();
            break;
        default:
            printErrorOnOpcode();
    }
}

void CPU::executeOpcodeEXStarStar() {
    PRINT_DEBUG("opcode 00EXStarStar");
    switch(opcode & 0x00FF) {
        case 0x009E:
            opcodeEx9E();
            break;
        case 0x00A1:
            opcodeExA1();
            break;
        default:
            printErrorOnOpcode();
    }
}

void CPU::opcodeFXStarStar() {
    PRINT_DEBUG("opcode FXStarStar");
    switch(opcode & 0x00FF) {
        case 0x0007:
            opcodeFx07();
            break;
        case 0x000A:
            opcodeFx0A();
            break;
        case 0x0015:
            opcodeFx15();
            break;
        case 0x0018:
            opcodeFx18();
            break;
        case 0x001E:
            opcodeFx1E();
            break;
        case 0x0029:
            opcodeFx29();
            break;
        case 0x0033:
            opcodeFx33();
            break;
        case 0x0055:
            opcodeFx55();
            break;
        case 0x0065:
            opcodeFx65();
            break;
        default:
            printErrorOnOpcode();
    }
}

void CPU::executeOpcode0x8StarStarStar() {
    PRINT_DEBUG("opcode 0x8StarStarStar");
    switch(opcode & 0x000F) {
        case 0x0000:
            opcode8xy0();
            break;
        case 0x0001:
            opcode8xy1();
            break;
        case 0x0002:
            opcode8xy2();
            break;
        case 0x0003:
            opcode8xy3();
            break;
        case 0x0004:
            opcode8xy4();
            break;
        case 0x0005:
            opcode8xy5();
            break;
        case 0x0006:
            opcode8xy6();
            break;
        case 0x0007:
            opcode8xy7();
            break;
        case 0x000E:
            opcode8xyE();
            break;
        default:
            printErrorOnOpcode();
    }
}

// TODO: Maybe use a function for chosing between
/// one or the other (executeInstruction or opcodeTable)
void CPU::executeInstruction() {
    PRINT_DEBUG("Execute Instruction");
    switch(opcode & 0xF000) {
        case 0x0000:
            // Opcode: 00E*
            executeOpcode00EStar();
            break;
        case 0x1000:
            opcode1nnn();
            break;
        case 0x2000:
            opcode2nnn();
            break;
        case 0x3000:
            opcode3xkk();
            break;
        case 0x4000:
            opcode4xkk();
            break;
        case 0x5000:
            opcode5xy0();
            break;
        case 0x6000:
            opcode6xkk();
            break;
        case 0x7000:
            opcode7xkk();
            break;
        case 0x8000:
            // 0x8***
            executeOpcode0x8StarStarStar();
            break;
        case 0x9000:
            opcode9xy0();
            break;
        case 0xA000:
            opcodeAnnn();
            break;
        case 0xB000:
            opcodeBnnn();
            break;
        case 0xC000:
            opcodeCxkk();
            break;
        case 0xD000:
            opcodeDxyn();
            break;
        case 0xE000:
            // Opcode: EX**
            executeOpcodeEXStarStar();
            break;
        case 0xF000:
            // Opcode FX**
            opcodeFXStarStar();
            break;
        default:
            printErrorOnOpcode();
    }
}

void CPU::runCycle() {
    PRINT_DEBUG("Run Cycle");
    opcode = (ram[pc] << 8u) | ram[pc + 1];
    pc += 2;

    // executeInstruction();
    PRINT_DEBUG("Execute Instruction");
    (this->*table[(opcode & 0x0F000u) >> 12u])();
    PRINT_DEBUG("Finished executing Instruction");
    
    if(soundTimer > 0) {
        if(soundTimer == 1) {
            simpleSound.play(FREQUENCY, SOUND_DURATION);
        }

        --soundTimer;
    }
    
    if (delayTimer > 0) {
        --delayTimer;
    }
}

