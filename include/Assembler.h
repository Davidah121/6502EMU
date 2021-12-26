#pragma once
#include "StringTools.h"
#include "SimpleFile.h"
#include "CPU.h"
class Assembler
{
public:
    Assembler();
    ~Assembler();

    static std::vector<std::string> convertToAsm(std::vector<unsigned char> bytes, int startLocation = 0x0600);
    static std::vector<unsigned char> convertToBinary(std::vector<std::string> asmCode, std::string& errorMessage, int startLocation = 0x0600);
private:
    static std::vector<unsigned char> convertLine(std::string s);
    static std::vector<std::string> removeCommentsAndEmptyLines(std::vector<std::string> asmCode);

    
    //Not yet finished
    static std::vector<std::string> removeLabels(std::vector<std::string> asmCode, int startLocation = 0x0600);
};

