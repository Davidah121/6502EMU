#pragma once
#include <vector>

class Memory
{
public:
    Memory(int amountOfPools);
    ~Memory();

    unsigned char& getMemory(unsigned short location);
    void setMemory(unsigned short location, unsigned char value);
    void reset();
    
    void setDataPool(int select);
    int getDataPool();

    int getSize();

    unsigned char* getDataPointer(int dataPool);

private:
    std::vector<unsigned char*> dataPools;
    const int size = 0x10000;
    const int pageSize = 0x100;
    int currentPool = 0;
};