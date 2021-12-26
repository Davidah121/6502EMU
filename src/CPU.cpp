#include "CPU.h"
#include <glib/StringTools.h>

#define ZERO_CHECK(a) (a==0)
#define NEGATIVE_CHECK(a) ((a & 0b10000000) != 0)

CPU::CPU(Memory* k)
{
    mem = k;
    reset();
    cyclesJustUsed = 0;
    totalCyclesUsed = 0;
}

CPU::~CPU()
{
}

void CPU::step()
{
    if(mem!=nullptr)
    {
        if(doInterupt && iFlag==0)
        {
            interupt();
        }
        else if(doManditoryInterupt)
        {
            interuptManditory();
        }
        else if(doReset)
        {
            reset();
        }
        else
        {
            bool valid = doInstruction();

            if(!valid)
            {
                finished = true;
            }
        }
    }
}

bool CPU::getFinished()
{
    return finished;
}

int CPU::getCyclesJustUsed()
{
    return cyclesJustUsed;
}

size_t CPU::getTotalCyclesUsed()
{
    return totalCyclesUsed;
}

void CPU::requestInterupt()
{
    doInterupt = true;
}

void CPU::requestManditoryInterupt()
{
    doManditoryInterupt = true;
}

void CPU::requestReset()
{
    doReset = true;
}

unsigned short CPU::getPC()
{
    return PC;
}

unsigned char CPU::getSP()
{
    return SP;
}

unsigned char CPU::getX()
{
    return X;
}

unsigned char CPU::getY()
{
    return Y;
}

unsigned char CPU::getA()
{
    return A;
}

unsigned char CPU::getCPUFlags()
{
    unsigned char status = 0;

    status |= nFlag;
    status = status<<1;

    status |= vFlag;
    status = status<<1;

    //EMPTY FLAG??
    status |= 1;
    status = status<<1;

    status |= bFlag;
    status = status<<1;

    status |= dFlag;
    status = status<<1;

    status |= iFlag;
    status = status<<1;

    status |= zFlag;
    status = status<<1;

    status |= cFlag;

    return status;
}

bool CPU::doInstruction()
{
    //read first byte
    bool valid = true;
    int opcode = mem->getMemory(PC);
    
    int bytesToRead = CPU::OpCodes[opcode].bytes;
    int cycles = CPU::OpCodes[opcode].cycles;

    switch (opcode)
    {
    #pragma region ADC
    case 0x69:
        ADC(getImmediate());
        break;
    case 0x65:
        ADC(getZeroPage());
        break;
    case 0x75:
        ADC(getZeroPageX());
        break;
    case 0x6D:
        ADC(getAbsolute());
        break;
    case 0x7D:
        ADC(getAbsoluteX(&cycles));
        break;
    case 0x79:
        ADC(getAbsoluteY(&cycles));
        break;
    case 0x61:
        ADC(getIndirectX());
        break;
    case 0x71:
        ADC(getIndirectY(&cycles));
        break;
    #pragma endregion

    #pragma region AND
    case 0x29:
        AND(getImmediate());
        break;
    case 0x25:
        AND(getZeroPage());
        break;
    case 0x35:
        AND(getZeroPageX());
        break;
    case 0x2D:
        AND(getAbsolute());
        break;
    case 0x3D:
        AND(getAbsoluteX(&cycles));
        break;
    case 0x39:
        AND(getAbsoluteY(&cycles));
        break;
    case 0x21:
        AND(getIndirectX());
        break;
    case 0x31:
        AND(getIndirectY(&cycles));
        break;
    #pragma endregion

    #pragma region ASL
    case 0x0A:
        ASL(A);
        break;
    case 0x06:
        ASL(getZeroPage());
        break;
    case 0x16:
        ASL(getZeroPageX());
        break;
    case 0x0E:
        ASL(getAbsolute());
        break;
    case 0x1E:
        ASL(getAbsoluteX(&cycles));
        break;
    #pragma endregion

    #pragma region BCC
    case 0x90:
        BCC(getImmediate(), &cycles);
        break;
    #pragma endregion

    #pragma region BCS
    case 0xB0:
        BCS(getImmediate(), &cycles);
        break;
    #pragma endregion

    #pragma region BEQ
    case 0xF0:
        BEQ(getImmediate(), &cycles);
        break;
    #pragma endregion

    #pragma region BIT
    case 0x24:
        BIT(getZeroPage());
        break;
    case 0x2C:
        BIT(getAbsolute());
        break;
    #pragma endregion

    #pragma region BMI
    case 0x30:
        BMI(getImmediate(), &cycles);
        break;
    #pragma endregion

    #pragma region BNE
    case 0xD0:
        BNE(getImmediate(), &cycles);
        break;
    #pragma endregion

    #pragma region BPL
    case 0x10:
        BPL(getImmediate(), &cycles);
        break;
    #pragma endregion

    #pragma region BRK
    case 0x00:
        BRK();
        break;
    #pragma endregion

    #pragma region BVC
    case 0x50:
        BVC(getImmediate(), &cycles);
        break;
    #pragma endregion

    #pragma region BVS
    case 0x70:
        BVS(getImmediate(), &cycles);
        break;
    #pragma endregion

    #pragma region CLC
    case 0x18:
        CLC();
        break;
    #pragma endregion

    #pragma region CLD
    case 0xD8:
        CLD();
        break;
    #pragma endregion

    #pragma region CLI
    case 0x58:
        CLI();
        break;
    #pragma endregion

    #pragma region CLV
    case 0xB8:
        CLV();
        break;
    #pragma endregion

    #pragma region CMP
    case 0xC9:
        CMP(getImmediate());
        break;
    case 0xC5:
        CMP(getZeroPage());
        break;
    case 0xD5:
        CMP(getZeroPageX());
        break;
    case 0xCD:
        CMP(getAbsolute());
        break;
    case 0xDD:
        CMP(getAbsoluteX(&cycles));
        break;
    case 0xD9:
        CMP(getAbsoluteY(&cycles));
        break;
    case 0xC1:
        CMP(getIndirectX());
        break;
    case 0xD1:
        CMP(getIndirectY(&cycles));
        break;
    #pragma endregion

    #pragma region CPX
    case 0xE0:
        CPX(getImmediate());
        break;
    case 0xE4:
        CPX(getZeroPage());
        break;
    case 0xEC:
        CPX(getAbsolute());
        break;
    #pragma endregion

    #pragma region CPY
    case 0xC0:
        CPY(getImmediate());
        break;
    case 0xC4:
        CPY(getZeroPage());
        break;
    case 0xCC:
        CPY(getAbsolute());
        break;
    #pragma endregion

    #pragma region DEC
    case 0xC6:
        DEC(getZeroPage());
        break;
    case 0xD6:
        DEC(getZeroPageX());
        break;
    case 0xCE:
        DEC(getAbsolute());
        break;
    case 0xDE:
        DEC(getAbsoluteX(nullptr));
        break;
    #pragma endregion

    #pragma region DEX
    case 0xCA:
        DEX();
        break;
    #pragma endregion

    #pragma region DEY
    case 0x88:
        DEY();
        break;
    #pragma endregion

    #pragma region EOR
    case 0x49:
        EOR(getImmediate());
        break;
    case 0x45:
        EOR(getZeroPage());
        break;
    case 0x55:
        EOR(getZeroPageX());
        break;
    case 0x4D:
        EOR(getAbsolute());
        break;
    case 0x5D:
        EOR(getAbsoluteX(&cycles));
        break;
    case 0x59:
        EOR(getAbsoluteY(&cycles));
        break;
    case 0x41:
        EOR(getIndirectX());
        break;
    case 0x51:
        EOR(getIndirectY(&cycles));
        break;
    #pragma endregion

    #pragma region INC
    case 0xE6:
        INC(getZeroPage());
        break;
    case 0xF6:
        INC(getZeroPageX());
        break;
    case 0xEE:
        INC(getAbsolute());
        break;
    case 0xFE:
        INC(getAbsoluteX(nullptr));
        break;
    #pragma endregion

    #pragma region INX
    case 0xE8:
        INX();
        break;
    #pragma endregion

    #pragma region INY
    case 0xC8:
        INY();
        break;
    #pragma endregion

    #pragma region JMP
    case 0x4C:
        //JMP ABSOLUTE
        JMP(getImmediateShort());
        bytesToRead = 0;
        break;
    case 0x6C:
        //JMP INDIRECT
        JMP(getIndirectShort());
        bytesToRead = 0;
        break;
    #pragma endregion

    #pragma region JSR
    case 0x20:
        //ABSOLUTE
        JSR(getImmediateShort());
        break;
    #pragma endregion

    #pragma region LDA
    case 0xA9:
        LDA(getImmediate());
        break;
    case 0xA5:
        LDA(getZeroPage());
        break;
    case 0xB5:
        LDA(getZeroPageX());
        break;
    case 0xAD:
        LDA(getAbsolute());
        break;
    case 0xBD:
        LDA(getAbsoluteX(&cycles));
        break;
    case 0xB9:
        LDA(getAbsoluteY(&cycles));
        break;
    case 0xA1:
        LDA(getIndirectX());
        break;
    case 0xB1:
        LDA(getIndirectY(&cycles));
        break;
    #pragma endregion

    #pragma region LDX
    case 0xA2:
        LDX(getImmediate());
        break;
    case 0xA6:
        LDX(getZeroPage());
        break;
    case 0xB6:
        LDX(getZeroPageY());
        break;
    case 0xAE:
        LDX(getAbsolute());
        break;
    case 0xBE:
        LDX(getAbsoluteY(&cycles));
        break;
    #pragma endregion

    #pragma region LDY
    case 0xA0:
        LDY(getImmediate());
        break;
    case 0xA4:
        LDY(getZeroPage());
        break;
    case 0xB4:
        LDY(getZeroPageX());
        break;
    case 0xAC:
        LDY(getAbsolute());
        break;
    case 0xBC:
        LDY(getAbsoluteX(&cycles));
        break;
    #pragma endregion

    #pragma region LSR
    case 0x4A:
        LSR(A);
        break;
    case 0x46:
        LSR(getZeroPage());
        break;
    case 0x56:
        LSR(getZeroPageX());
        break;
    case 0x4E:
        LSR(getAbsolute());
        break;
    case 0x5E:
        LSR(getAbsoluteX(nullptr));
        break;
    #pragma endregion

    #pragma region NOP
    case 0xEA:
        NOP();
        break;
    #pragma endregion

    #pragma region ORA
    case 0x09:
        ORA(getImmediate());
        break;
    case 0x05:
        ORA(getZeroPage());
        break;
    case 0x15:
        ORA(getZeroPageX());
        break;
    case 0x0D:
        ORA(getAbsolute());
        break;
    case 0x1D:
        ORA(getAbsoluteX(&cycles));
        break;
    case 0x19:
        ORA(getAbsoluteY(&cycles));
        break;
    case 0x01:
        ORA(getIndirectX());
        break;
    case 0x11:
        ORA(getIndirectY(&cycles));
        break;
    #pragma endregion

    #pragma region PHA
    case 0x48:
        PHA();
        break;
    #pragma endregion

    #pragma region PHP
    case 0x08:
        PHP();
        break;
    #pragma endregion

    #pragma region PLA
    case 0x68:
        PLA();
        break;
    #pragma endregion

    #pragma region PLP
    case 0x28:
        PLP();
        break;
    #pragma endregion

    #pragma region ROL
    case 0x2A:
        ROL(A);
        break;
    case 0x26:
        ROL(getZeroPage());
        break;
    case 0x36:
        ROL(getZeroPageX());
        break;
    case 0x2E:
        ROL(getAbsolute());
        break;
    case 0x3E:
        ROL(getAbsoluteX(nullptr));
        break;
    #pragma endregion

    #pragma region ROR
        case 0x6A:
        ROR(A);
        break;
    case 0x66:
        ROR(getZeroPage());
        break;
    case 0x76:
        ROR(getZeroPageX());
        break;
    case 0x6E:
        ROR(getAbsolute());
        break;
    case 0x7E:
        ROR(getAbsoluteX(nullptr));
        break;
    #pragma endregion

    #pragma region RTI
    case 0x40:
        RTI();
        break;
    #pragma endregion

    #pragma region RTS
    case 0x60:
        RTS();
        break;
    #pragma endregion

    #pragma region SBC
    case 0xE9:
        SBC(getImmediate());
        break;
    case 0xE5:
        SBC(getZeroPage());
        break;
    case 0xF5:
        SBC(getZeroPageX());
        break;
    case 0xED:
        SBC(getAbsolute());
        break;
    case 0xFD:
        SBC(getAbsoluteX(&cycles));
        break;
    case 0xF9:
        SBC(getAbsoluteY(&cycles));
        break;
    case 0xE1:
        SBC(getIndirectX());
        break;
    case 0xF1:
        SBC(getIndirectY(&cycles));
        break;
    #pragma endregion

    #pragma region SEC
    case 0x38:
        SEC();
        break;
    #pragma endregion

    #pragma region SED
    case 0xF8:
        SED();
        break;
    #pragma endregion

    #pragma region SEI
    case 0x78:
        SEI();
        break;
    #pragma endregion

    #pragma region STA
    case 0x85:
        STA(getZeroPage());
        break;
    case 0x95:
        STA(getZeroPageX());
        break;
    case 0x8D:
        STA(getAbsolute());
        break;
    case 0x9D:
        STA(getAbsoluteX(&cycles));
        break;
    case 0x99:
        STA(getAbsoluteY(&cycles));
        break;
    case 0x81:
        STA(getIndirectX());
        break;
    case 0x91:
        STA(getIndirectY(&cycles));
        break;
    #pragma endregion

    #pragma region STX
    case 0x86:
        STX(getZeroPage());
        break;
    case 0x96:
        STX(getZeroPageY());
        break;
    case 0x8E:
        STX(getAbsolute());
        break;
    #pragma endregion

    #pragma region STY
    case 0x84:
        STY(getZeroPage());
        break;
    case 0x94:
        STY(getZeroPageX());
        break;
    case 0x8C:
        STY(getAbsolute());
        break;
    #pragma endregion

    #pragma region TAX
    case 0xAA:
        TAX();
        break;
    #pragma endregion

    #pragma region TAY
    case 0xA8:
        TAY();
        break;
    #pragma endregion

    #pragma region TSX
    case 0xBA:
        TSX();
        break;
    #pragma endregion

    #pragma region TXA
    case 0x8A:
        TXA();
        break;
    #pragma endregion

    #pragma region TXS
    case 0x9A:
        TXS();
        break;
    #pragma endregion

    #pragma region TYA
    case 0x98:
        TYA();
        break;
    #pragma endregion

    default:
        //invalid instruction probably
        valid = false;
        break;
    }

    PC += bytesToRead;
    cyclesJustUsed = cycles;
    totalCyclesUsed += cycles;

    return valid;
}

#pragma MEMORY_ACCESS

byte& CPU::getImmediate()
{
    return mem->getMemory(PC+1);
}

byte& CPU::getZeroPage()
{
    return mem->getMemory( mem->getMemory(PC+1) );
}

byte& CPU::getZeroPageX()
{
    unsigned short location = (byte)mem->getMemory(PC+1) + X;
    return mem->getMemory(location);
}

byte& CPU::getZeroPageY()
{
    unsigned short location = (byte)mem->getMemory(PC+1) + Y;
    return mem->getMemory(location);
}

byte& CPU::getAbsolute()
{
    unsigned short location = ((int)mem->getMemory(PC+2)<<8) + mem->getMemory(PC+1);
    return mem->getMemory(location);
}

byte& CPU::getAbsoluteX(int* cycles)
{
    unsigned short location = ((int)mem->getMemory(PC+2)<<8) + mem->getMemory(PC+1);
    
    int pageNum = location/256;
    location += X;
    int nPageNum = location/256;
    //changed page number
    
    if(nPageNum!=pageNum && cycles!=nullptr)
        (*cycles)++;

    return mem->getMemory(location);
}

byte& CPU::getAbsoluteY(int* cycles)
{
    unsigned short location = ((int)mem->getMemory(PC+2)<<8) + mem->getMemory(PC+1);
    
    int pageNum = location/256;
    location += Y;
    int nPageNum = location/256;
    //changed page number
    if(nPageNum!=pageNum && cycles!=nullptr)
        (*cycles)++;
        
    return mem->getMemory(location);
}

byte& CPU::getIndirectX()
{
    unsigned short location = mem->getMemory(PC+1);
    location += X;

    return mem->getMemory(location);
}

byte& CPU::getIndirectY(int* cycles)
{
    unsigned short zeroPageLocation = mem->getMemory(PC+1);
    unsigned short location = ((int)mem->getMemory(zeroPageLocation+1)<<8) + mem->getMemory(zeroPageLocation);
    
    int pageNum = location/256;
    location += Y;
    int nPageNum = location/256;
    //changed page number
    if(nPageNum!=pageNum && cycles!=nullptr)
        (*cycles)++;
    
    return mem->getMemory(location);
}

unsigned short CPU::getImmediateShort()
{
    unsigned short location = ((int)mem->getMemory(PC+2)<<8) + mem->getMemory(PC+1);
    return location;
}

unsigned short CPU::getIndirectShort()
{
    unsigned short location = ((int)mem->getMemory(PC+2)<<8) + mem->getMemory(PC+1);
    unsigned short result = ((int)mem->getMemory(location+1)<<8) + mem->getMemory(location);
    return result;
}

#pragma endregion

#pragma region INSTRUCTIONS

void CPU::ADC(byte value)
{
    int result = A + value + cFlag;

    if(A < 0x80 && value < 0x80)
    {
        vFlag = ((result & 0b10000000) != 0);
    }
    else
        vFlag = false;

    if(A > 0x7F && value > 0x7F)
    {
        vFlag = ((result & 0b10000000) == 0);
    }
    else
        vFlag = false;

    A = result & 0xFF;

    cFlag = (result & 0b100000000) != 0;
    zFlag = ZERO_CHECK(A);
    nFlag = NEGATIVE_CHECK(A);
}

void CPU::AND(byte value)
{
    A &= value;

    zFlag = ZERO_CHECK(A);
    nFlag = NEGATIVE_CHECK(A);
}

void CPU::ASL(byte& value)
{
    int nValue = value << 1;
    value = nValue & 0xFF;

    cFlag = nValue>0xFF;

    zFlag = ZERO_CHECK(value);
    nFlag = NEGATIVE_CHECK(value);
}

void CPU::BCC(byte value, int* cycles)
{
    if(cFlag == false)
    {
        (*cycles)++;

        unsigned short location = PC+2;
        
        int pageNum = location/256;
        location += (char)value;
        int nPageNum = location/256;
        //changed page number
        if(nPageNum!=pageNum && cycles!=nullptr)
            (*cycles)++;

        PC = location-1;
    }
}

void CPU::BCS(byte value, int* cycles)
{
    if(cFlag == true)
    {
        (*cycles)++;
        
        unsigned short location = PC+2;
        
        int pageNum = location/256;
        location += (char)value;
        int nPageNum = location/256;
        //changed page number
        if(nPageNum!=pageNum && cycles!=nullptr)
            (*cycles)++;

        PC = location-1;
    }
}

void CPU::BEQ(byte value, int* cycles)
{
    if(zFlag == true)
    {
        (*cycles)++;
        
        unsigned short location = PC+1;
        
        int pageNum = location/256;
        location += (char)value;
        int nPageNum = location/256;
        //changed page number
        if(nPageNum!=pageNum && cycles!=nullptr)
            (*cycles)++;

        PC = location-1;
    }
}

void CPU::BIT(byte value)
{
    byte result = A & value;

    zFlag = (result==0);
    vFlag = (result>>6) & 0x01;
    nFlag = (result>>7) & 0x01;
}

void CPU::BMI(byte value, int* cycles)
{
    if(nFlag == true)
    {
        (*cycles)++;
        
        unsigned short location = PC;
        
        int pageNum = location/256;
        location += (char)value;
        int nPageNum = location/256;
        //changed page number
        if(nPageNum!=pageNum && cycles!=nullptr)
            (*cycles)++;

        PC = location;
    }
}

void CPU::BNE(byte value, int* cycles)
{
    if(zFlag == false)
    {
        (*cycles)++;
        
        unsigned short location = PC;
        
        int pageNum = location/256;
        location += (char)value;
        int nPageNum = location/256;
        //changed page number
        if(nPageNum!=pageNum && cycles!=nullptr)
            (*cycles)++;

        PC = location;
    }
}

void CPU::BPL(byte value, int* cycles)
{
    if(nFlag == false)
    {
        (*cycles)++;
        
        unsigned short location = PC;
        
        int pageNum = location/256;
        location += (char)value;
        int nPageNum = location/256;
        //changed page number
        if(nPageNum!=pageNum && cycles!=nullptr)
            (*cycles)++;

        PC = location;
    }
}

void CPU::BRK()
{
    unsigned short IRQ = 0xFFFE;
    unsigned short jmpLocation = mem->getMemory(IRQ);
    jmpLocation = (jmpLocation<<8) + mem->getMemory(IRQ+1);

    JMP(jmpLocation);
    PHP();
    bFlag = true;
    finished = true;
}

void CPU::BVC(byte value, int* cycles)
{
    if(vFlag == false)
    {
        (*cycles)++;
        
        unsigned short location = PC;
        
        int pageNum = location/256;
        location += (char)value;
        int nPageNum = location/256;
        //changed page number
        if(nPageNum!=pageNum && cycles!=nullptr)
            (*cycles)++;

        PC = location;
    }
}

void CPU::BVS(byte value, int* cycles)
{
    if(vFlag == true)
    {
        (*cycles)++;
        
        unsigned short location = PC;
        
        int pageNum = location/256;
        location += (char)value;
        int nPageNum = location/256;
        //changed page number
        if(nPageNum!=pageNum && cycles!=nullptr)
            (*cycles)++;

        PC = location;
    }
}

void CPU::CLC()
{
    cFlag = false;
}

void CPU::CLD()
{
    dFlag = false;
}

void CPU::CLI()
{
    iFlag = false;
}

void CPU::CLV()
{
    vFlag = false;
}

void CPU::CMP(byte value)
{
    int result = A - value;
    cFlag = A >= value;
    zFlag = ZERO_CHECK(result);
    nFlag = NEGATIVE_CHECK(result);
}

void CPU::CPX(byte value)
{
    int result = X - value;
    cFlag = X >= value;
    zFlag = ZERO_CHECK(result);
    nFlag = NEGATIVE_CHECK(result);
}

void CPU::CPY(byte value)
{
    int result = Y - value;
    cFlag = Y >= value;
    zFlag = ZERO_CHECK(result);
    nFlag = NEGATIVE_CHECK(result);
}

void CPU::DEC(byte& value)
{
    value--;
    zFlag = ZERO_CHECK(value);
    nFlag = NEGATIVE_CHECK(value);
}

void CPU::DEX()
{
    X--;
    zFlag = ZERO_CHECK(X);
    nFlag = NEGATIVE_CHECK(X);
}

void CPU::DEY()
{
    Y--;
    zFlag = ZERO_CHECK(Y);
    nFlag = NEGATIVE_CHECK(Y);
}

void CPU::EOR(byte value)
{
    A ^= value;
    zFlag = ZERO_CHECK(A);
    nFlag = NEGATIVE_CHECK(A);
}

void CPU::INC(byte& value)
{
    value++;
    zFlag = ZERO_CHECK(value);
    nFlag = NEGATIVE_CHECK(value);
}

void CPU::INX()
{
    X++;
    zFlag = ZERO_CHECK(X);
    nFlag = NEGATIVE_CHECK(X);
}

void CPU::INY()
{
    Y++;
    zFlag = ZERO_CHECK(Y);
    nFlag = NEGATIVE_CHECK(Y);
}

void CPU::JMP(unsigned short location)
{
    PC = location;
}

void CPU::JSR(unsigned short location)
{
    unsigned short oldLocation = PC-1;
    mem->setMemory(STACK_LOCATION + SP, oldLocation & 0xFF);
    SP++;
    mem->setMemory(STACK_LOCATION + SP, (oldLocation>>8) & 0xFF);
    SP++;
    
    PC = location;
}

void CPU::LDA(byte value)
{
    A = value;
    zFlag = ZERO_CHECK(A);
    nFlag = NEGATIVE_CHECK(A);
}

void CPU::LDX(byte value)
{
    X = value;
    zFlag = ZERO_CHECK(X);
    nFlag = NEGATIVE_CHECK(X);
}

void CPU::LDY(byte value)
{
    Y = value;
    zFlag = ZERO_CHECK(Y);
    nFlag = NEGATIVE_CHECK(Y);
}

void CPU::LSR(byte& value)
{
    int nValue = value >> 1;
    
    cFlag = value & 0x01;

    value = nValue & 0xFF;

    zFlag = ZERO_CHECK(value);
    nFlag = NEGATIVE_CHECK(value);
}

void CPU::NOP()
{

}

void CPU::ORA(byte value)
{
    A |= value;

    zFlag = ZERO_CHECK(value);
    nFlag = NEGATIVE_CHECK(value);
}

void CPU::PHA()
{
    mem->setMemory(STACK_LOCATION + SP, A);
    SP--;
}

void CPU::PHP()
{
    byte status = getCPUFlags();

    mem->setMemory(STACK_LOCATION + SP, status);
    SP--;
}

void CPU::PLA()
{
    SP++;
    A = mem->getMemory(STACK_LOCATION + SP);

    zFlag = ZERO_CHECK(A);
    nFlag = NEGATIVE_CHECK(A);
}

void CPU::PLP()
{
    SP++;
    byte status = mem->getMemory(STACK_LOCATION + SP);

    cFlag = status&0b000000001;
    zFlag = status&0b000000010;
    iFlag = status&0b000000100;
    dFlag = status&0b000001000;
    bFlag = status&0b000010000;
    //cFlag = status&0b001000000;
    vFlag = status&0b01000000;
    nFlag = status&0b100000000;
}

void CPU::ROL(byte& value)
{
    byte oldFlag = cFlag;
    cFlag = value>>7;
    value = value<<1;
    value += oldFlag;

    zFlag = ZERO_CHECK(value);
    nFlag = NEGATIVE_CHECK(value);
}

void CPU::ROR(byte& value)
{
    byte oldFlag = cFlag;
    cFlag = value & 0x01;
    value = value>>1;
    value += oldFlag<<7;

    zFlag = ZERO_CHECK(value);
    nFlag = NEGATIVE_CHECK(value);
}

void CPU::RTI()
{
    PLA();

    SP++;
    unsigned short location = mem->getMemory(STACK_LOCATION + SP);
    SP++;
    location = (location<<8) + mem->getMemory(STACK_LOCATION + SP);

    PC = location;

    doInterupt = false;
    doManditoryInterupt = false;
}

void CPU::RTS()
{
    SP++;
    unsigned short location = mem->getMemory(STACK_LOCATION + SP);
    SP++;
    location = (location<<8) + mem->getMemory(STACK_LOCATION + SP);

    PC = location;
}

void CPU::SBC(byte value)
{
    int result = A - value - (1-cFlag);

    if(A < 0x80 && value > 0x7F)
    {
        vFlag = ((result & 0b10000000) == 0);
    }
    else
        vFlag = false;

    if(A > 0x7F && value < 0x80)
    {
        vFlag = ((result & 0b10000000) != 0);
    }
    else
        vFlag = false;

    A = result & 0xFF;

    cFlag = (result & 0b100000000) == 0;
    zFlag = ZERO_CHECK(A);
    nFlag = NEGATIVE_CHECK(A);
}

void CPU::SEC()
{
    cFlag = true;
}

void CPU::SED()
{
    dFlag = true;
}

void CPU::SEI()
{
    iFlag = true;
}

void CPU::STA(byte& location)
{
    location = A;
}

void CPU::STX(byte& location)
{
    location = X;
}

void CPU::STY(byte& location)
{
    location = Y;
}

void CPU::TAX()
{
    X = A;
    zFlag = ZERO_CHECK(X);
    nFlag = NEGATIVE_CHECK(X);
}

void CPU::TAY()
{
    Y = A;
    zFlag = ZERO_CHECK(Y);
    nFlag = NEGATIVE_CHECK(Y);
}

void CPU::TSX()
{
    X = SP;
    zFlag = ZERO_CHECK(X);
    nFlag = NEGATIVE_CHECK(X);
}

void CPU::TXA()
{
    A = X;
    zFlag = ZERO_CHECK(A);
    nFlag = NEGATIVE_CHECK(A);
}

void CPU::TXS()
{
    Y = SP;
    zFlag = ZERO_CHECK(Y);
    nFlag = NEGATIVE_CHECK(Y);
}

void CPU::TYA()
{
    A = Y;
    zFlag = ZERO_CHECK(A);
    nFlag = NEGATIVE_CHECK(A);
}

void CPU::interupt()
{
    if(iFlag == 0)
    {
        mem->setMemory(STACK_LOCATION + SP, (PC&0xFF));
        SP--;
        mem->setMemory(STACK_LOCATION + SP, ((PC>>8)&0xFF));
        SP--;

        bFlag = false;
        iFlag = true;
        PHP();

        PC = 0xFFFE;
        PC = mem->getMemory(PC+1);
        PC = (PC<<8) + mem->getMemory(PC);

        cyclesJustUsed = 7;
        totalCyclesUsed += 7;
    }
}

void CPU::interuptManditory()
{
    mem->setMemory(STACK_LOCATION + SP, (PC&0xFF));
    SP--;
    mem->setMemory(STACK_LOCATION + SP, ((PC>>8)&0xFF));
    SP--;

    bFlag = false;
    iFlag = true;
    PHP();

    PC = 0xFFFA;
    PC = mem->getMemory(PC+1);
    PC = (PC<<8) + mem->getMemory(PC);

    cyclesJustUsed = 8;
    totalCyclesUsed += 8;
}

void CPU::reset()
{
    PC = 0xFFFC;
    SP = 0xFF;
    A = 0;
    X = 0;
    Y = 0;
    cFlag = 0;
    zFlag = 0;
    iFlag = 0;
    dFlag = 0;
    bFlag = 0;
    vFlag = 0;
    nFlag = 0;

    //set PC to whatever is at 0xFFFC
    PC = mem->getMemory(PC+1);
    PC = (PC<<8) + mem->getMemory(PC);

    cyclesJustUsed = 8;
    totalCyclesUsed += 8;

    doReset = false;
}



#pragma endregion