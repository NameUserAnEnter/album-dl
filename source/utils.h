#ifndef UTILS_H
#define UTILS_H
#define cUNDEF 0x0000

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
    std::wstring wideStr = L"";
    for (int i = 0; raw[i] != '\0'; i += 2)
    {
        wideStr.push_back(0x00);
        wideStr.back() += (unsigned char)raw[i];
        wideStr.back() += (unsigned short)(raw[i + 1] << 8);
    }
    return wideStr;
}

inline std::wstring GetWideFromWindows1250(const char* raw)
{
    // source:
    // https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1250.TXT
    static wchar_t table[256] = {
        0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
        0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
        0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
        0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
        0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
        0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
        0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
        0x20AC, cUNDEF, 0x201A, cUNDEF, 0x201E, 0x2026, 0x2020, 0x2021, cUNDEF, 0x2030, 0x0160, 0x2039, 0x015A, 0x0164, 0x017D, 0x0179,
        cUNDEF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, cUNDEF, 0x2122, 0x0161, 0x203A, 0x015B, 0x0165, 0x017E, 0x017A,
        0x00A0, 0x02C7, 0x02D8, 0x0141, 0x00A4, 0x0104, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x015E, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x017B,
        0x00B0, 0x00B1, 0x02DB, 0x0142, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x0105, 0x015F, 0x00BB, 0x013D, 0x02DD, 0x013E, 0x017C,
        0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7, 0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
        0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7, 0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
        0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7, 0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
        0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7, 0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9
    };

    std::wstring wideStr = L"";
    for (int i = 0; raw[i] != '\0'; i++)
    {
        unsigned char currentByte = (unsigned char)raw[i];
        wchar_t wCodePoint = table[currentByte];

        wideStr += wCodePoint;
    }
    return wideStr;
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

inline int ClearFileData(const wchar_t* filename)
{
    return WriteDataToFile("", filename);
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

