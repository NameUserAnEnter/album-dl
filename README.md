# album-dl README

## Description
This program is a tool for downloading album-playlists from youtube as audio files, with album cover-artwork attached.

## Requirements
- Windows 64-bit OS.
- `ffmpeg.exe`

album-dl requires ffmpeg to work, if you don't have ffmpeg installed, visit:  
https://ffmpeg.org/download.html

## Basis
This program bases on the following projects:
- yt-dlp, a fork of youtube-dl: <https://github.com/yt-dlp/yt-dlp>
- youtube-dl: <https://github.com/ytdl-org/youtube-dl>
- ffmpeg: <https://ffmpeg.org/about.html>

## Quick tips
### Shortcuts
- `Ctrl + S`:      Save settings
- `Esc`:           Exit the program
- `Tab`:           Focus next field
- `Shift + Tab`:   Focus previous field
### Settings
Press `Ctrl + S` or go to "File/Save settings" to save:
- directores
- window position/size
- notify-when-done checkbox state
- check-for-updates checkbox state
- audio bitrate

These settings are automatically loaded each time album-dl is opened.

## Program details
### Directories
Each directory can be either a full or a relative path.

#### Albums directory
A directory where the downloaded album-folder will be moved,
e.g.: `C:/Albums/`  
If the directory does not exist, it will be created.
#### Working directory
Working directory with the following files:
- `yt-dlp.exe`
- `config.txt`
- `cookies.txt` (optional)

By default, working directory is set to (relative path): `workfolder/` and all necessary files are provided there.
#### ffmpeg.exe directory
A directory containing `ffmpeg.exe`.

album-dl requires ffmpeg to work, if you don't have ffmpeg installed, visit:  
https://ffmpeg.org/download.html

### Album data fields
#### Artist
Used to name the album-folder as well as audio files.
#### Album name & Album year
Used in the album-folder name.
#### Playlist URL
This playlist will be downloaded into .mp3's.
If no artwork-playlist URL is provided, this one is used to download the album artwork as well.
#### Playlist URL with artwork
Album artwork is downloaded from this playlist. If left empty, URL from "Playlist URL" is used.

### Output
During execution, album-dl's terminal prints out each executed command's output, as well as it's own info in that box.  
Each execution's full output is stored in the `log` file, in `album-dl.exe` directory, although only untill the next execution overwrites it. Only the last execution's output is kept in `log`.

## Album folder name and audio filenames format
    Albums Directory/
        Artist - Album Name (Album Year)/
            Track Number. Artist - Track Title.mp3
            Track Number. Artist - Track Title.mp3
            Track Number. Artist - Track Title.mp3
            ...
            Track Number. Artist - Track Title.mp3
            artwork.png

## Execution stages
album-dl goes through following stages after the "Download" button is pressed:
1. If update checkbox is selected, check for YT-DLP updates and update if newer version is available
2. Download best audio quality .mp4 files
3. Download the album artwork
4. Get track titles from the playlist
5. Convert .mp4 files into .mp3 files
6. Remove .mp4 files
7. Rename .mp3 files
8. Attach artwork.png to .mp3 metadata
9. Create destination album subfolder in the albums directory
10. Move .mp3 files into the destination folder
11. Move artwork.png into the destination folder

## `Trash` folder
This program uses Windows commands to execute renaming and moving files as well as creating directories.
For extra-safety this program does not use the `DEL` command,
left-over files are instead put in the `<working-directory>/Trash/` subfolder in the working directory
and you are free to delete contents of that subfolder manually.

However, there is no need to clear these files as album-dl
always overwrites left-over files with the same name, each time leftover files are moved there,
so as a result the amount of disk space taken by the `<working-directory>/Trash/` subfolder does not stack,
instead it is around the amount of disk space taken by just the last run's left-over files.

## Bypassing age-restriction
In case of an age-restricted URL: 
1. get a youtube cookies dump into cookies.txt
2. place cookies.txt in the working directory
3. include the following line in config.txt: --cookies "cookies.txt"

## Motivation
The primary way of using `youtube-dl`/`yt-dlp` for downloading albums, in my case, used to be combining it with `ffmpeg` to get the songs from a playlist into `.mp3`'s.
To simplify things a `.bat` script would be created to execute download, convert, `MKDIR` and `MOVE` commands.

Renaming files was initially done completely manually, although later a `.bat` script was also created for that purpose. However, that script still required filling track titles and artist/album data manually for folder/filenames.  

Furthermore, album's cover artwork would be downloaded manually from the playlist's thumbnail into a `.png` file, then attached to `.mp3`'s metadata using `foobar2000` audio player.
Later on, during that process I would use a custom `.js` script to extract the thumbnail URL which was faster than extracting it manually from the HTML DOM.

To achieve further automation to facilitate expanding an offline music library at hand, `album-dl` has been created, which is the simplest form of that process known to me so far.

## License
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
