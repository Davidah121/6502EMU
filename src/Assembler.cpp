#include "Assembler.h"
#include <unordered_map>

Assembler::Assembler()
{
}

Assembler::~Assembler()
{
}

std::vector<std::string> Assembler::convertToAsm(std::vector<unsigned char> bytes, int startLocation)
{
    struct TempStruct
    {
        std::string asmString;
        int bytesUsed = 0;
    };
    
    std::vector<TempStruct> output;
    std::vector<std::string> finalOutput;

    std::unordered_map<int, int> jmpLocations;

    size_t labels = 0;
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
            asmString = StringTools::formatString(info.syntax, b2, b1);
        }

        unsigned short hiByte = (index+startLocation) >> 8;
        unsigned char loByte = (index+startLocation) & 0xFF;
        unsigned short finalLocation = index+startLocation+2;

        switch (instruction)
        {
        case 0x10:
            finalLocation += (char)bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "BPL L" + std::to_string( labels );
            labels++;
            break;
        case 0x30:
            finalLocation += (char)bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "BMI L" + std::to_string( labels );
            labels++;
            break;
        case 0x50:
            finalLocation += (char)bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "BVC L" + std::to_string( labels );
            labels++;
            break;
        case 0x70:
            finalLocation += (char)bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "BVS L" + std::to_string( labels );
            labels++;
            break;
        case 0x90:
            finalLocation += (char)bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "BCC L" + std::to_string( labels );
            labels++;
            break;
        case 0xB0:
            finalLocation += (char)bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "BCS L" + std::to_string( labels );
            labels++;
            break;
        case 0xD0:
            finalLocation += (char)bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "BNE L" + std::to_string( labels );
            labels++;
            break;
        case 0xF0:
            finalLocation += (char)bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "BEQ L" + std::to_string( labels );
            labels++;
            break;
        case 0x4C:
            finalLocation = bytes[index+2];
            finalLocation = (finalLocation<<8) + bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "JMP L" + std::to_string( labels );
            labels++;
            break;
        case 0x20:
            finalLocation = bytes[index+2];
            finalLocation = (finalLocation<<8) + bytes[index+1];
            jmpLocations[finalLocation] = labels;

            asmString = "JSR L" + std::to_string( labels );
            labels++;
            break;
        default:
            break;
        }

        output.push_back( {asmString, 1+info.additionalBytes} );

        index+=1+info.additionalBytes;
    }

    //add the labels to the final asm output
    size_t numBytes = startLocation;
    for(int i=0; i<output.size(); i++)
    {
        auto it = jmpLocations.find(numBytes);

        if(it != jmpLocations.end())
        {
            //add label
            finalOutput.push_back( "L" + std::to_string(it->second) + ":" );
        }
        
        finalOutput.push_back(output[i].asmString);
        numBytes += output[i].bytesUsed;
    }

    auto it = jmpLocations.find(numBytes);

    if(it != jmpLocations.end())
    {
        //add label
        finalOutput.push_back( "L" + std::to_string(it->second) + ":" );
    }

    return finalOutput;
}

std::vector<unsigned char> Assembler::convertToBinary(std::vector<std::string> asmCode, std::string& errorValue, int startLocation)
{
    std::vector<unsigned char> output;

    std::vector<std::string> parsedInput = removeCommentsAndEmptyLines(asmCode);
    parsedInput = removeLabels(parsedInput, startLocation);
    
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

        std::string temp = asmCode[i];

        if(indexOfSemiColon < asmCode[i].size())
        {
            temp = asmCode[i].substr(0, indexOfSemiColon);
        }
        
        //remove excess spaces
        std::string parsedString = "";
        for(int i=0; i<temp.size(); i++)
        {
            if(temp[i] > 32)
                parsedString += temp[i];
            else if(temp[i] == 32)
            {
                if(i > 0)
                {
                    if(temp[i-1] != ' ')
                    {
                        parsedString += ' ';
                    }
                }
            }
        }

        if(!parsedString.empty())
        {
            finalOutput.push_back(parsedString);
        }
    }
    return finalOutput;
}

//Not yet finished
std::vector<std::string> Assembler::removeLabels(std::vector<std::string> asmCode, int startLocation)
{
    //record locations of labels
    std::unordered_map<std::string, int> labels;

    std::vector<int> byteLocation;

    std::vector<std::string> finalOutput;
    int currByteLocation = startLocation;

    for(int i=0; i<asmCode.size(); i++)
    {
        size_t indexOfColon = -1;
        indexOfColon = asmCode[i].find_first_of(':');

        if(indexOfColon < asmCode[i].size())
        {
            std::string labelName = asmCode[i].substr(0, indexOfColon);
            labels.insert( std::make_pair(labelName, currByteLocation) );
        }
        else
        {
            std::vector<unsigned char> byteValues = convertLine( asmCode[i] );
            InstructionInfo info = CPU::OpCodes[byteValues[0]];
            
            //check if invalid
            if(info.additionalBytes == 0 && info.cycles == 0)
            {
                //invalid
                break;
            }
            
            currByteLocation += info.additionalBytes+1;
        }
        byteLocation.push_back(currByteLocation);
    }

    currByteLocation = startLocation;
    for(int i=0; i<asmCode.size(); i++)
    {
        size_t indexOfColon = -1;
        indexOfColon = asmCode[i].find_first_of(':');

        if(indexOfColon < asmCode[i].size())
        {
            //probably a label
        }
        else
        {
            //replace label with index if possible
            std::string finalString = asmCode[i];

            if(finalString[0] == 'J')
            {
                //probably a jmp or jsr
                std::vector<std::string> split = StringTools::splitString(finalString, ' ');

                if(split.size() > 1)
                {
                    auto it = labels.find(split[1]);
                    if(it != labels.end())
                    {
                        finalString = split[0] + " $";
                        finalString += StringTools::toHexString((unsigned short)it->second);
                    }
                }

            }
            else if(finalString[0] == 'B')
            {
                //probably a branch
                std::vector<std::string> split = StringTools::splitString(finalString, ' ');

                if(split.size() > 1)
                {
                    auto it = labels.find(split[1]);
                    if(it != labels.end())
                    {
                        finalString = split[0] + " $";
                        short moveValue = it->second - byteLocation[i];
                        finalString += StringTools::toHexString((unsigned char)moveValue);
                    }
                }

            }

            finalOutput.push_back(finalString);
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

    enum INSTRUCTION_TYPE{
        IndirectX = 0,
        ZeroPage = 4,
        Immediate = 8,
        Absolute = 12,
        IndirectY = 16,
        ZeroPageX = 20,
        AbsoluteY = 24,
        AbsoluteX = 28
    };

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
            addV = Immediate;
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
                            addV = IndirectY;
                        }
                        else
                        {
                            //indirectX
                            inputBytes = uppercase.substr(indexOfHexStart+1,2);
                            addV = IndirectX;
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
                    int tempDis = indexOfComma - (indexOfHexStart+1);
                    inputBytes = uppercase.substr(indexOfHexStart+1, tempDis);
                    if(inputBytes.size() <= 2)
                    {
                        //zero page X
                        addV = ZeroPageX;
                    }
                    else if(inputBytes.size() <= 4)
                    {
                        if(uppercase.back()=='Y')
                        {
                            //absolute Y
                            addV = AbsoluteY;
                        }
                        else if(uppercase.back()=='X')
                        {
                            //absolute X
                            addV = AbsoluteX;
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
                    addV = ZeroPage;
                }
                else if(inputBytes.size() <= 4)
                {
                    //absolute
                    addV = Absolute;
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
        byteCode.push_back( 0x02 + addV);
    else if(opcode == "BIT")
        byteCode.push_back( 0x20 + addV);
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
        byteCode.push_back( 0x4C + addV ); //special case: addV will be 0x00 or 0x20
    else if(opcode == "JSR")
        byteCode.push_back( 0x20 );
    else if(opcode == "LDA")
        byteCode.push_back( 0xA1 + addV );
    else if(opcode == "LDX")//special
    {
        switch (addV)
        {
        case Immediate:
            byteCode.push_back(0xA2);
            break;
        case ZeroPage:
            byteCode.push_back(0xA6);
            break;
        case Absolute:
            byteCode.push_back(0xAE);
            break;
        case AbsoluteY:
            byteCode.push_back(0xBE);
            break;
        default:
            byteCode.push_back(0xB6); //ASSUME ZeroPageY
            break;
        }
    }
    else if(opcode == "LDY")
    {
        switch (addV)
        {
        case Immediate:
            byteCode.push_back(0xA0);
            break;
        case ZeroPage:
            byteCode.push_back(0xA4);
            break;
        case ZeroPageX:
            byteCode.push_back(0xB4);
            break;
        case Absolute:
            byteCode.push_back(0xAC);
            break;
        case AbsoluteX:
            byteCode.push_back(0xBC);
            break;
        default:
            break;
        }
    }
    else if(opcode == "LSR")
    {
        switch (addV)
        {
        case ZeroPage:
            byteCode.push_back(0x46);
            break;
        case ZeroPageX:
            byteCode.push_back(0x56);
            break;
        case Absolute:
            byteCode.push_back(0x4E);
            break;
        case AbsoluteX:
            byteCode.push_back(0x5E);
            break;
        default:
            byteCode.push_back(0x4A);
            break;
        }
    }
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
    {
        switch (addV)
        {
        case ZeroPage:
            byteCode.push_back(0x26);
            break;
        case ZeroPageX:
            byteCode.push_back(0x36);
            break;
        case Absolute:
            byteCode.push_back(0x2E);
            break;
        case AbsoluteX:
            byteCode.push_back(0x3E);
            break;
        default:
            byteCode.push_back(0x2A);
            break;
        }
    }
    else if(opcode == "ROR")
    {
        switch (addV)
        {
        case ZeroPage:
            byteCode.push_back(0x66);
            break;
        case ZeroPageX:
            byteCode.push_back(0x76);
            break;
        case Absolute:
            byteCode.push_back(0x6E);
            break;
        case AbsoluteX:
            byteCode.push_back(0x7E);
            break;
        default:
            byteCode.push_back(0x6A);
            break;
        }
    }
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
    {
        switch (addV)
        {
        case ZeroPage:
            byteCode.push_back(0x86);
            break;
        case Absolute:
            byteCode.push_back(0x8E);
            break;
        default:
            byteCode.push_back(0x96); // ZeroPageY
            break;
        }
    }
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
                byteCode.push_back(finalInput&0xFF);
                byteCode.push_back((finalInput>>8)&0xFF);
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