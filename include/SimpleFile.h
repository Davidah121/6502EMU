#pragma once
#include<fstream>
#include<vector>

class SimpleFile
{
public:
    static const bool READ = false;
    static const bool WRITE = true;
    
    SimpleFile(std::string filename, bool type);
    ~SimpleFile();

    std::vector<unsigned char> readAllBytes();
    std::vector<std::string> readAllStrings();
    void writeBytes(void* data, size_t totalSize);
    void writeByte(unsigned char c);
    void writeByte(char c);
    bool isOpen();
    void close();
private:
    bool type = SimpleFile::READ;
    std::fstream file;
};

inline SimpleFile::SimpleFile(std::string filename, bool type)
{
    this->type = type;
    if(type == READ)
        file = std::fstream(filename, std::fstream::in | std::fstream::binary);
    else
        file = std::fstream(filename, std::fstream::out | std::fstream::binary);
}

inline SimpleFile::~SimpleFile()
{
    file.close();
}

inline std::vector<unsigned char> SimpleFile::readAllBytes()
{
    if(type == WRITE)
    {
        return {};
    }
    std::vector<unsigned char> bytes;

    while(true)
    {
        unsigned char c = file.get();
        if(file.eof())
            break;
        
        bytes.push_back(c);
    }

    return bytes;
}

inline std::vector<std::string> SimpleFile::readAllStrings()
{
    if(type == WRITE)
    {
        return {};
    }
    std::vector<std::string> lines;
    std::string accum;

    while(true)
    {
        unsigned char c = file.get();
        if(file.eof())
            break;
        
        if(c >= 32)
        {
            accum += c;
        }
        else if(c == '\n')
        {
            lines.push_back(accum);
            accum = "";
        }
    }

    if(accum != "")
    {
        lines.push_back(accum);
    }
    
    return lines;
}

inline void SimpleFile::writeBytes(void* data, size_t totalSize)
{
    if(type == WRITE)
    {
        file.write((char*)data, totalSize);
    }
}


inline void SimpleFile::writeByte(unsigned char c)
{
    if(type == WRITE)
    {
        file << c;
    }
}
inline void SimpleFile::writeByte(char c)
{
    if(type == WRITE)
    {
        file << c;
    }
}

inline bool SimpleFile::isOpen()
{
    return file.is_open();
}

inline void SimpleFile::close()
{
    file.close();
}
