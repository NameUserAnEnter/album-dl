#ifndef INFO_H
#define INFO_H
#define VER 1.1
#include "utils.h"



inline float GetVersionNum()
{
    unsigned int fraction_digits = 0;
    float verNum = VER;
    while (verNum != (int)verNum)
    {
        verNum *= 10;
        fraction_digits++;
    }

    return verNum;
}

inline std::string GetVersionStr()
{
    unsigned int fraction_digits = 0;
    float verNum = VER;
    while (verNum != (int)verNum)
    {
        verNum *= 10;
        fraction_digits++;
    }


    std::string verStr = std::to_string((int)VER);
    if (fraction_digits > 0)
    {
        verStr += ".";
        for (int i = 0; i < std::to_string(VER).size(); i++)
        {
            if (std::to_string(VER)[i] == '.')
            {
                i++;
                for (int j = 0; j < fraction_digits && i + j < std::to_string(VER).size(); j++)
                {
                    verStr += std::to_string(VER)[i + j];
                }
                break;
            }
        }
    }
    else verStr += ".0";

    return verStr;
}




inline std::string GetLicense()
{
    std::string output =
        "This is free and unencumbered software released into the public domain.\n\n"

        "Anyone is free to copy, modify, publish, use, compile, sell, or\n"
        "distribute this software, either in source code form or as a compiled\n"
        "binary, for any purpose, commercial or non-commercial, and by any\n"
        "means.\n\n"

        "In jurisdictions that recognize copyright laws, the author or authors\n"
        "of this software dedicate any and all copyright interest in the\n"
        "software to the public domain. We make this dedication for the benefit\n"
        "of the public at large and to the detriment of our heirs and\n"
        "successors. We intend this dedication to be an overt act of\n"
        "relinquishment in perpetuity of all present and future rights to this\n"
        "software under copyright law.\n\n"

        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,\n"
        "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\n"
        "MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\n"
        "IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR\n"
        "OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,\n"
        "ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR\n"
        "OTHER DEALINGS IN THE SOFTWARE.\n\n"

        "For more information, please refer to: https://unlicense.org/";
    return output;
}




inline std::string GetAbout()
{
    float verNum = GetVersionNum();
    std::string verStr = GetVersionStr();

    std::string output = "";

    output +=
        "# VERSION:\n"
        "album-dl" "_v" + verStr + "\n"
        
        "\n\n# DESCRIPTION:\n"
        "This program is a tool for downloading album-playlists from youtube as audio files, with album cover-artwork attached.\n"
        "For more information visit:\n"
        "https://github.com/NameUserAnEnter/album-dl\n"

        "\n# REQUIREMENTS:\n"
        "album-dl requires ffmpeg to work, if you don't have ffmpeg installed, visit:\n"
        "https://ffmpeg.org/download.html\n"

        "\n# SHORTCUTS:\n"
        "- `Ctrl + S`:      Save settings\n"
        "- `Esc`:           Exit the program\n"
        "- `Tab`:           Focus next field\n"
        "- `Shift + Tab`:   Focus previous field\n"
        ;

    return output;
}

#endif

