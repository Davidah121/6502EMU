#include <glib/StringTools.h>
#include <glib/SimpleFile.h>
#include "CPU.h"

int hexStringToInt(std::string x)
{
    int value = 0;
    for(unsigned char a : x)
    {
        value = value << 4;
        if(a>='0' && a<='9')
        {
            value |= a-'0';
        }
        else if(a >= 'A' && a <= 'F')
        {
            value |= (a-'A')+10;
        }
        else if(a >= 'a' && a <= 'f')
        {
            value |= (a-'a')+10;
        }
        else
        {
            break;
        }
    }

    return value;
}

void loadInstructionsIntoMemory(Memory* mem)
{
    int location = 0x600;
    std::vector<unsigned char> instructions;

    SimpleFile f = SimpleFile("6502.a", SimpleFile::ASCII | SimpleFile::READ);
    if(f.isOpen())
    {
        instructions = f.readFullFileAsBytes();
        f.close();
    }

    for(unsigned char a : instructions)
    {
        mem->setMemory(location, a);
        location++;
    }
}

void mem_dump(Memory* mem)
{
    SimpleFile f = SimpleFile("mem_dump", SimpleFile::ASCII | SimpleFile::WRITE);
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
        location1 = hexStringToInt(values[0]);
        location2 = location1;
    }
    else if(values.size()==2)
    {
        location1 = hexStringToInt(values[0]);
        location2 = hexStringToInt(values[1]);

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

void run(bool debugMode)
{
    Memory k = Memory(1);

    loadInstructionsIntoMemory(&k);

    k.setMemory(0xFFFC, 0x00);
    k.setMemory(0xFFFD, 0x06);
    
    CPU cpu = CPU(&k);

    if(!debugMode)
    {
        while(!cpu.getFinished())
        {
            cpu.step();
        }
    }
    else
    {
        while(true)
        {
            StringTools::println("totalCycles: %d \t lastCycles: %d", cpu.getTotalCyclesUsed(), cpu.getCyclesJustUsed());
            StringTools::println("PC: %x \t SP: %x", cpu.getPC(), cpu.getSP());
            StringTools::println("A: %x \t X: %x \t Y: %x", cpu.getA(), cpu.getX(), cpu.getY());
            StringTools::println("Flags: NV-BDIZC");
            StringTools::println("       %s", StringTools::toBinaryString(cpu.getCPUFlags(), 8));
            
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

            system("cls");
        }
    }
}

int main()
{
    StringTools::init();
    run(true);
    return 0;
}