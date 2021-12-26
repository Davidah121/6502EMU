#pragma once
#include<iostream>
#include<vector>
#include<string>

#include<io.h>
#include <stdarg.h>
#include <initializer_list>

class StringTools
{
public:
    static std::vector<std::string> splitString(std::string s, const char delim, bool removeEmpty=true);
	static std::vector<std::string> splitString(std::string s, std::string delim, bool removeEmpty=true);
	static std::vector<std::string> splitStringMultipleDeliminators(std::string s, std::string delim, bool removeEmpty=true);
	
	static std::string getString();
    static std::string toLower(std::string s);
    static std::string toUpper(std::string s);

    static int hexStringToInt(std::string x);
    static char base10ToBase16(char val);
    static char charToBase16(char v);
	static int base16ToBase10(char v);

    template<class T>
	static char* toHexString(T value)
	{
		int size = sizeof(T) * 2;

		char* hexString = new char[size+1];
		int maxVal = 4*sizeof(T) - 4;

		unsigned long long convertedValue = (unsigned long long)value;
		
		for(int i=0; i<size; i++)
		{
			hexString[size-i-1] = base10ToBase16((convertedValue >> (i*4)) & 0xF);
		}

		hexString[size] = '\0';

		return hexString;
	}

    template<class T>
    static char* toBinaryString(T value, int bits, bool LMSB = true)
    {
        int size = sizeof(T);
        char* binString = new char[bits+1];

        char* originalValueAsBytes = (char*)&value;

        for(int i=0; i<bits; i++)
        {
            int byteNum = i/8;
            int bitNum = i%8;

            if(LMSB)
                binString[bits-i-1] = ((originalValueAsBytes[byteNum] >> bitNum) & 0x01) ? '0':'1';
            else
                binString[bits-i-1] = ((originalValueAsBytes[byteNum] >> (7-bitNum)) & 0x01) ? '0':'1';
        }
        binString[bits] = '\0';

        return binString;
    }

    static std::string formatString(std::string text, ...);

    static void print(std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		std::string finalString = formatStringInternal( fmt, args);
		va_end(args);

		std::cout << finalString;
	}

	static void println(std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		std::string finalString = formatStringInternal( fmt, args);
		va_end(args);

		std::cout << finalString << std::endl;
	}

    static void clearConsole(bool clearScrollBuffer);
    static void moveConsoleCursor(int horizontal, int vertical, bool absolute = false);
    static void eraseConsoleLine(bool eraseFromCursor);

private:
	static std::string formatStringInternal(std::string text, va_list orgArgs);
};

inline int StringTools::hexStringToInt(std::string x)
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

inline char StringTools::base10ToBase16(char val)
{
    if(val<10)
        return (char)(48+val);
    else
        return (char)(65+val-10);
}

inline int StringTools::base16ToBase10(char val)
{
    if(val >= '0' && val <= '9')
    {
        return (int)(val-'0');
    }
    else if(val >= 'A' && val <='F')
    {
        return ((int)(val-'A')) + 10;
    }
    else
    {
        return -1;
    }
}

inline std::string StringTools::getString()
{
    std::string temp = "";
    std::getline(std::cin, temp);

    return temp;
}


inline std::string StringTools::toLower(std::string s)
{
    std::string nString;
    for(char& c : s)
    {
        nString += tolower(c);
    }
    return nString;
}

inline std::string StringTools::toUpper(std::string s)
{
    std::string nString;
    for(char& c : s)
    {
        nString += toupper(c);
    }
    return nString;
}

inline std::vector<std::string> StringTools::splitString(std::string s, const char delim, bool removeEmpty)
{
    std::vector<std::string> stringArray = std::vector<std::string>();

    std::string temp = "";

    int i = 0;
    while (i < s.size())
    {
        if (s.at(i) != delim)
        {
            temp += s.at(i);
        }
        else
        {
            if(removeEmpty)
            {
                if(temp!="")
                    stringArray.push_back(temp);
            }
            else
            {
                stringArray.push_back(temp);
            }
            
            temp = "";
        }
        i++;
    }

    stringArray.push_back(temp);

    return stringArray;
}

inline std::vector<std::string> StringTools::splitStringMultipleDeliminators(std::string s, std::string delim, bool removeEmpty)
{
    std::vector<std::string> stringArray = std::vector<std::string>();

    std::string temp = "";

    int i = 0;
    int dSize = delim.size();

    while (i < s.size())
    {
        bool valid = true;

        for(int x=0; x<dSize; x++)
        {
            if (s.at(i) == delim[x])
            {
                valid = false;
            }
        }

        if (valid)
        {
            temp += s.at(i);
        }
        else
        {
            if(removeEmpty)
            {
                if(temp!="")
                    stringArray.push_back(temp);
            }
            else
            {
                stringArray.push_back(temp);
            }
            
            temp = "";
        }
        
        i++;
    }

    stringArray.push_back(temp);

    return stringArray;
}

inline std::vector<std::string> StringTools::splitString(std::string s, std::string delim, bool removeEmpty)
{
    std::vector<std::string> stringArray = std::vector<std::string>();

    std::string temp = "";
    std::string otherString = "";

    int dSize = delim.size();

    int i = 0;
    int count = 0;

    while (i < s.size())
    {
        if (s.at(i) != delim[0])
        {
            temp += s.at(i);
            i++;
        }
        else
        {
            if (dSize + i > s.size())
            {
                //can't contain the substring.
                temp+=s.at(i);
                i++;
                continue;
            }

            while (count < dSize)
            {
                if (s.at(i + count) == delim[count])
                {
                    otherString += delim[count];
                }
                else
                {
                    break;
                }
                count++;
            }

            if (count == dSize)
            {
                //Found subString
                if(removeEmpty)
                {
                    if(temp!="")
                        stringArray.push_back(temp);
                }
                else
                {
                    stringArray.push_back(temp);
                }
                
                temp = "";
                i += count;

                count = 0;
                otherString = "";
            }
            else
            {
                temp += otherString;
                i += count;
                count = 0;
                otherString = "";
            }

        }
    }

    stringArray.push_back(temp);

    return stringArray;
}

inline std::string StringTools::formatStringInternal(std::string text, va_list orgArgs)
{
    std::string finalText = "";
    std::vector<std::string> splits = splitString(text, "%ls", false);

    va_list args;
    va_copy(args, orgArgs);

    int i=0;

    while(i<splits.size())
    {
        std::string str = splits[i];

        int size = vsnprintf(nullptr, 0, str.c_str(), args);
        size++;

        char* nText = new char[size];
        
        vsnprintf(nText, size, str.c_str(), args);
        finalText += nText;

        delete[] nText;
        
        int count = 0;
        size_t loc = 0;
        
        while(true)
        {
            size_t nLoc = str.find('%', loc);
            if(nLoc != SIZE_MAX)
            {
                loc = nLoc;
                
                while(loc < str.size())
                {
                    loc++;
                    //read till flag
                    if(str[loc] == 'd' || str[loc] == 'i' || str[loc] == 'u' || str[loc] == 'o'
                    || str[loc] == 'x' || str[loc] == 'X' || str[loc] == 'c')
                    {
                        va_arg(args, size_t);
                        count++;
                        break;
                    }
                    else if(str[loc] == 'f' || str[loc] == 'F' || str[loc] == 'e' || str[loc] == 'E'
                    || str[loc] == 'g' || str[loc] == 'G' || str[loc] == 'a' || str[loc] == 'A')
                    {
                        va_arg(args, long double);
                        count++;
                        break;
                    }
                    else if(str[loc] == 's')
                    {
                        //should always be char*
                        va_arg(args, char*);
                        count++;
                        break;
                    }
                    else if(str[loc] == 'p')
                    {
                        va_arg(args, void*);
                        count++;
                        break;
                    }
                    else if(str[loc] == 'n')
                    {
                        va_arg(args, void*);
                        count++;
                        break;
                    }
                    else if(str[loc] == L'%')
                    {
                        break;
                    }
                    else if(str[loc] == '*')
                    {
                        va_arg(args, int);
                        count++;
                    }
                }
            }
            else
            {
                break;
            }
        }

        if(i < splits.size()-1)
        {
            std::wstring delayedStr = va_arg(args, wchar_t*);
            // finalText += StringTools::toCString(delayedStr);
            count++;
        }

        i++;
    }

    va_end(args);

    return finalText;
}

inline std::string StringTools::formatString(std::string text, ...)
{
    std::string finalText = "";

    va_list args;
    va_start(args, text);

    finalText = StringTools::formatStringInternal(text, args);

    va_end(args);

    return finalText;
}

inline void StringTools::clearConsole(bool clearScrollBuffer)
{
    if(clearScrollBuffer)
        StringTools::print("\x1B[2J\x1B[3J\x1B[H");
    else
        StringTools::print("\x1B[2J\x1B[H");
}

inline void StringTools::moveConsoleCursor(int horizontal, int vertical, bool absolute)
{
    if(absolute)
    {
        int realHVal = (horizontal>=1) ? horizontal : 1;
        int realVVal = (vertical>=1) ? vertical : 1;
        
        StringTools::print("\x1B[%d;%dH", realVVal, realHVal);
    }
    else
    {
        if(horizontal>0)
            StringTools::print("\x1B[%dC", horizontal);
        else if(horizontal<0)
            StringTools::print("\x1B[%dD", std::abs(horizontal));

        if(vertical>0)
            StringTools::print("\x1B[%dB", vertical);
        else if(vertical<0)
            StringTools::print("\x1B[%dA", std::abs(vertical));
    }
}

inline void StringTools::eraseConsoleLine(bool eraseFromCursor)
{
    if(eraseFromCursor)
        StringTools::print("\x1B[K");
    else
        StringTools::print("\x1B[2K\r");
}