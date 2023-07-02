#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <windows.h>
#include <thread>
#include <time.h>
#include "Console.h"

inline double power(double base, unsigned int exponent)
{
    double return_value = 0;
    if (base != 0)
    {
        return_value = 1;
    }
    for (int i = 0; i < exponent; i++)
    {
        return_value *= base;
    }
    return return_value;
}

inline std::string NumToStr(unsigned long long num, unsigned long long base = 10, unsigned int min_digits = 1)
{
    std::string str = "";

    unsigned int count_digits = 0;
    // FFFFFFFF
    while (power(base, count_digits) <= num) count_digits++;

    unsigned int size = count_digits;
    if (size < min_digits) size = min_digits;

    for (int i = 0; i < size - count_digits; i++) str += '0';
    for (int i = count_digits - 1; i >= 0; i--)
    {
        // 5248 % 10 = 8 -> 8 / 1 = 8
        // 5248 % 100 = 48 -> 48 / 10 = 4
        // 5248 % 1000 = 248 -> 248 / 100 = 2
        // 5248 % 10000 = 5248 -> 5248 / 1000 = 5
        unsigned int digit = (num % (unsigned long long)power(base, i + 1)) / power(base, i);

        if (digit < 10) str += (unsigned char)((unsigned int)'0' + digit);
        else str += (unsigned char)((unsigned int)'A' + (digit - 10));
    }
    return str;
}

inline std::string HexToStr(unsigned long long hex, unsigned int digits = 2)
{
    return "0x" + NumToStr(hex, 16, digits);
}





inline std::wstring toWide(std::string str)
{
    std::wstring wide;
    for (int i = 0; i < str.size(); i++)
    {
        wide += (wchar_t)str[i];
    }
    return wide;
}

inline std::string fromWide(std::wstring wstr)
{
    std::string str;
    for (int i = 0; i < wstr.size(); i++)
    {
        str += (unsigned char)wstr[i];
    }
    return str;
}






inline void MessageDialog(std::wstring msg, std::wstring title = L"")
{
    if (!msg.empty())
    {
        if (msg.back() == L'\n') msg.pop_back();
    }

    MessageBoxW(NULL, msg.c_str(), title.c_str(), MB_OK);
}

inline void MessageDialog(std::string msg, std::string title = "")
{
    MessageDialog(toWide(msg), toWide(title));
}



inline std::string FuncErrOutput(std::string function)
{
    std::string output = "Failed to invoke " + function + "()";
    return output;
}

inline std::wstring FuncErrOutput(std::wstring function)
{
    std::wstring output = L"Failed to invoke " + function + L"()";
    return output;
}

inline std::string FuncErrOutput(std::string function, unsigned long external_code)
{
    std::string output = FuncErrOutput(function);
    output += ": " + NumToStr(external_code) + " (" + HexToStr(external_code) + ")";

    return output;
}

inline std::wstring FuncErrOutput(std::wstring function, unsigned long external_code)
{
    std::wstring output = FuncErrOutput(function);
    output += L": " + toWide(NumToStr(external_code)) + L" (" + toWide(HexToStr(external_code)) + L")";

    return output;
}

inline void ErrFuncExit(std::string function)
{
    std::string output = FuncErrOutput(function);

    MessageDialog(output);
    ExitProcess(0x01);
}

inline void ErrFuncExitCode(std::string function, unsigned long external_code)
{
    std::string output = FuncErrOutput(function, external_code);

    MessageDialog(output);
    ExitProcess(0x01);
}

inline void ErrFuncExit(std::wstring function)
{
    std::wstring output = FuncErrOutput(function);

    MessageDialog(output);
    ExitProcess(0x01);
}

inline void ErrFuncExitCode(std::wstring function, unsigned long external_code)
{
    std::wstring output = FuncErrOutput(function, external_code);

    MessageDialog(output);
    ExitProcess(0x01);
}


inline void ErrMsgExit(std::string msg)
{
    MessageDialog(msg);
    ExitProcess(0x01);
}

inline void ErrMsgExitCode(std::string msg, unsigned long external_code)
{
    MessageDialog(msg);
    ExitProcess(0x01);
}

inline void ErrMsgExit(std::wstring msg)
{
    MessageDialog(msg);
    ExitProcess(0x01);
}

inline void ErrMsgExitCode(std::wstring msg, unsigned long external_code)
{
    MessageDialog(msg);
    ExitProcess(0x01);
}













inline std::string EncodeToUTF8(std::wstring str)
{
    std::string encoded = "";
    for (int i = 0; i < str.size(); i++)
    {
        wchar_t character = str[i];
        if (character <= 0x7F) encoded += (unsigned char)character;
        else
        {
            if (character >= 0x80 && character <= 0x7FF)
            {
                encoded += (unsigned char)(((0xC0 << 8) + ((character << 2) & (0x1F << 8))) >> 8);
                encoded += (unsigned char)(((0x80) + (character & 0x3F)));
            }
            else if (character >= 0x800 && character <= 0xFFFF)
            {
                encoded += (unsigned char)(((0xE0 << 16) + ((character << 4) & (0x0F << 16))) >> 16);
                encoded += (unsigned char)(((0x80 << 8) + ((character << 2) & (0x3F << 8))) >> 8);
                encoded += (unsigned char)(((0x80) + (character & 0x3F)));
            }
            else if (character >= 0x10000)  // abstract (this kind of value would not fit into wchar)
            {
                // wchar_t is 16-bit on Windows, 0x10000+ code points are not supported
            }
        }
    }
    return encoded;
}

inline std::wstring DecodeFromUTF8(std::string str)
{
    std::wstring decoded = L"";
    for (int i = 0; i < str.size(); i++)
    {
        wchar_t currentChar = (unsigned char)str[i];
        if ((currentChar & 128) == 128 && (currentChar & 64) == 64)
        {
            if ((currentChar & 32) == 0 && i + 1 < str.size())
            {
                // 2-Byte encoded character
                // 110xxxxx ...
                unsigned char Byte[] ={ (unsigned char)currentChar & 31, str[i + 1] & 63 };
                i++;

                // 16-bit decoded character:
                currentChar = (Byte[1]) + (((unsigned short)Byte[0] << 8) >> 2);
            }
            else if ((currentChar & 32) == 32)
            {
                if ((currentChar & 16) == 0 && i + 2 < str.size())
                {
                    // 3-Byte encoded character
                    // 1110xxxx ...
                    unsigned char Byte[] ={ (unsigned char)currentChar & 15, str[i + 1] & 63, str[i + 2] & 63 };
                    i++;
                    i++;

                    // 16-bit decoded character:
                    currentChar = (Byte[2]) + (((unsigned short)Byte[1] << 8) >> 2) + (((unsigned int)Byte[0] << 16) >> 4);
                }
                else if ((currentChar & 16) == 16 && i + 3 < str.size())
                {
                    if ((currentChar & 8) == 0)
                    {
                        //4-Byte encoded character
                        // 11110xxx ...
                        for (int i = 0; i < 3; i++) i++;

                        // 16-bit substitue character:
                        currentChar = 'e';
                    }
                }
            }
        }

        decoded += currentChar;
    }
    return decoded;
}

inline std::wstring GetWideFromFixedUnicode16(const char* raw)
{
    std::wstring raw_input = L"";
    for (int i = 0; raw[i] != '\0'; i += 2)
    {
        raw_input.push_back(0x00);
        raw_input.back() += (unsigned char)raw[i];
        raw_input.back() += (unsigned short)(raw[i + 1] << 8);
    }
    return raw_input;
}

inline std::wstring GetWideFromFixedUnicode8(const char* raw)
{
    std::wstring raw_input = L"";
    for (int i = 0; raw[i] != '\0'; i++)
    {
        unsigned char currentByte = (unsigned char)raw[i];
        raw_input.push_back((wchar_t)currentByte);
    }
    return raw_input;
}





inline int GetFileData(const wchar_t* filename, std::string* returnData = nullptr)
{
    std::string data = "";

    FILE* file;
    _wfopen_s(&file, filename, L"rb");
    if (file == nullptr)
    {
        ErrMsgExit(L"Cannot open: " + std::wstring(filename) + L'\n');
        return 1;
    }

    int currentChar;
    do
    {
        currentChar = getc(file);
        if (currentChar != EOF) data += (unsigned char)currentChar;
    } while (currentChar != EOF);
    fclose(file);

    if (returnData != nullptr) *returnData = data;
    return 0;
}

inline int WriteDataToFile(std::string data, const wchar_t* filename)
{
    FILE* file;
    _wfopen_s(&file, filename, L"wb");
    if (file == nullptr)
    {
        ErrMsgExit(L"Cannot open: " + std::wstring(filename) + L'\n');
        return 1;
    }

    for (int i = 0; i < data.size(); i++)
    {
        putc((unsigned char)data[i], file);
    }
    fclose(file);

    return 0;
}

inline int AppendDataToFile(std::string data, const wchar_t* filename)
{
    FILE* file;
    _wfopen_s(&file, filename, L"a+b");
    if (file == nullptr)
    {
        ErrMsgExit(L"Cannot open: " + std::wstring(filename) + L"\n");
        return 1;
    }

    for (int i = 0; i < data.size(); i++)
    {
        putc((unsigned char)data[i], file);
    }
    fclose(file);

    return 0;
}


inline bool isStrNum(std::wstring str)
{
    for (auto symbol : str)
    {
        if (!iswdigit(symbol))
        {
            return false;
        }
    }
    return true;
}



inline bool findInStr(std::wstring str, std::wstring query)
{
    unsigned int index = 0;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == query[index])
        {
            index++;
            if (index == query.size())
            {
                return true;
            }
        }
        else
        {
            index = 0;
        }
    }
    return false;
}

inline int whereSubStr(std::wstring str, std::wstring query)
{
    unsigned int index = 0;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == query[index])
        {
            if (index + 1 == query.size())
            {
                return i - index;
            }
            index++;
        }
        else index = 0;
    }
    return -1;
}

inline void replaceSubStr(std::wstring& str, std::wstring query, std::wstring replacement)
{
    int start = whereSubStr(str, query);
    std::wstring copy = L"";
    for (int i = 0; i < start; i++)
    {
        copy += str[i];
    }
    for (int i = 0; i < replacement.size(); i++)
    {
        copy += replacement[i];
    }
    for (int i = start + query.size(); i < str.size(); i++)
    {
        copy += str[i];
    }

    str = copy;
}

inline void replaceAllSubStr(std::wstring& str, std::wstring query, std::wstring replacement)
{
    while (whereSubStr(str, query) != -1)
    {
        replaceSubStr(str, query, replacement);
    }
}

inline bool beginWith(std::wstring str, std::wstring query)
{
    if (whereSubStr(str, query) == 0) return true;
    else return false;
}










inline std::tm GetCurrentDateAndTime()
{
    std::time_t seconds_since = time(nullptr);
    std::tm calendar = { };
    gmtime_s(&calendar, &seconds_since);

    calendar.tm_year += 1900;
    calendar.tm_mon += 1;

    return calendar;
}

inline std::string GetDateAndTimeStr()
{
    std::tm current_time = GetCurrentDateAndTime();
    unsigned int mt = current_time.tm_mon;
    unsigned int dd = current_time.tm_mday;
    unsigned int yyyy = current_time.tm_year;
    unsigned int hh = current_time.tm_hour;
    unsigned int mn = current_time.tm_min;
    unsigned int ss = current_time.tm_sec;
 
    std::string str = "";
    str += "[";
    str += NumToStr(mt, 10, 2) + "-";
    str += NumToStr(dd, 10, 2) + "-";
    str += NumToStr(yyyy, 10, 4) + ": ";

    str += NumToStr(hh, 10, 2) + ":";
    str += NumToStr(mn, 10, 2) + ":";
    str += NumToStr(ss, 10, 2);
    str += " UTC+0]";

    return str;
}


#endif

