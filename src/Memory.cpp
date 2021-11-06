#include "Memory.h"

Memory::Memory(int amountOfPools)
{
    dataPools = std::vector<unsigned char*>(amountOfPools);

    for(int i=0; i<amountOfPools; i++)
    {
        dataPools[i] = new unsigned char[size];
        std::memset(dataPools[i], 0, size);
    }
}

Memory::~Memory()
{
    for(int i=0; i<dataPools.size(); i++)
    {
        delete[] dataPools[i];
        dataPools[i] = nullptr;
    }

    dataPools.clear();
}

void Memory::reset()
{
    for(int i=0; i<dataPools.size(); i++)
    {
        dataPools[i] = new unsigned char[size];
        std::memset(dataPools[i], 0, size);
    }
}

unsigned char& Memory::getMemory(unsigned short location)
{
    return dataPools[currentPool][location];
}

void Memory::setMemory(unsigned short location, unsigned char value)
{
    dataPools[currentPool][location] = value;
}

unsigned char* Memory::getDataPointer(int pool)
{
    return dataPools[pool];
}

void Memory::setDataPool(int pool)
{
    currentPool = pool;
}

int Memory::getDataPool()
{
    return currentPool;
}

int Memory::getSize()
{
    return size;
}