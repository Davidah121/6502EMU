#include "Assembler.h"
#include <unordered_map>

Assembler::Assembler()
{
}

Assembler::~Assembler()
{
}

std::vector<std::string> Assembler::convertToAsm(std::vector<unsigned char> bytes)
{
    std::vector<std::string> output;
    size_t index = 0;
    while(index < bytes.size())
    {
        unsigned char instruction = bytes[index];
        InstructionInfo info = CPU::OpCodes[instruction];
        std::string asmString;
        if(info.additionalBytes == 0)
        {
            if(info.cycles == 0)
            {
                //invalid line
                break;
            }
            asmString = info.syntax;
        }
        else if(info.additionalBytes == 1)
        {
            if(index+1 >= bytes.size())
            {
                //invalid line
                break;
            }
            unsigned char b1 = bytes[index+1];
            asmString = StringTools::formatString(info.syntax, b1);
        }
        else if(info.additionalBytes == 2)
        {
            if(index+2 >= bytes.size())
            {
                //invalid line
                break;
            }
            unsigned char b1 = bytes[index+1];
            unsigned char b2 = bytes[index+2];
            asmString = StringTools::formatString(info.syntax, b1, b2);
        }

        output.push_back( asmString );

        index+=1+info.additionalBytes;
    }
    return output;
}

std::vector<unsigned char> Assembler::convertToBinary(std::vector<std::string> asmCode, std::string& errorValue)
{
    std::vector<unsigned char> output;
    std::vector<std::string> parsedInput = removeCommentsAndEmptyLines(asmCode);

    for(int i=0; i<parsedInput.size(); i++)
    {
        std::vector<unsigned char> byteValues = convertLine( parsedInput[i] );
        InstructionInfo info = CPU::OpCodes[byteValues[0]];

        if(info.additionalBytes == 0 && info.cycles == 0)
        {
            errorValue = "Failed to parse assembly at line " + std::to_string(i);
            return {};
        }
        else
        {
            //probably okay
            for(int k=0; k<byteValues.size(); k++)
            {
                output.push_back(byteValues[k]);
            }
        }
    }

    return output;
}


std::vector<std::string> Assembler::removeCommentsAndEmptyLines(std::vector<std::string> asmCode)
{
    std::vector<std::string> finalOutput;
    for(int i=0; i<asmCode.size(); i++)
    {
        size_t indexOfSemiColon = -1;
        indexOfSemiColon = asmCode[i].find_first_of(';');

        if(indexOfSemiColon < asmCode[i].size())
        {
            finalOutput.push_back( asmCode[i].substr(0, indexOfSemiColon) );
        }
        
        if(!asmCode[i].empty())
        {
            finalOutput.push_back(asmCode[i]);
        }
    }
    return finalOutput;
}

//Not yet finished
std::vector<std::string> Assembler::removeLabels(std::vector<std::string> asmCode)
{
    //record locations of labels
    struct Label
    {
        std::string key = "";
        size_t index = 0;
    };
    
    std::vector<Label> labels;

    std::vector<std::string> finalOutput;
    for(int i=0; i<asmCode.size(); i++)
    {
        size_t indexOfColon = -1;
        indexOfColon = asmCode[i].find_first_of(':');

        if(indexOfColon < asmCode[i].size())
        {
            //probably a label
            std::string labelName = asmCode[i].substr(0, indexOfColon);
            labels.push_back( {labelName, finalOutput.size()} );
        }
        else
        {
            //replace label with index if possible
            for(Label& l : labels)
            {
                size_t lI = asmCode[i].find(l.key);
                if(lI == SIZE_MAX)
                {
                    //no match
                }
                else
                {
                    std::string finalString = asmCode[i].substr(lI);
                    if(asmCode[i][0] == 'J')
                    {
                        //JUMP instruction
                        finalString += StringTools::toHexString((unsigned short)l.index);
                    }
                    else
                    {
                        //BRANCH instruction 
                        finalString += StringTools::toHexString((unsigned char)(finalOutput.size() - l.index));
                    }
                }
            }
            
            finalOutput.push_back(asmCode[i]);
        }
    }



    
    return finalOutput;
}

std::vector<unsigned char> Assembler::convertLine(std::string s)
{
    std::vector<unsigned char> byteCode;

    std::string uppercase = StringTools::toUpper(s);
    std::string opcode = uppercase.substr(0,3);
    std::string inputBytes;
    int indexOfImmediate = -1;
    int indexOfHexStart = -1;
    int indexOfComma = -1;
    int indexOfOpenBracket = -1;
    int indexOfCloseBracket = -1;
    int indexOfX = -1;
    int indexOfY = -1;

    int addV = 0;
    bool hexValue = true;

    for(int i=3; i<uppercase.size(); i++)
    {
        switch (uppercase[i])
        {
        case '#':
            indexOfImmediate = i;
            break;
        case '$':
            indexOfHexStart = i;
            break;
        case ',':
            indexOfComma = i;
            break;
        case '(':
            indexOfOpenBracket = i;
            break;
        case ')':
            indexOfCloseBracket = i;
            break;
        case 'X':
            indexOfX = i;
            break;
        case 'Y':
            indexOfY = i;
            break;
        default:
            break;
        }
    }

    if(opcode == "JMP")
    {
        //special case
        if(indexOfOpenBracket > 0 && indexOfOpenBracket < indexOfHexStart)
        {
            addV = 0x20;
            inputBytes = uppercase.substr(indexOfHexStart+1, 4);
        }
        else
        {
            inputBytes = uppercase.substr(indexOfHexStart+1, 4);
        }
    }
    else
    {
        if(indexOfImmediate > 0)
        {
            addV = 8;
            if(indexOfHexStart < 0)
            {
                hexValue=false;
                inputBytes = uppercase.substr(indexOfImmediate+1);
            }
            else
            {
                inputBytes = uppercase.substr(indexOfHexStart+1);
            }
        }
        else if(indexOfHexStart > 0)
        {
            //some valid thing
            if(indexOfComma > 0)
            {
                if(indexOfOpenBracket > 0)
                {
                    //indirectX, indirectY
                    if(indexOfCloseBracket > 0)
                    {
                        if(indexOfCloseBracket < indexOfComma)
                        {
                            //indirectY
                            inputBytes = uppercase.substr(indexOfHexStart+1,2);
                            addV = 16;
                        }
                        else
                        {
                            //indirectX
                            inputBytes = uppercase.substr(indexOfHexStart+1,2);
                            addV = 0;
                        }
                    }
                    else
                    {
                        //invalid
                        return {};
                    }
                }
                else
                {
                    //zero page or absolute
                    int tempDis = (indexOfHexStart+1) - indexOfComma;
                    inputBytes = uppercase.substr(indexOfHexStart+1, tempDis);
                    if(inputBytes.size() <= 2)
                    {
                        //zero page X
                        addV = 20;
                    }
                    else if(inputBytes.size() <= 4)
                    {
                        if(uppercase.back()=='Y')
                        {
                            //absolute Y
                            addV = 24;
                        }
                        else if(uppercase.back()=='X')
                        {
                            //absolute X
                            addV = 28;
                        }
                        else 
                        {
                            //invalid
                            return {};
                        }
                    }
                    else 
                    {
                        //invalid
                        return {};
                    }
                }
            }
            else
            {
                //zero page or absolute
                inputBytes = uppercase.substr(indexOfHexStart+1);
                if(inputBytes.size() <= 2)
                {
                    //zero page
                    addV = 4;
                }
                else if(inputBytes.size() <= 4)
                {
                    //absolute
                    addV = 12;
                }
                else 
                {
                    //invalid
                    return {};
                }
            }
        }
        else
        {
            //invalid or no inputs
            //assume no inputs
        }
    }

    //set opcode
    if(opcode == "ADC")
        byteCode.push_back( 0x61 + addV);
    else if(opcode == "AND")
        byteCode.push_back( 0x21 + addV);
    else if(opcode == "ASL")
        byteCode.push_back( 0x06 + addV);
    else if(opcode == "BIT")
        byteCode.push_back( 0x24 + addV);
    else if(opcode == "BPL")
        byteCode.push_back( 0x10 );
    else if(opcode == "BMI")
        byteCode.push_back( 0x30 );
    else if(opcode == "BVC")
        byteCode.push_back( 0x50 );
    else if(opcode == "BVS")
        byteCode.push_back( 0x70 );
    else if(opcode == "BCC")
        byteCode.push_back( 0x90 );
    else if(opcode == "BCS")
        byteCode.push_back( 0xB0 );
    else if(opcode == "BNE")
        byteCode.push_back( 0xD0 );
    else if(opcode == "BEQ")
        byteCode.push_back( 0xF0 );
    else if(opcode == "BRK")
        byteCode.push_back( 0x00 );
    else if(opcode == "CMP")
        byteCode.push_back( 0xC1 + addV );
    else if(opcode == "CPX")
        byteCode.push_back( 0xD8 + addV );
    else if(opcode == "CPY")
        byteCode.push_back( 0xB8 + addV );
    else if(opcode == "DEC")
        byteCode.push_back( 0xC2 + addV );
    else if(opcode == "EOR")
        byteCode.push_back( 0x41 + addV );
    else if(opcode == "CLC")
        byteCode.push_back( 0x18 );
    else if(opcode == "SEC")
        byteCode.push_back( 0x38 );
    else if(opcode == "CLI")
        byteCode.push_back( 0x58 );
    else if(opcode == "SEI")
        byteCode.push_back( 0x78 );
    else if(opcode == "CLV")
        byteCode.push_back( 0xB8 );
    else if(opcode == "CLD")
        byteCode.push_back( 0xD8 );
    else if(opcode == "SED")
        byteCode.push_back( 0xF8 );
    else if(opcode == "INC")
        byteCode.push_back( 0xE2 + addV );
    else if(opcode == "JMP")
        byteCode.push_back( 0x4C + addV );
    else if(opcode == "JSR")
        byteCode.push_back( 0x20 );
    else if(opcode == "LDA")
        byteCode.push_back( 0xA1 + addV );
    else if(opcode == "LDX")
        byteCode.push_back( 0xA2 + addV );
    else if(opcode == "LDY")
        byteCode.push_back( 0xA0 + addV );
    else if(opcode == "LSR")
        byteCode.push_back( 0x42 + addV );
    else if(opcode == "NOP")
        byteCode.push_back( 0xEA );
    else if(opcode == "ORA")
        byteCode.push_back( 0x01 + addV );
    else if(opcode == "TAX")
        byteCode.push_back( 0xAA );
    else if(opcode == "TXA")
        byteCode.push_back( 0x8A );
    else if(opcode == "DEX")
        byteCode.push_back( 0xCA );
    else if(opcode == "INX")
        byteCode.push_back( 0xE8 );
    else if(opcode == "TAY")
        byteCode.push_back( 0xA8 );
    else if(opcode == "TYA")
        byteCode.push_back( 0x98 );
    else if(opcode == "DEY")
        byteCode.push_back( 0x88 );
    else if(opcode == "INY")
        byteCode.push_back( 0xC8 );
    else if(opcode == "ROL")
        byteCode.push_back( 0x22 + addV );
    else if(opcode == "ROR")
        byteCode.push_back( 0x62 + addV );
    else if(opcode == "RTI")
        byteCode.push_back( 0x40 );
    else if(opcode == "RTS")
        byteCode.push_back( 0x60 );
    else if(opcode == "SBC")
        byteCode.push_back( 0xE1 + addV );
    else if(opcode == "STA")
        byteCode.push_back( 0x81 + addV );
    else if(opcode == "TXS")
        byteCode.push_back( 0x9A );
    else if(opcode == "TSX")
        byteCode.push_back( 0xBA );
    else if(opcode == "PHA")
        byteCode.push_back( 0x48 );
    else if(opcode == "PLA")
        byteCode.push_back( 0x68 );
    else if(opcode == "PHP")
        byteCode.push_back( 0x08 );
    else if(opcode == "PLP")
        byteCode.push_back( 0x28 );
    else if(opcode == "STX")
        byteCode.push_back( 0x82 + addV );
    else if(opcode == "STY")
        byteCode.push_back( 0x80 + addV );
    else
    {
        //unknown
        byteCode.push_back(0xFF);
    }
    

    //set input bytes if any
    if(inputBytes.size() > 0)
    {
        if(hexValue)
        {
            int finalInput = StringTools::hexStringToInt(inputBytes);
            if(inputBytes.size() == 2)
            {
                byteCode.push_back(finalInput&0xFF);
            }
            else if(inputBytes.size() == 4)
            {
                byteCode.push_back((finalInput>>8)&0xFF);
                byteCode.push_back(finalInput&0xFF);
            }
        }
        else
        {
            int finalInput = std::stoi(inputBytes.c_str());
            byteCode.push_back(finalInput&0xFF);
        }
    }
    
    return byteCode;
}