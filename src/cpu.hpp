//
//  Chip-8
//
//  Created by Cosme Jordan on 12.09.20.
//  Copyright © 2020 Cosme Jordan. All rights reserved.
//

#ifndef cpu_hpp
#define cpu_hpp

#include <cstdint>
#include <cstring>
#include <fstream>

#include <chrono>
#include <random>

#include "sound.hpp"

#define INIT_VALUE 0

class CPU {
private:
    // Constants
    static const unsigned int NUMBER_FONTSETS = 80;
    static const unsigned int STARTING_ADDRESS = 0x200;
    static const unsigned int STARTING_ADDRESS_FONTSET = 0x50;
    static const uint32_t SCREEN_PIXEL_CONSTANT = 0xFFFFFFFF;
    
    static const unsigned int SIZE_TABLE = 0x10;
    static const unsigned int SIZE_TABLE0x0 = 0xF;
    static const unsigned int SIZE_TABLE0x8 = 0xF;
    static const unsigned int SIZE_TABLE0xE = 0xF;
    static const unsigned int SIZE_TABLE0xF = 0x100;
    
    static const unsigned int RAM_SIZE = 0x1000; // 4096
    static const unsigned int STACK_SIZE = 0x10; // 16
    static const unsigned int REGISTERS_SIZE = 0x10; // 16
    static const unsigned int KEYBOARD_SIZE = 0x10; // 16
    
    static const unsigned int SCREEN_SIZE = 64 * 32;
    
private:
    uint8_t registers[REGISTERS_SIZE];
    uint8_t ram[RAM_SIZE];
    uint16_t stack[STACK_SIZE];
    
    uint16_t opcode = INIT_VALUE;
    uint16_t I = INIT_VALUE;
    
    uint16_t pc = INIT_VALUE;
    uint8_t sp = INIT_VALUE;
    
    uint8_t delayTimer = INIT_VALUE;
    uint8_t soundTimer = INIT_VALUE;
    
    typedef void (CPU::*OpcodeFunction)();
    //    OpcodeFunction* table;
    //    OpcodeFunction* table0x0;
    //    OpcodeFunction* table0x8;
    //    OpcodeFunction* table0xE;
    //    OpcodeFunction* table0xF;
    
    OpcodeFunction table[SIZE_TABLE];
    OpcodeFunction table0x0[SIZE_TABLE0x0];
    OpcodeFunction table0x8[SIZE_TABLE0x8];
    OpcodeFunction table0xE[SIZE_TABLE0xE];
    OpcodeFunction table0xF[SIZE_TABLE0xF];


    static const int SOUND_DURATION = 50;
    static constexpr double FREQUENCY = 440;

    SimpleSound simpleSound;
    
public:
    uint8_t keyboard[KEYBOARD_SIZE];
    uint32_t screen[SCREEN_SIZE];
    
public:
    CPU();
    ~CPU();
    void loadROM(const char* filename);
    void runCycle();
    
private:
    
    void randomGenerator();
    std::default_random_engine randomEngine;
    std::uniform_int_distribution<uint8_t> distribution;
    
    void initNopes();
    void initOpcodeTables();
    
    void opcode0nnn();
    void opcode00E0();
    void opcode00EE();
    void opcode1nnn();
    void opcode2nnn();
    void opcode3xkk();
    void opcode4xkk();
    void opcode5xy0();
    void opcode6xkk();
    void opcode7xkk();
    void opcode8xy0();
    void opcode8xy1();
    void opcode8xy2();
    void opcode8xy3();
    void opcode8xy4();
    void opcode8xy5();
    void opcode8xy6();
    void opcode8xy7();
    void opcode8xyE();
    void opcode9xy0();
    void opcodeAnnn();
    void opcodeBnnn();
    void opcodeCxkk();
    void opcodeDxyn();
    void opcodeEx9E();
    void opcodeExA1();
    void opcodeFx07();
    void opcodeFx0A();
    void opcodeFx15();
    void opcodeFx18();
    void opcodeFx1E();
    void opcodeFx29();
    void opcodeFx33();
    void opcodeFx55();
    void opcodeFx65();
    
    // Utility functions
    uint8_t y();
    uint8_t x();
    uint8_t kk();
    uint16_t nnn();
    uint8_t n();
    
    size_t get0xFValue();
    size_t get0xFFValue();
    
    // =========================================================================
    // =========================================================================
    // =========================================================================
    
    void accessTable0x0();
    void accessTable0x8();
    void accessTable0xE();
    void accessTable0xF();
    void opcodeNOPE();
    
    // =========================================================================
    // =========================================================================
    // =========================================================================
    
    void executeOpcode00EStar();
    void executeOpcodeEXStarStar();
    void executeOpcode0x8StarStarStar();
    void opcodeFXStarStar();
    
    void executeInstruction();
    
    void printErrorOnOpcode();
};

#endif /* cpu_hpp */
