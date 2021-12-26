#include <glib/StringTools.h>
#include <glib/SimpleFile.h>
#include <glib/WndWindow.h>
#include <glib/System.h>
#include <glib/BitmapFont.h>
#include "CPU.h"

bool step = false;
bool stepContinue = false;

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

void setCPUStatus(GuiTextBlock* statusBlock, CPU* cpu)
{
    std::string text;

    std::string b1 = StringTools::formatString("totalCycles: %d\nlastCycles: %d", cpu->getTotalCyclesUsed(), cpu->getCyclesJustUsed());
    std::string b2 = StringTools::formatString("PC: %x   SP: %x", cpu->getPC(), cpu->getSP());
    std::string b3 = StringTools::formatString("A: %x   X: %x   Y: %x", cpu->getA(), cpu->getX(), cpu->getY());
    std::string b4 = StringTools::formatString("Flags: NV-BDIZC");
    std::string b5 = StringTools::formatString("       %s", StringTools::toBinaryString(cpu->getCPUFlags(), 8));

    text = b1; text += '\n';
    text += b2; text += '\n';
    text += b3; text += '\n';
    text += b4; text += '\n';
    text += b5;

    statusBlock->setText(text);
}

void changeMemView(GuiTextBlock* memViewBlock, int scrollValue, Memory* k)
{
    std::string text = "       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n";
    for(int i=scrollValue; i<=0x0F + scrollValue; i++)
    {
        text += '\n';
        text += StringTools::toHexString((unsigned short)(i<<4));
        text += "   ";

        for(int j=0; j<16; j++)
        {
            text += StringTools::toHexString( k->getMemory((i<<4) + j) );
            text += " ";
        }
    }
    memViewBlock->setText(text);
}

void repaintWindow(WndWindow* wnd, GuiTextBlock* statusBlock, CPU* cpu, GuiTextBlock* memBlock, GuiScrollBar* memScroll, Memory* k)
{
    while(wnd->getRunning())
    {
        time_t t1 = System::getCurrentTimeNano();
        setCPUStatus(statusBlock, cpu);
        changeMemView(memBlock, memScroll->getCurrentStep(), k);
        wnd->repaint();
        
        while ((System::getCurrentTimeNano() - t1) < 16666666)
        {
            std::this_thread::yield();
        }
    }
}

void windowTest()
{
    Graphics::init();
    Graphics::setDefaultFont(Graphics::MEDIUM_FONT);
    BitmapFont fnt = BitmapFont("./Resources/EqualizedFont.fnt");
    BitmapFont fnt2 = BitmapFont("./Resources/EqualizedFontMemView.fnt");

    WndWindow wind = WndWindow("6502Emulator", 640, 480);

    #pragma region EMULATION_STUFF
    Memory k = Memory(1);

    loadInstructionsIntoMemory(&k);

    k.setMemory(0xFFFC, 0x00);
    k.setMemory(0xFFFD, 0x06);
    
    CPU cpu = CPU(&k);

    #pragma endregion

    #pragma region BACKGROUND
    GuiRectangleButton background = GuiRectangleButton(0, 0, 640, 480);
    background.setFocusOutlineColor({0,0,255,255});
    background.setBackgroundColor({0,0,255,255});
    background.setOutlineColor({0,0,0,0});
    
    wind.getGuiManager()->addElement(&background);
    #pragma endregion

    #pragma region MEM_VIEW
    GuiTextBlock memBlock = GuiTextBlock(16,32,432,400);
    memBlock.setFont(&fnt2);
    memBlock.setTextColor({255,255,255,255});

    GuiScrollBar memScroll = GuiScrollBar(464-8, 48, 464+8, 336);
    memScroll.setSteps(0xFF0);

    memScroll.setOnSlideFunction([&memBlock, &k](GuiInstance* ins) -> void {
        changeMemView(&memBlock, ((GuiScrollBar*)ins)->getCurrentStep(), &k);
    });

    changeMemView(&memBlock, 0, &k);

    wind.getGuiManager()->addElement(&memBlock);
    wind.getGuiManager()->addElement(&memScroll);
    #pragma endregion

    #pragma region DISASSEMBLY
    GuiTextBlock statusBlock = GuiTextBlock(480,32,640-480,400);
    statusBlock.setTextColor({255,255,255,255});
    statusBlock.setFont(&fnt);

    setCPUStatus(&statusBlock, &cpu);

    wind.getGuiManager()->addElement(&statusBlock);
    #pragma endregion

    #pragma region BUTTONS
    GuiRectangleButton stepButton = GuiRectangleButton(32, 368, 48, 24);
    GuiTextBlock stepTextBlock = GuiTextBlock(2,0,46,24);
    stepTextBlock.setText("STEP");
    stepButton.addChild(&stepTextBlock);

    stepButton.setOnClickFunction( [](GuiInstance* ins) -> void{
        step = true;
    });

    GuiRectangleButton runButton = GuiRectangleButton(96, 368, 48, 24);
    GuiTextBlock runTextBlock = GuiTextBlock(2,0,46,24);
    runTextBlock.setText("RUN");
    runButton.addChild(&runTextBlock);

    runButton.setOnClickFunction( [](GuiInstance* ins) -> void{
        stepContinue = true;
    });

    GuiRectangleButton endButton = GuiRectangleButton(160, 368, 48, 24);
    GuiTextBlock endTextBlock = GuiTextBlock(2,0,46,24);
    endTextBlock.setText("END");
    endButton.addChild(&endTextBlock);

    endButton.setOnClickFunction( [](GuiInstance* ins) -> void{
        step = false;
        stepContinue = false;
    });

    wind.getGuiManager()->addElement(&stepButton);
    wind.getGuiManager()->addElement(&runButton);
    wind.getGuiManager()->addElement(&endButton);
    #pragma endregion

    std::thread paintThread = std::thread(repaintWindow, &wind, &statusBlock, &cpu, &memBlock, &memScroll, &k);

    time_t totalRunTime = 0;
    int timeScale = 1;
    while(wind.getRunning())
    {
        if(stepContinue)
        {
            if(!cpu.getFinished())
                cpu.step();
            
            if(cpu.getFinished())
                stepContinue = false;

            //sleep for (559 * cyclesUsed) * timeScale nanoseconds
            time_t sleepTime = 559 * timeScale * cpu.getCyclesJustUsed();

            time_t t1 = System::getCurrentTimeNano();
            while(true)
            {
                if(System::getCurrentTimeNano() - t1 >= sleepTime)
                {
                    break;
                }
            }

            totalRunTime+=sleepTime;
        }
        else if(step)
        {
            if(!cpu.getFinished())
                cpu.step();

            step = false;
        }
    }

    if(paintThread.joinable())
        paintThread.join();
    
    Graphics::dispose();

    StringTools::println("Total Run Time: %llu", totalRunTime);
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

            system("cls");
        }
    }
}

int main()
{
    StringTools::init();
    //run(true);
    windowTest();
    return 0;
}