#include "StringTools.h"
#include "SimpleFile.h"
#include "CPU.h"
#include "Assembler.h"

std::string inputInstructions = "6502.a";

void assembleCode(std::string inputFile, std::string outputFile)
{
    std::vector<std::string> instructions;

    SimpleFile f = SimpleFile(inputFile, SimpleFile::READ);
    if(f.isOpen())
    {
        instructions = f.readAllStrings();
        f.close();
    }

    std::string errorMessage = "";
    std::vector<unsigned char> outputBinary = Assembler::convertToBinary(instructions, errorMessage);

    if(errorMessage != "")
    {
        StringTools::println("ERROR HAS OCCURED.");
        StringTools::println(errorMessage);
    }
    else
    {
        SimpleFile f2 = SimpleFile(outputFile, SimpleFile::WRITE);
        if(f2.isOpen())
        {
            f2.writeBytes(outputBinary.data(), outputBinary.size());
            f2.close();
        }
    }
}

void disassembleCode(std::string inputFile, std::string outputFile)
{
    std::vector<unsigned char> binaryCode;

    SimpleFile f = SimpleFile(inputFile, SimpleFile::READ);
    if(f.isOpen())
    {
        binaryCode = f.readAllBytes();
        f.close();
    }

    std::vector<std::string> outputBinary = Assembler::convertToAsm(binaryCode);

    SimpleFile f2 = SimpleFile(outputFile, SimpleFile::WRITE);
    if(f2.isOpen())
    {
        for(int i=0; i<outputBinary.size(); i++)
        {
            f2.writeBytes(outputBinary[i].data(), outputBinary[i].size());
            f2.writeByte('\n');
        }
        f2.close();
    }
}

bool loadInstructionsIntoMemory(Memory* mem)
{
    int location = 0x600;
    std::vector<unsigned char> instructions;

    SimpleFile f = SimpleFile(inputInstructions, SimpleFile::READ);
    if(f.isOpen())
    {
        instructions = f.readAllBytes();
        f.close();
    }
    else
    {
        StringTools::println("ERROR LOADING INSTRUCTIONS from file %s", inputInstructions.c_str());
        return false;
    }

    for(unsigned char a : instructions)
    {
        mem->setMemory(location, a);
        location++;
    }
    return true;
}

void mem_dump(Memory* mem)
{
    SimpleFile f = SimpleFile("mem_dump", SimpleFile::WRITE);
    if(f.isOpen())
    {
        f.writeBytes(mem->getDataPointer(0), mem->getSize());
        f.close();
    }
}

void mem_view(Memory* mem)
{
    StringTools::println("Enter a single location or range separated by a dash in hexadecimal.");
    std::string ranges = StringTools::getString();

    std::vector<std::string> values = StringTools::splitString(ranges, '-');

    int location1 = 0;
    int location2 = 0;

    if(values.size()==1)
    {
        location1 = StringTools::hexStringToInt(values[0]);
        location2 = location1;
    }
    else if(values.size()==2)
    {
        location1 = StringTools::hexStringToInt(values[0]);
        location2 = StringTools::hexStringToInt(values[1]);

        StringTools::println("%d, %d", location1, location2);
    }
    else
    {
        StringTools::println("Invalid number of arguments");
        return;
    }

    int count = 0;
    StringTools::println("\t 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    
    std::string currLine = "";
    int offset = location1 % 16;

    unsigned short startLoc = location1 - offset;
    currLine += StringTools::toHexString( (unsigned short)((startLoc+1) & 0xFFF0) );
    currLine += "\t ";

    for(int i=0; i<offset; i++)
    {
        currLine += "  ";
    }
    
    count = offset;

    for(int i=location1; i<=location2; i++)
    {
        currLine += StringTools::toHexString(mem->getMemory(i));
        currLine += " ";
        count++;

        if(count%16 == 0 && i!=location2)
        {
            StringTools::println(currLine);

            currLine = "";

            currLine += StringTools::toHexString((unsigned short) ((i+1) & 0xFFF0) );
            currLine += "\t ";
        }
    }

    if(currLine!="")
    {
        StringTools::println(currLine);
        currLine = "";
    }
}

void run()
{
    Memory k = Memory(1);

    bool valid = loadInstructionsIntoMemory(&k);

    k.setMemory(0xFFFC, 0x00);
    k.setMemory(0xFFFD, 0x06);
    
    CPU cpu = CPU(&k);

    if(!valid)
    {
        return;
    }


    while(true)
    {
        StringTools::println("totalCycles: %d \t lastCycles: %d", cpu.getTotalCyclesUsed(), cpu.getCyclesJustUsed());
        StringTools::println("PC: %x \t SP: %x", cpu.getPC(), cpu.getSP());
        StringTools::println("A: %x \t X: %x \t Y: %x", cpu.getA(), cpu.getX(), cpu.getY());
        StringTools::println("Flags: NV-BDIZC");
        StringTools::println("       %s", StringTools::toBinaryString(cpu.getCPUFlags(), 8));

        StringTools::println("");
        StringTools::println("Next Instruction: %s", cpu.getNextInstructionAsString().c_str());
        
        StringTools::print("Enter Commands: ");
        std::string command = StringTools::getString();

        if(command == "help")
        {
            system("start help.txt");
        }
        else if(command == "step")
        {
            if(!cpu.getFinished())
                cpu.step();
            else
                StringTools::println("Program can no longer be executed.");
        }
        else if(command == "run")
        {
            while(!cpu.getFinished())
            {
                cpu.step();
            }
            StringTools::println("Program finished execution");
            system("pause");
        }
        else if(command == "end")
        {
            StringTools::println("Ending Execution");
            break;
        }
        else if(command == "mem_dump")
        {
            StringTools::println("Dumping memory to file mem_dump");
            mem_dump(&k);
            system("pause");
        }
        else if(command == "mem_view")
        {
            mem_view(&k);
            system("pause");
        }
        else if(command == "irq")
        {
            cpu.requestInterupt();
        }
        else if(command == "nmi")
        {
            cpu.requestManditoryInterupt();
        }
        else if(command == "reset")
        {
            cpu.requestReset();
        }
        else if(command == "hard_reset")
        {
            k.reset();
            loadInstructionsIntoMemory(&k);
            cpu.hardReset();
        }

        StringTools::clearConsole(true);
    }
    
}

int main(int argc, char** argv)
{
    if(argc > 1)
    {
        std::string inputFile = "";
        std::string outputFile = "";
        int type = 0;
        int index = 1;

        while(index < argc)
        {
            std::string arg = argv[index];
            if(arg == "-i")
            {
                inputFile = argv[index+1];
                index+=2;
            }
            else if(arg == "-o")
            {
                outputFile = argv[index+1];
                index+=2;
            }
            else if(arg == "-R")
            {
                type = 0;
                index++;
            }
            else if(arg == "-A")
            {
                type = 1;
                index++;
            }
            else if(arg == "-D")
            {
                type = 2;
                index++;
            }
            else
            {
                index++;
            }
        }

        if(type == 0)
        {
            //run a program
            if(inputFile != "")
            {
                inputInstructions = inputFile;
            }
            run();
        }
        else
        {
            if(inputFile!="" && outputFile!="")
            {
                if(type == 1)
                {
                    assembleCode(inputFile, outputFile);
                }
                else if(type == 2)
                {
                    disassembleCode(inputFile, outputFile);
                }
            }
            else
            {
                StringTools::println("Must specify input and output file to assemble or disassemble code");
            }
        }
    }
    else
    {
        //attempt to run with default settings
        run();
    }
    return 0;
}