#pragma once
#include <vector>

class Memory
{
public:
    Memory(int amountOfPools);
    ~Memory();

    unsigned char& getMemory(unsigned short location);
    void setMemory(unsigned short location, unsigned char value);

    void setDataPool(int select);
    int getDataPool();

    int getSize();

    unsigned char* getDataPointer(int dataPool);

private:
    std::vector<unsigned char*> dataPools;
    const int size = 0xFFFF;
    const int pageSize = 0xFF;
    int currentPool = 0;
};