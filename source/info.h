#ifndef INFO_H
#define INFO_H
#define VER 1.1
#include "utils.h"


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
        "software to the public domain.We make this dedication for the benefit\n"
        "of the public at largeand to the detriment of our heirsand\n"
        "successors.We intend this dedication to be an overt act of\n"
        "relinquishment in perpetuity of all presentand future rights to this\n"
        "software under copyright law.\n\n"

        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,\n"
        "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\n"
        "MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\n"
        "IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR\n"
        "OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,\n"
        "ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR\n"
        "OTHER DEALINGS IN THE SOFTWARE.\n\n"

        "For more information, please refer to <http://unlicense.org/>";
    return output;
}

inline std::string GetReadMe()
{
    std::string output = "";

    unsigned int fraction_digits = 0;
    float uVer = VER;
    while (uVer != (int)uVer)
    {
        uVer *= 10;
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

    output +=
        std::string("VERSION:\n") +
        //std::string("album-dl") + " v" + std::to_string((int)uVer) + ", " + std::to_string(fraction_digits) + "\n" +
        std::string("album-dl") + "_v" + verStr + "\n" +

        std::string("\n\nDESCRIPTION:\n") +
        std::string("This program is a tool for downloading albums from youtube.\nIt uses youtube-dl (yt-dlp) to download an album and it's artwork") +
        std::string(" and then moves that album into a given directory after attaching the album artwork as metadata.\n") +

        std::string("\nSAVING SETTINGS:\n") +
        std::string("Press Ctrl+S or go to \"File/Save settings\" to save album directory, working directory, alert-checkbox state & window position.") +
        std::string(" These settings are automatically loaded each time album-dl is opened.\n") +

        std::string("\n\n------------------------------------------------------------------------------------------\n") +

        std::string("ALBUMS DIRECTORY:\n") +
        std::string("Full or relative path to a directory where the album folder will be moved,\ne.g.: C:/Albums/\n") +
        std::string("If the directory does not exist, it will be created.\n") +

        std::string("\n\nWORKING DIRECTORY:\n") +
        std::string("Full or relative path to a working directory with the following files:\n") +
        std::string("            -yt-dlp.exe\n") +
        std::string("            -ffmpeg.exe\n") +
        std::string("            -config.txt\n") +
        std::string("            -cookies.txt (optional)\n") +
        std::string("By default working directory is set to (relative path): workfolder/ and all necessary files are provided there.\n") +

        std::string("\n\nARTIST:\n") +
        std::string("Used for output album folder name and track filenames.\n") +

        std::string("\nALBUM NAME & ALBUM YEAR:\n") +
        std::string("Used only for output album folder name.\n") +

        std::string("\nPLAYLIST URL:\n") +
        std::string("This playlist will be downloaded into .mp3's.\n") +
        std::string("If no artwork-playlist URL is provided, this one is used to download the album artwork as well.\n") +

        std::string("\nPLAYLIST URL WITH PROPER ARTWORK:\n") +
        std::string("Album artwork is downloaded from this playlist. If left empty, URL from \"Playlist URL\" is used.\n") +

        std::string("\n------------------------------------------------------------------------------------------\n") +

        std::string("OUTPUT FILENAMES & FOLDER NAME FORMAT:\n") +
        std::string("Albums Directory/\n") +
        std::string("            Artist - Album Name (Album Year)/\n") +
        std::string("                        Track Number. Artist - Track Title.mp3\n") +
        std::string("                        Track Number. Artist - Track Title.mp3\n") +
        std::string("                        Track Number. Artist - Track Title.mp3\n") +
        std::string("                        ...\n") +
        std::string("                        Track Number. Artist - Track Title.mp3\n") +
        std::string("                        artwork.png\n") +

        std::string("\n\nTRASH:\n") +
        std::string("This program uses windows commands to execute renaming and moving files as well as creating directories.") +
        std::string(" For extra-safety this program does not use the DEL command,") +
        std::string(" left-over .mp4 files are instead put in the /Trash/ subfolder in the working directory") +
        std::string(" and you are free to delete the contents of that subfolder manually.") +
        std::string(" However there is no need to clear these files as album-dl") +
        std::string(" always overwrites .mp4's with the same name each time leftover .mp4's are moved there,") +
        std::string(" so as a result the amount of disk space taken by the /Trash/ subfolder does not stack.\n") +

        std::string("\n\nSTAGES:\n") +
        std::string("The program goes through the following stages after the \"Run\" button is pressed:\n") +
        std::string("1) downloading best audio quality .mp4's\n") +
        std::string("2) getting the track titles\n") +
        std::string("3) converting .mp4 files into .mp3's\n") +
        std::string("4) removing the .mp4's\n") +
        std::string("5) renaming the .mp3's\n") +
        std::string("6) downloading the album artwork\n") +
        std::string("7) attaching artwork.png to .mp3's metadata\n") +
        std::string("8) creating destination album subfolder in the albums directory\n") +
        std::string("9) moving .mp3's into the destination folder\n") +
        std::string("10) moving .png into the destination folder\n") +

        std::string("\n------------------------------------------------------------------------------------------\n") +

        std::string("BYPASSING AGE-RESTRICTION:\n") +
        std::string("In case of an age-restricted URL: \n") +
        std::string("1) get a youtube cookies dump into cookies.txt\n") +
        std::string("2) place cookies.txt in the working directory\n") +
        std::string("3) include a following line in config.txt:\n--cookies \"cookies.txt\"\n") +


        std::string("\n\n------------------------------------------------------------------------------------------\n") +
        std::string("LICENSE:\n") + GetLicense();

    return output;
}

inline void DumpReadMe()
{
    WriteDataToFile(GetReadMe(), L"README.txt");
}

inline void DumpLicense()
{
    WriteDataToFile(GetLicense(), L"LICENSE.txt");
}

#endif
