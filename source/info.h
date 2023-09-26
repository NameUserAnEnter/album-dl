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

        "For more information, please refer to <https://unlicense.org/>";
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
        "This program is a tool for downloading albums from youtube.\n"

        "\n# REQUIREMENTS:\n"
        "Album-dl requires FFmpeg to work, if you don't have FFmpeg installed, visit:\n"
        "https://ffmpeg.org/download.html\n"

        "\n# SHORTCUTS:\n"
        "'Ctrl + S':      Save settings\n"
        "'Esc':           Exit the program\n"
        "'Tab':           Next field\n"
        "'Shift + Tab':   Previous field"
        ;

    return output;
}

inline std::string GetReadMe()
{
    std::string output = "";


    output +=
        GetAbout() +

        //"\n\n------------------------------------------------------------------------------------------\n"

        "# SAVING SETTINGS:\n" +
        "Press 'Ctrl+S' or go to \"File/Save settings\" to save:\n"
        "albums directory, working directory, FFmpeg directory, alert-checkbox state, bitrate & window position/size.\n"
        "These settings are automatically loaded each time album-dl is opened.\n"

        //"\n\n------------------------------------------------------------------------------------------\n"

        "# ALBUMS DIRECTORY:\n"
        "Full or relative path to a directory where the downloaded album folder will be moved,\ne.g.: C:/Albums/\n"
        "If the directory does not exist, it will be created.\n"

        "\n\n# WORKING DIRECTORY:\n"
        "Full or relative path to a working directory with the following files:\n"
        "            -yt-dlp.exe\n"
        "            -config.txt\n"
        "            -cookies.txt (optional)\n"
        "By default working directory is set to (relative path): workfolder/ and all necessary files are provided there.\n"

        "\n\n# FFMPEG.EXE DIRECTORY:\n"
        "Full or relative path to a directory containing ffmpeg.exe.\n"
        "Album-dl requires FFmpeg to work, if you don't have FFmpeg installed, visit:\n"
        "https://ffmpeg.org/download.html\n"

        //"\n\n------------------------------------------------------------------------------------------\n"

        "# ARTIST:\n"
        "Used for output album folder name and track filenames.\n"

        "\n# ALBUM NAME & ALBUM YEAR:\n"
        "Used only for output album folder name.\n"

        "\n# PLAYLIST URL:\n"
        "This playlist will be downloaded into .mp3's.\n"
        "If no artwork-playlist URL is provided, this one is used to download the album artwork as well.\n"

        "\n# PLAYLIST URL WITH PROPER ARTWORK:\n"
        "Album artwork is downloaded from this playlist. If left empty, URL from \"Playlist URL\" is used.\n"

        //"\n\n------------------------------------------------------------------------------------------\n"

        "# OUTPUT FILENAMES & FOLDER NAME FORMAT:\n"
        "Albums Directory/\n"
        "            Artist - Album Name (Album Year)/\n"
        "                        Track Number. Artist - Track Title.mp3\n"
        "                        Track Number. Artist - Track Title.mp3\n"
        "                        Track Number. Artist - Track Title.mp3\n"
        "                        ...\n"
        "                        Track Number. Artist - Track Title.mp3\n"
        "                        artwork.png\n"

        "\n\n# TRASH:\n"
        "This program uses Windows commands to execute renaming and moving files as well as creating directories.\n"
        "For extra-safety this program does not use DEL command,\n"
        "left-over .mp4 files are instead put in /Trash/ subfolder in the working directory\n"
        "and you are free to delete contents of that subfolder manually.\n"
        "However there is no need to clear these files as album-dl\n"
        "always overwrites .mp4's with the same name each time leftover .mp4's are moved there,\n"
        "so as a result the amount of disk space taken by /Trash/ subfolder does not stack.\n"

        "\n\n# STAGES:\n"
        "Album-dl goes through following stages after the \"Run\" button is pressed:\n"
        "1. downloading best audio quality .mp4's\n"
        "6. downloading the album artwork\n"
        "2. getting the track titles\n"
        "3. converting .mp4 files into .mp3's\n"
        "4. removing the .mp4's\n"
        "5. renaming the .mp3's\n"
        "7. attaching artwork.png to .mp3's metadata\n"
        "8. creating destination album subfolder in the albums directory\n"
        "9. moving .mp3's into the destination folder\n"
        "10. moving .png into the destination folder\n"

        //"\n\n------------------------------------------------------------------------------------------\n"

        "# BYPASSING AGE-RESTRICTION:\n"
        "In case of an age-restricted URL: \n"
        "1. get a youtube cookies dump into cookies.txt\n"
        "2. place cookies.txt in the working directory\n"
        "3. include the following line in config.txt: --cookies \"cookies.txt\"\n"


        //"\n\n------------------------------------------------------------------------------------------\n"
        "# LICENSE:\n" + GetLicense();

    return output;
}

inline void DumpReadMe()
{
    std::string readme = GetReadMe();
    replaceAllSubStr(readme, "\n", "\r\n");
    WriteDataToFile(readme, L"README.md");
}

inline void DumpLicense()
{
    std::string license = GetLicense();
    replaceAllSubStr(license, "\n", "\r\n");
    WriteDataToFile(license, L"LICENSE.txt");
}

#endif
