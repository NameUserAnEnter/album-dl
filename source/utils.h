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

inline double absolute_value(double num)
{
    if (num >= 0) return num;
    else return num * (-1);
}

inline std::string NumToStr(unsigned long long num, unsigned long long base = 10, unsigned int min_digits = 1, char whitespace = '0')
{
    // 1, 10, 2, ' '

    std::string str = "";
    if (num == 0)   // special case
    {
        str = "0";
        for (int i = 0; i < min_digits - 1; i++)
        {
            str = whitespace + str;
        }
        return str;
    }

    unsigned int count_digits = 0;
    // FFFFFFFF
    while (power(base, count_digits) <= num) count_digits++;

    unsigned int size = count_digits;
    if (size < min_digits) size = min_digits;

    for (int i = 0; i < size - count_digits; i++)
    {
        str += whitespace;
    }
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




// COMMON SIMPLE STRUCTURES
struct Margin
{
    int top, left, bottom, right;
};

struct Rect
{
    int x, y, width, height;
};

inline Rect RectUnion(Rect r1, Rect r2)
{
    //        UNION
    //     _____/\_____
    //    /            \
    //    ______________ 
    //   | _______      |/_______ r1.y
    //   ||       |     |\
    //   ||  r1___|____ |/_________ r2.y
    //   ||   |   |    ||\
    //   ||___|___|    ||/___________ r1.y + r1.height
    //   |    |    r2  ||\
    //   |    |________||/____________ r2.y + r2.height
    //   |______________|\
    //    ^   ^   ^    ^
    //    |   |   |    |
    //  r1.x  |   |    |_
    //        |   |      |
    //      r2.x  | r2.x + r2.width
    //            |
    //       r1.x + r1.width
    //

    Rect result;

    int leftmost;
    if (r1.x <= r2.x) leftmost = r1.x;
    else leftmost = r2.x;

    int topmost;
    if (r1.y <= r2.y) topmost = r1.y;
    else topmost = r2.y;

    int rightmost = r1.x + r1.width;
    if (r1.x + r2.width > rightmost) rightmost = r1.x + r2.width;
    if (r2.x + r2.width > rightmost) rightmost = r2.x + r2.width;
    if (r2.x + r1.width > rightmost) rightmost = r2.x + r1.width;
    result.x = leftmost;
    result.width = rightmost - leftmost;

    int bottommost = r1.y + r1.height;
    if (r1.y + r2.height > bottommost) bottommost = r1.y + r2.height;
    if (r2.y + r2.height > bottommost) bottommost = r2.y + r2.height;
    if (r2.y + r1.height > bottommost) bottommost = r2.y + r1.height;
    result.y = topmost;
    result.height = bottommost - topmost;

    return result;
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

inline bool FileExist(const wchar_t* filename)
{
    FILE* file;
    _wfopen_s(&file, filename, L"rb");
    if (file == nullptr)
    {
        return false;
    }

    fclose(file);
    return true;
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





// BYTES DUMPING
inline void AppendBytesDump(std::string buf, std::string filepath, unsigned int& point)
{
    std::string output = "";
    for (int i = 0; i < buf.size(); i++)
    {
        output += NumToStr((unsigned char)buf[i], 16, 2);

        point++;
        if (point % 16 == 0) output += "\n";
        else output += " ";
    }
    AppendDataToFile(output, filepath + "_bytes");
    AppendDataToFile(buf, filepath);
}

inline void ClearBytesDump(std::string filepath)
{
    ClearFileData(filepath + "_bytes");
    ClearFileData(filepath);
}




// VECTOR UTILS
template<typename T>
std::vector<T> reverseVector(std::vector<T> input)
{
    std::vector<T> returnValue;
    for (int i = input.size() - 1; i >= 0; i--)
    {
        returnValue.push_back(input[i]);
    }
    return returnValue;
}



// STRING TRANSFORMATIONS UTILS
template<typename T>
std::basic_string<T> reverseStr(std::basic_string<T> input)
{
    std::basic_string<T> returnValue;
    for (int i = input.size() - 1; i >= 0; i--)
    {
        returnValue.push_back(input[i]);
    }
    return returnValue;
}

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
                // 012345678901
                //      ^
                // tests\rtests
                // i = 5, index = 0
                // i = 6, index = 1, index + 1 = 2 == 2, 6 - 1 = 5
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
    std::basic_string<T> currentFragment;
    unsigned int fragmentIndex = 0;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == query[fragmentIndex])
        {
            // got a fragment of the query
            if (fragmentIndex + 1 == query.size())
            {
                // found the full query
                
                str.replace(i - fragmentIndex, query.size(), replacement);  // args: (first, count, replacement)
                i -= query.size();                      // place the index back at start of the query
                i += replacement.size();                // skip over the new replacement

                // reset the current fragment for next finds
                currentFragment.clear();
                fragmentIndex = 0;
                continue;   // instatly skip to condition check to make sure 'i' won't exceed str.size() even if code is added after the if
            }
            else
            {
                // still just a fragment of the query
                fragmentIndex++;
            }
        }
        else
        {
            // not a fragment of the query, reset the current fragment
            fragmentIndex = 0;
            currentFragment.clear();
        }
    }
}

template<typename T>
inline void replaceAllSubStr(std::basic_string<T>& str, const T* query, const T* replacement)
{
    replaceAllSubStr(str, std::basic_string<T>(query), std::basic_string<T>(replacement));
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

template<typename T>
inline std::vector<std::basic_string<T>> splitByChar(std::basic_string<T> str, T query, bool leaveQueried = true)
{
    std::vector<std::basic_string<T>> returnValue;
    returnValue.clear();
    returnValue.push_back(std::basic_string<T>());
    for (int i = 0; i < str.size(); i++)
    {
        returnValue.back() += str[i];
        if (str[i] == query)
        {
            if (!leaveQueried) returnValue.back().pop_back();
            returnValue.push_back(std::basic_string<T>());
        }
    }
    if (returnValue.back().size() == 0) returnValue.pop_back();
    return returnValue;
}

template<typename T>
inline std::vector<std::basic_string<T>> splitByNewlines(std::basic_string<T> str, bool leaveNewlines = true)
{
    return splitByChar(str, (T)'\n', leaveNewlines);
}

template<typename T>
inline void splitByFirstFoundChar(std::basic_string<T> str, T query, std::basic_string<T>& firstHalf, std::basic_string<T>& secondHalf)
{
    firstHalf.clear();
    secondHalf.clear();
    bool bFound = false;
    for (int i = 0; i < str.size(); i++)
    {
        if (!bFound)
        {
            firstHalf += str[i];
        }
        else
        {
            secondHalf += str[i];
            continue;
        }

        if (str[i] == query)
        {
            bFound = true;
        }
    }
}

template<typename T>
inline void splitByLastFoundChar(std::basic_string<T> str, T query, std::basic_string<T>& firstHalf, std::basic_string<T>& secondHalf)
{
    firstHalf.clear();
    secondHalf.clear();
    bool bFound = false;
    for (int i = str.size() - 1; i >= 0; i--)
    {
        if (str[i] == query)
        {
            bFound = true;
        }

        if (!bFound)
        {
            secondHalf += str[i];
        }
        else
        {
            firstHalf += str[i];
            continue;
        }
    }

    firstHalf = reverseStr(firstHalf);
    secondHalf = reverseStr(secondHalf);
}

template<typename T>
inline std::vector<std::basic_string<T>> splitByStr(std::basic_string<T> str, std::basic_string<T> query, bool leaveQueried = true)
{
    std::vector<std::basic_string<T>> returnValue;
    returnValue.clear();

    int result = whereSubStr(str, query);

    if (result != -1)
    {
        std::basic_string<T> left, right;
        left.clear();
        right.clear();

        //           1         2
        // 0123456789012345678901
        // Stars in the universe.
        // query.size(): 3
        // query: the
        // result: 9
        // query[9]:  t
        // query[10}: h
        // query[11]: e
        // 
        // 9 + x = 11
        //     x = 11 - 9
        //     x = 2
        //
        for (int i = 0; i < str.size(); i++)
        {
            if (i < result) left += str[i];
            else
            {
                if (i <= result + query.size() - 1)
                {
                    if (leaveQueried) left += str[i];
                }
                else
                {
                    right += str[i];
                }
            }
        }

        if (!left.empty()) returnValue.push_back(left);

        std::vector<std::basic_string<T>> rest = splitByStr(right, query);
        for (auto current : rest) returnValue.push_back(current);
    }
    else
    {
        returnValue.push_back(str);
    }


    return returnValue;
}

template<typename T>
inline std::vector<std::basic_string<T>> splitByStr(std::basic_string<T> str, const T* query, bool leaveQueried = true)
{
    return splitByStr(str, std::basic_string<T>(query), leaveQueried);
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
inline std::string MsToSec(long ms, unsigned int minFracDigits = 0)
{
    // minFracDigits: 0 <-> 3
    std::string returnValue = NumToStr(ms, 10, 4, '0');
    returnValue.insert(returnValue.size() - 1 + 1 - 3, 1, '.');

    unsigned int cutFracDigits = 0;
    while (returnValue.back() == '0')
    {
        if (3 - cutFracDigits == minFracDigits) break;

        returnValue.pop_back();
        cutFracDigits++;
    }

    if (returnValue.back() == '.') returnValue.pop_back();

    returnValue += "s";
    return returnValue;
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

