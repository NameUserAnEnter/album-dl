#ifndef ENCODING_H
#define ENCODING_H
#define cUNDEF 0x0000
#include "utils.h"
#include <string>

namespace codepage
{
    static wchar_t table_CP437[256] = {
        0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
        0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
        0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
        0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
        0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
        0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
        0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
        0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7, 0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
        0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9, 0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192,
        0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba, 0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
        0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
        0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
        0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
        0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4, 0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
        0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248, 0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0
    };

    // source:
    // https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/PC/CP852.TXT
    static wchar_t table_CP852[256] = {
        0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
        0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
        0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
        0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
        0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
        0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
        0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
        0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x016f, 0x0107, 0x00e7, 0x0142, 0x00eb, 0x0150, 0x0151, 0x00ee, 0x0179, 0x00c4, 0x0106,
        0x00c9, 0x0139, 0x013a, 0x00f4, 0x00f6, 0x013d, 0x013e, 0x015a, 0x015b, 0x00d6, 0x00dc, 0x0164, 0x0165, 0x0141, 0x00d7, 0x010d,
        0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x0104, 0x0105, 0x017d, 0x017e, 0x0118, 0x0119, 0x00ac, 0x017a, 0x010c, 0x015f, 0x00ab, 0x00bb,
        0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00c1, 0x00c2, 0x011a, 0x015e, 0x2563, 0x2551, 0x2557, 0x255d, 0x017b, 0x017c, 0x2510,
        0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x0102, 0x0103, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x00a4,
        0x0111, 0x0110, 0x010e, 0x00cb, 0x010f, 0x0147, 0x00cd, 0x00ce, 0x011b, 0x2518, 0x250c, 0x2588, 0x2584, 0x0162, 0x016e, 0x2580,
        0x00d3, 0x00df, 0x00d4, 0x0143, 0x0144, 0x0148, 0x0160, 0x0161, 0x0154, 0x00da, 0x0155, 0x0170, 0x00fd, 0x00dd, 0x0163, 0x00b4,
        0x00ad, 0x02dd, 0x02db, 0x02c7, 0x02d8, 0x00a7, 0x00f7, 0x00b8, 0x00b0, 0x00a8, 0x02d9, 0x0171, 0x0158, 0x0159, 0x25a0, 0x00a0
    };

    // source:
    // https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1250.TXT
    static wchar_t table_CP1250[256] = {
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
                unsigned char Byte[] = { (unsigned char)currentChar & 31, str[i + 1] & 63 };
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
                    unsigned char Byte[] = { (unsigned char)currentChar & 15, str[i + 1] & 63, str[i + 2] & 63 };
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






inline std::wstring GetWideFromCodePage(const char* raw, wchar_t table[])
{
    std::wstring wideStr = L"";
    for (int i = 0; raw[i] != '\0'; i++)
    {
        unsigned char currentByte = (unsigned char)raw[i];
        wchar_t wCodePoint = table[currentByte];

        wideStr += wCodePoint;
    }
    return wideStr;
}

inline std::string EncodeToCodePage(std::wstring wstr, wchar_t table[])
{
    std::string encoded = "";
    for (wchar_t wide : wstr)
    {
        for (unsigned int tableIndex = 0; tableIndex < 256; tableIndex++)
        {
            if (wide == table[tableIndex])
            {
                encoded += tableIndex;
                break;
            }
        }
    }
    return encoded;
}



inline std::wstring GetTableDiff(wchar_t table1[], wchar_t table2[])
{
    std::wstring tableStr1 = L"";
    std::wstring tableStr2 = L"";

    for (int i = 0; i < 256; i++)
    {
        tableStr1 += table1[i];
        tableStr2 += table2[i];
    }

    std::wstring diffStr = L"";
    for (int i = 0; i < tableStr1.size(); i++)
    {
        if (!findCharInStr(tableStr2, tableStr1[i]))
        {
            diffStr += tableStr1[i];
        }
    }

    for (int i = 0; i < tableStr2.size(); i++)
    {
        if (!findCharInStr(tableStr1, tableStr2[i]))
        {
            diffStr += tableStr2[i];
        }
    }
    return diffStr;
}









inline std::wstring printTableDiff(wchar_t table1[], wchar_t table2[])
{
    std::wstring tableDiff = GetTableDiff(table1, table2);
    std::wstring output = L"";
    for (int i = 0; i < tableDiff.size(); i++)
    {
        if (isPrintable(tableDiff[i])) output += tableDiff[i];

        if ((i + 1) % 16 == 0 && i != output.size() - 1) output += L"\n";
        else output += ' ';
    }
    if (!output.empty()) output += L"\n\n";
    return output;
}

inline std::wstring printTable(wchar_t table[])
{
    std::wstring output = L"";

    //output += L' ' + L' ' + L':';
    output += L"   ";
    for (int i = 0; i < 16; i++)
    {
        output += L"   " + NumToWstr(i % 16, 16, 2);
    }

    for (int i = 0; i < 256; i++)
    {
        if (i % 16 == 0) output += L"\n" + NumToWstr(i, 16, 2) + L": ";
        else output += L" ";

        if (isPrintable(table[i]))
        {
            output += L"   ";
            output += table[i];
        }
        else
        {
            //output += NumToWstr(table[i], 16, 4);
            output += L"    ";
        }
    }

    output += L"\n----";
    for (int i = 0; i < 16; i++)
    {
        output += L"-----";
    }
    output += L"\n\n";
    return output;
}



#endif
