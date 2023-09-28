# VERSION:
album-dl_v1.1


# DESCRIPTION:
This program is a tool for downloading albums from youtube.

# REQUIREMENTS:
album-dl requires FFmpeg to work, if you don't have FFmpeg installed, visit:
https://ffmpeg.org/download.html

# SHORTCUTS:
-'Ctrl + S':      Save settings
-'Esc':           Exit the program
-'Tab':           Focus next field
-'Shift + Tab':   Focus previous field


# SAVING SETTINGS:
Press 'Ctrl+S' or go to "File/Save settings" to save:
-directores
-window position/size
-alert-on-done checkbox state
-audio bitrate

These settings are automatically loaded each time album-dl is opened.


# ALBUMS DIRECTORY:
Full or relative path to a directory where the downloaded album folder will be moved,
e.g.: C:/Albums/
If the directory does not exist, it will be created.


# WORKING DIRECTORY:
Full or relative path to a working directory with the following files:
            -yt-dlp.exe
            -config.txt
            -cookies.txt (optional)
By default working directory is set to (relative path): workfolder/ and all necessary files are provided there.


# FFMPEG.EXE DIRECTORY:
Full or relative path to a directory containing ffmpeg.exe.
Album-dl requires FFmpeg to work, if you don't have FFmpeg installed, visit:
https://ffmpeg.org/download.html


# ARTIST:
Used for output album folder name and track filenames.

# ALBUM NAME & ALBUM YEAR:
Used only for output album folder name.

# PLAYLIST URL:
This playlist will be downloaded into .mp3's.
If no artwork-playlist URL is provided, this one is used to download the album artwork as well.

# PLAYLIST URL WITH ARTWORK:
Album artwork is downloaded from this playlist. If left empty, URL from "Playlist URL" is used.


# OUTPUT FILENAMES & FOLDER NAME FORMAT:
Albums Directory/
            Artist - Album Name (Album Year)/
                        Track Number. Artist - Track Title.mp3
                        Track Number. Artist - Track Title.mp3
                        Track Number. Artist - Track Title.mp3
                        ...
                        Track Number. Artist - Track Title.mp3
                        artwork.png


# TRASH:
This program uses Windows commands to execute renaming and moving files as well as creating directories.
For extra-safety this program does not use the DEL command,
left-over files are instead put in the /Trash/ subfolder in the working directory
and you are free to delete contents of that subfolder manually.
However there is no need to clear these files as album-dl
always overwrites left-over files with the same name, each time leftover files are moved there,
so as a result the amount of disk space taken by the /Trash/ subfolder does not stack,
instead it is around the amount of disk space taken by just the last run's left-over files.


# STAGES:
Album-dl goes through following stages after the "Download" button is pressed:
1. downloading best audio quality .mp4's
6. downloading the album artwork
2. getting the track titles
3. converting .mp4 files into .mp3's
4. removing the .mp4's
5. renaming the .mp3's
7. attaching artwork.png to .mp3's metadata
8. creating destination album subfolder in the albums directory
9. moving .mp3's into the destination folder
10. moving .png into the destination folder


# BYPASSING AGE-RESTRICTION:
In case of an age-restricted URL: 
1. get a youtube cookies dump into cookies.txt
2. place cookies.txt in the working directory
3. include the following line in config.txt: --cookies "cookies.txt"


# LICENSE:
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org/>