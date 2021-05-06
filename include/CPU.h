#pragma once
#include "Memory.h"

#define CLOCK_SPEED 1789773 //Measured in Hz. 1.789773 MHz
#define STACK_LOCATION 0x100

typedef unsigned char byte;

/*
    Clock speed represents how many ticks should be called per second.
    Instructions can take a different amount of ticks before finishing.
    CPU maintains its previous and next tick so it can wait accordingly.
*/

struct instructionInfo
{
    int bytes = 0;
    int cycles = 0;
};

class CPU
{
public:
    CPU(Memory* k);
    ~CPU();

    //processor instructions with bytes and cycles
    static instructionInfo OpCodes[256];

    void step();
    bool getFinished();
    int getCyclesJustUsed();
    size_t getTotalCyclesUsed();

    unsigned short getPC();
    unsigned char getSP();
    unsigned char getX();
    unsigned char getY();
    unsigned char getA();
    
    unsigned char getCPUFlags();

    void requestInterupt();
    void requestManditoryInterupt();
    void requestReset();

private:
    
    bool doInstruction();

    bool doInterupt = false;
    bool doManditoryInterupt = false;
    bool doReset = false;

    void interupt();
    void interuptManditory();
    void reset();

    byte& getImmediate();
    byte& getZeroPage();
    byte& getZeroPageX();
    byte& getZeroPageY();
    byte& getAbsolute();
    byte& getAbsoluteX(int* cycles);
    byte& getAbsoluteY(int* cycles);
    byte& getIndirectX();
    byte& getIndirectY(int* cycles);

    unsigned short getImmediateShort();
    unsigned short getIndirectShort();


    void ADC(byte value);
    void AND(byte value);
    void ASL_A(byte value);
    void ASL(byte& value);
    void BCC(byte value, int* cycles);
    void BCS(byte value, int* cycles);
    void BEQ(byte value, int* cycles);
    void BIT(byte value);
    void BMI(byte value, int* cycles);
    void BNE(byte value, int* cycles);
    void BPL(byte value, int* cycles);

    void BRK();

    void BVC(byte value, int* cycles);
    void BVS(byte value, int* cycles);
    void CLC();
    void CLD();
    void CLI();
    void CLV();
    void CMP(byte value);
    void CPX(byte value);
    void CPY(byte value);
    void DEC(byte& value);
    void DEX();
    void DEY();
    void EOR(byte value);
    void INC(byte& value);
    void INX();
    void INY();
    void JMP(unsigned short location);
    void JSR(unsigned short location);
    void LDA(byte value);
    void LDX(byte value);
    void LDY(byte value);
    void LSR(byte& value);
    void NOP();
    void ORA(byte value);
    void PHA();
    void PHP();
    void PLA();
    void PLP();

    void ROL(byte& value);
    void ROR(byte& value);

    void RTI();
    void RTS();

    void SBC(byte value);

    void SEC();
    void SED();
    void SEI();
    void STA(byte& location);
    void STX(byte& location);
    void STY(byte& location);

    void TAX();
    void TAY();

    void TSX();
    void TXA();
    void TXS();
    void TYA();

    bool cFlag = 0;
    bool zFlag = 0;
    bool iFlag = 0;
    bool dFlag = 0;
    bool bFlag = 0;
    bool vFlag = 0;
    bool nFlag = 0;

    byte X = 0;
    byte Y = 0;
    byte A = 0;
    
    byte SP = 0xFF;
    unsigned short PC = 0;

    int cyclesJustUsed = 0;
    size_t totalCyclesUsed = 0;

    bool finished = false;

    Memory* mem;
};
