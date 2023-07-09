#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <windows.h>
#include <thread>
#include <time.h>
#include "Console.h"


// TRIM WIDE TO NARROW & CONVERT NARROW TO WIDE
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





// MATH & SIMPLE NON-NEGATIVE NUMBERS-TO-STR
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

inline std::string NumToStr(unsigned long long num, unsigned long long base = 10, unsigned int min_digits = 1, char whitespace = '0')
{
    std::string str = "";

    unsigned int count_digits = 0;
    // FFFFFFFF
    while (power(base, count_digits) <= num) count_digits++;

    unsigned int size = count_digits;
    if (size < min_digits) size = min_digits;

    for (int i = 0; i < size - count_digits; i++) str += whitespace;
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

inline std::wstring NumToWstr(unsigned long long num, unsigned long long base = 10, unsigned int min_digits = 1, char whitespace = '0')
{
    return toWide(NumToStr(num, base, min_digits, whitespace));
}

inline std::wstring HexToWstr(unsigned long long hex, unsigned int digits = 2)
{
    return toWide(HexToStr(hex, digits));
}









// DIALOG-BOX UTILS
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















// FILE UTILS
inline int GetFileData(const wchar_t* filename, std::string* returnData = nullptr)
{
    std::string data = "";

    FILE* file;
    _wfopen_s(&file, filename, L"rb");
    if (file == nullptr)
    {
        //ErrMsgExit(L"Cannot open: " + std::wstring(filename) + L'\n');
        MessageDialog(L"Cannot open: " + std::wstring(filename) + L'\n');
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
        //ErrMsgExit(L"Cannot open: " + std::wstring(filename) + L'\n');
        MessageDialog(L"Cannot open: " + std::wstring(filename) + L'\n');
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




//      NULL-TERMINATED NARROW OVERLOADS
inline int GetFileData(const char* filename, std::string* returnData = nullptr)
{
    return GetFileData(toWide(std::string(filename)).c_str(), returnData);
}

inline int WriteDataToFile(std::string data, const char* filename)
{
    return WriteDataToFile(data, toWide(std::string(filename)).c_str());
}

inline int AppendDataToFile(std::string data, const char* filename)
{
    return AppendDataToFile(data, toWide(std::string(filename)).c_str());
}

inline int ClearFileData(const char* filename)
{
    return ClearFileData(toWide(std::string(filename)).c_str());
}


//      STRING NARROW OVERLOADS
inline int GetFileData(std::string filename, std::string* returnData = nullptr)
{
    return GetFileData(toWide(filename).c_str(), returnData);
}

inline int WriteDataToFile(std::string data, std::string filename)
{
    return WriteDataToFile(data, toWide(filename).c_str());
}

inline int AppendDataToFile(std::string data, std::string filename)
{
    return AppendDataToFile(data, toWide(filename).c_str());
}

inline int ClearFileData(std::string filename)
{
    return ClearFileData(toWide(filename).c_str());
}





//      WIDE STRING OVERLOADS
inline int GetFileData(std::wstring filename, std::string* returnData = nullptr)
{
    return GetFileData(filename.c_str(), returnData);
}

inline int WriteDataToFile(std::string data, std::wstring filename)
{
    return WriteDataToFile(data, filename.c_str());
}

inline int AppendDataToFile(std::string data, std::wstring filename)
{
    return AppendDataToFile(data, filename.c_str());
}

inline int ClearFileData(std::wstring filename)
{
    return ClearFileData(filename.c_str());
}





// STRING TRANSFORMATIONS UTILS
template<typename T>
inline std::basic_string<T> GetBackslashPath(std::basic_string<T> forwardslashPath)
{
    std::wstring backslashPath = L"";
    for (int i = 0; i < forwardslashPath.size(); i++)
    {
        if (forwardslashPath[i] != '/')
        {
            backslashPath += forwardslashPath[i];
        }
        else if (forwardslashPath[i] == '/' && i != forwardslashPath.size() - 1)
        {
            std::basic_string<T> sep;
            sep.clear();
            sep += (T)'\\';

            backslashPath += sep;
        }
    }
    return backslashPath;
}

template<typename T>
inline unsigned int GetNumFromFirstWord(std::basic_string<T> str)
{
    std::string num = "";
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            num += str[i];
        }
        else break;
    }

    if (num.empty()) return 0;
    return std::stoi(num);
}


template<typename T>
inline bool findCharInStr(std::basic_string<T> str, T query)
{
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == query) return true;
    }
    return false;
}


template<typename T>
inline bool findInStr(std::basic_string<T> str, std::basic_string<T> query)
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


template<typename T>
inline int whereSubStr(std::basic_string<T> str, std::basic_string<T> query)
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

template<typename T>
inline int whereSubStr(std::basic_string<T> str, const T* query)
{
    return whereSubStr(str, std::basic_string<T>(query));
}

template<typename T>
inline void replaceSubStr(std::basic_string<T>& str, std::basic_string<T> query, std::basic_string<T> replacement)
{
    int start = whereSubStr(str, query);
    std::basic_string<T> copy;
    copy.clear();

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

template<typename T>
inline void replaceSubStr(std::basic_string<T>& str, const T* query, const T* replacement)
{
    replaceSubStr(str, std::basic_string<T>(query), std::basic_string<T>(replacement));
}

template<typename T>
inline void replaceAllSubStr(std::basic_string<T>& str, std::basic_string<T> query, std::basic_string<T> replacement)
{
    while (whereSubStr(str, query) != -1)
    {
        replaceSubStr(str, query, replacement);
    }
}

template<typename T>
inline void replaceAllSubStr(std::basic_string<T>& str, const T* query, const T* replacement)
{
    while (whereSubStr(str, query) != -1)
    {
        replaceSubStr(str, query, replacement);
    }
}

template<typename T>
inline bool beginWith(std::basic_string<T> str, std::basic_string<T> query)
{
    if (whereSubStr(str, query) == 0) return true;
    else return false;
}

template<typename T>
inline bool beginWith(std::basic_string<T> str, const T* query)
{
    return beginWith(str, std::basic_string<T>(query));
}



// STRING & CHAR EXAMINATION
inline bool isStrNum(std::basic_string<wchar_t> str)
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

inline bool isStrNum(std::basic_string<char> str)
{
    for (auto symbol : str)
    {
        if (!isdigit(symbol))
        {
            return false;
        }
    }
    return true;
}

inline bool isPrintable(wchar_t ch)
{
    if (ch >= 0x20) return true;
    else return false;
}







// TIME UTILS
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

