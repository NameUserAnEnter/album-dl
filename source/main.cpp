#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include <vector>
#include <wininet.h>

// ID's for GUI-elements
enum
{
    ID_Save = 1,
    ID_albumsDir_Field,
    ID_workingDir_Field,
    ID_artist_Field,
    ID_albumName_Field,
    ID_albumYear_Field,
    ID_tracks_Field,
    ID_URL_Field,
    ID_Button,
    ID_Frame,
    ID_AlertOnDone,
    ID_URL_Artwork_Field
};

unsigned int ClientWidth, ClientHeight;

wxSize mainOffset(20, 40);
wxSize fieldBetweenSpace(10, 20);

wxSize TextBoxSize(500, 20);
wxSize LargeBoxSize(500, 500);
wxSize ButtonSize(100, 25);

RECT labelOffset;


double power(double base, unsigned int exponent)
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

std::string NumToStr(unsigned int num, unsigned int base = 10, unsigned int min_digits = 1)
{
    std::string str = "";

    unsigned int count_digits = 0;
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
        unsigned int digit = (num % (unsigned int)power(base, i + 1)) / power(base, i);

        if (digit < 10) str += (unsigned char)((unsigned int)'0' + digit);
        else str += (unsigned char)((unsigned int)'A' + (digit - 10));
    }
    return str;
}

std::string HexToStr(unsigned int hex, unsigned int digits = 2)
{
    return NumToStr(hex, 16, digits);
}

int GetThumbnailURL(std::string* returnURL, const char* inputFilename);
int GetResource(const char* host, const char* resource, const char* outputFilename);

void PrintConsole(std::string text)
{
    unsigned long charsWritten = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), text.c_str(), text.size(), &charsWritten, NULL);
}

void PrintConsole(std::wstring text)
{
    unsigned long charsWritten = 0;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), text.c_str(), text.size(), &charsWritten, NULL);
}

void printHex(std::string, std::string = "");

std::string EncodeToUTF8(std::wstring str)
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
            else if (character >= 0x10000)
            {
                // wchar_t is 16-bit on Windows, 0x10000+ code points are not supported
            }
        }
    }
    return encoded;
}

std::wstring DecodeFromUTF8(std::string str)
{
    std::wstring decoded = L"";
    for (int i = 0; i < str.size(); i++)
    {
        wchar_t currentChar = str[i];
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

int GetFileData(const wchar_t* filename, std::string* returnData = nullptr)
{
    std::string data = "";

    FILE* file;
    _wfopen_s(&file, filename, L"rb");
    if (file == nullptr)
    {
        PrintConsole(L"Cannot open: " + std::wstring(filename) + L'\n');
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

int WriteDataToFile(std::string data, const wchar_t* filename)
{
    FILE* file;
    _wfopen_s(&file, filename, L"wb");
    if (file == nullptr)
    {
        PrintConsole(L"Cannot open: " + std::wstring(filename) + L'\n');
        return 1;
    }

    for (int i = 0; i < data.size(); i++)
    {
        putc((unsigned char)data[i], file);
    }
    fclose(file);

    return 0;
}




class TextBox
{
public:
    wxStaticBox* labelBox;
    wxTextCtrl* textField;

    TextBox(wxString label, wxWindowID textFieldID, wxPoint position, wxSize size, wxPanel* panel, bool multiline = false)
    {
        labelBox = new wxStaticBox(
            panel, wxID_ANY, label,
            wxPoint(position.x - labelOffset.left, position.y - labelOffset.top),
            wxSize(labelOffset.left + size.x + labelOffset.right, labelOffset.top + size.y + labelOffset.bottom),
            0, wxString(label + " label"));


        long style = 0;
        if (multiline) style = wxTE_MULTILINE;
        textField = new wxTextCtrl(
            labelBox, textFieldID, "",
            wxPoint(labelOffset.left, labelOffset.top),
            size,
            style, wxDefaultValidator, wxString(label + " text field"));

        mainOffset.y += size.y + fieldBetweenSpace.y;
    }

    ~TextBox()
    {
        delete labelBox;
        delete textField;
    }
};


class MainApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MainFrame : public wxFrame
{
public:
    MainFrame();
private:
    TextBox* albumsDir_Field;
    TextBox* workingDir_Field;
    TextBox* artist_Field;
    TextBox* albumName_Field;
    TextBox* albumYear_Field;
    TextBox* tracks_Field;

    TextBox* URL_Field;
    TextBox* URL_Artwork_Field;

    wxCheckBox* checkAlert;

private:
    void OnButtonPress(wxCommandEvent& event);
    
    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);
public:
    void SaveSettings();
    void OpenSettings();
private:
    void LoadTrackTitles();
    void GetAlbum();
    void ResetTracksFile();

    void AttachArtwork(std::wstring, std::wstring);

    void ValidateFilesystemString(std::wstring& str);
    void ValidateTrackTitles();

    void SetTracksField();
    void CreateTrashDir();
private:
    std::wstring URL;
    std::wstring artworkURL;

    std::wstring albumsDirectory;
    std::wstring workingDirectory;

    std::wstring execName;
    std::wstring configName;

    std::wstring artist;
    std::wstring albumName;
    std::wstring albumYear;

    std::vector<std::wstring> trackTitles;
    std::wstring artworkFilename;

    std::wstring albumsDirBackslashes;
    std::wstring albumPathBackslashes;
    std::wstring workingDirBackslashes;
private:
    void DownloadStage();
    void ConvertStage();
    void RemoveLeftoverStage();
    
    void GetTitlesStage();
    
    void RenameFilesStage();
    
    void GetArtworkStage();
    void AttachArtworkStage();
    
    void CreateAlbumDirectoryStage();
    void MoveAudioStage();
    void MoveArtworkStage();
};

// Macro containing the main() function, invoking the entry-point of the program
wxIMPLEMENT_APP(MainApp);



bool MainApp::OnInit()
{
    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;

    MainFrame* frame = new MainFrame();
    frame->SetClientSize(ClientWidth, ClientHeight);
    frame->SetPosition(wxPoint(1020, 20));
    frame->OpenSettings();
    frame->Show(true);
    return true;
}


MainFrame::MainFrame() : wxFrame(NULL, ID_Frame, "album-dl")
{
    // File:
    //      Save settings
    wxMenu* menuFile = new wxMenu;
    // Characters preceded by an ampersand in menu item text are the mnemonic underlined-in-alt-mode acces-keys for these menu items
    // The \tShortcut specify the item shortcuts
    menuFile->Append(ID_Save, "&Save settings\tCtrl+S", "Save Albums directory, Working directory, window position & alert check box");
    menuFile->AppendSeparator();
    //      Exit
    menuFile->Append(wxID_EXIT, "&Exit\tEsc");

    // Help:
    //      About
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    
    
    // File | Help
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("");

    // default sizes and pos because it's automatically stretched to the frame anyway
    wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2621440L, "Main Panel");


    albumsDir_Field = new TextBox("Albums directory:", ID_albumsDir_Field,
                                  wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);
    workingDir_Field = new TextBox("Working directory:", ID_workingDir_Field,
                                  wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);

    mainOffset.y += TextBoxSize.y + fieldBetweenSpace.y;
    artist_Field = new TextBox("Artist:", ID_artist_Field,
                                   wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);
    albumName_Field = new TextBox("Album name:", ID_albumName_Field,
                               wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);
    albumYear_Field = new TextBox("Album year:", ID_albumYear_Field,
                                  wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);


    tracks_Field = new TextBox("Tracks (auto-filled):", ID_tracks_Field,
                               wxPoint(mainOffset.x, mainOffset.y), LargeBoxSize, mainPanel, true);
    //tracks_Field->textField->Disable();
    tracks_Field->textField->SetEditable(false);

    URL_Field = new TextBox("Playlist URL:", ID_URL_Field,
                            wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);

    URL_Artwork_Field = new TextBox("Playlist URL with proper artwork:", ID_URL_Artwork_Field,
                                    wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);



    wxButton* button = new wxButton(mainPanel, ID_Button, "Run",
                                    wxPoint(mainOffset.x, mainOffset.y),
                                    ButtonSize, NULL, wxDefaultValidator, "Run button");

    checkAlert = new wxCheckBox(mainPanel, ID_AlertOnDone, "Alert on done",
                                wxPoint(mainOffset.x + ButtonSize.x + fieldBetweenSpace.x, mainOffset.y),
                                ButtonSize, 0, wxDefaultValidator, "Alert checkbox");
    mainOffset.y += ButtonSize.y + fieldBetweenSpace.y;

    Bind(wxEVT_BUTTON, &MainFrame::OnButtonPress, this, ID_Button);

    
    Bind(wxEVT_MENU, &MainFrame::OnSave, this, ID_Save);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);


    ClientWidth = mainOffset.x + TextBoxSize.x + mainOffset.x;
    ClientHeight = mainOffset.y;

    // Sample test values for convenience
    /*
    artist_Field->textField->SetValue("Big Black");
    albumName_Field->textField->SetValue("Lungs");
    albumYear_Field->textField->SetValue("1982");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_lSCRmY_Qw8RCNnMKHcp05O1K8fAIyqLjs");
    URL_Artwork_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_lSCRmY_Qw8RCNnMKHcp05O1K8fAIyqLjs");
    */

    /*
    artist_Field->textField->SetValue("O.S.T.R.");
    albumName_Field->textField->SetValue("Tylko Dla Doros³ych");
    albumYear_Field->textField->SetValue("2010");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=PLIKxxmyVA3HZ5vCNl3b0gQXDhuMWLz-mG");
    URL_Artwork_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_l6DSlExq2EbVR7ILChbL9ZHn-1SbyKRO8");
    */

    artist_Field->textField->SetFocus();
}

void MainFrame::OpenSettings()
{
    FILE* settingsFile = nullptr;
    std::string path = "settings";
    fopen_s(&settingsFile, path.c_str(), "r");
    if (settingsFile != nullptr)
    {
        std::string currentWord = "";
        enum value_ID {
            albumsDir = 1,
            workingDir,
            windowX,
            windowY,
            alertDone,
            none
        };
        unsigned int id = albumsDir;
        char currentChar;
        do
        {
            currentChar = fgetc(settingsFile);
            if (currentChar >= 32 && currentChar <= 126)
            {
                currentWord += currentChar;
            }
            else
            {
                if (currentWord.size() > 0)
                {
                    if (id == albumsDir) albumsDir_Field->textField->SetValue(currentWord);
                    if (id == workingDir) workingDir_Field->textField->SetValue(currentWord);
                    if (id == windowX) SetPosition(wxPoint(std::stoi(currentWord), GetPosition().y));
                    if (id == windowY) SetPosition(wxPoint(GetPosition().x, std::stoi(currentWord)));
                    if (id == alertDone)
                    {
                        if (currentWord == "0") checkAlert->SetValue(false);
                        if (currentWord == "1") checkAlert->SetValue(true);
                    }
                }

                currentWord = "";
                id++;
            }

            SetStatusText("Settings have beed loaded");
        } while (currentChar != EOF);


        fclose(settingsFile);
    }
    else
    {
        SetStatusText("Settings file not found");
    }
}

void MainFrame::SaveSettings()
{
    FILE* settingsFile = nullptr;
    std::string path = "settings";
    fopen_s(&settingsFile, path.c_str(), "w");
    if (settingsFile == nullptr)
    {
        SetStatusText("Cannot save settings");
        return;
    }
    fputs(albumsDir_Field->textField->GetValue().c_str(), settingsFile); fputc('\n', settingsFile);
    fputs(workingDir_Field->textField->GetValue().c_str(), settingsFile); fputc('\n', settingsFile);
    fputs(NumToStr(GetPosition().x).c_str(), settingsFile); fputc('\n', settingsFile);
    fputs(NumToStr(GetPosition().y).c_str(), settingsFile); fputc('\n', settingsFile);

    if (checkAlert->GetValue() == true) fputc((unsigned char)'1', settingsFile);
    else fputc((unsigned char)'0', settingsFile);


    fclose(settingsFile);
    SetStatusText("Settings have been saved");
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    //SaveSettings();
    
    Destroy();
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    std::string output = "";

#define VER 1.0
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
        std::string("\n\nVERSION:\n") +
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

        std::string("\nTRACKS:\n") +
        std::string("Track names are automatically taken from video titles in the playlist.\n") +
        std::string("After album-dl is done, filenames that were set for individual tracks are displayed.\n") +

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
        std::string("2) converting them into .mp3's\n") +
        std::string("3) removing the .mp4's\n") +
        std::string("4) getting the track titles\n") +
        std::string("5) renaming the .mp3's\n") +
        std::string("6) downloading the album artwork\n") +
        std::string("7) attaching the artwork.png to .mp3's metadata\n") +
        std::string("8) creating the album subfolder in albums directory\n") +
        std::string("9) moving the .mp3's there\n") +
        std::string("10) moving the .png there\n") +

        std::string("\n------------------------------------------------------------------------------------------\n") +

        std::string("BYPASSING AGE-RESTRICTION:\n") +
        std::string("In case of an age-restricted URL: \n") +
        std::string("1) get a youtube cookies dump into cookies.txt\n") +
        std::string("2) place cookies.txt in the working directory\n") +
        std::string("3) include a following line in config.txt:\n--cookies \"cookies.txt\"\n");
    wxMessageBox(output, "About", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnSave(wxCommandEvent& event)
{
    SaveSettings();
    wxMessageBox("Settings have been saved.", "album-dl", wxOK | wxICON_NONE);
}

bool validField(std::wstring data)
{
    for (int i = 0; i < data.size(); i++)
    {
        if ((unsigned char)data[i] >= (unsigned char)32 && (unsigned char)data[i] <= (unsigned char)126)
        {
            return true;
        }
    }
    return false;
}

bool findInStr(std::wstring data, std::wstring query)
{
    unsigned int index = 0;
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i] == query[index])
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
    //wxMessageBox(data + "\n" + query);
    return false;
}

bool validField(std::wstring data, std::wstring mandatoryFragment)
{
    if (!validField(data)) return false;
    if (!findInStr(data, mandatoryFragment)) return false;
    return true;
}

bool validField(std::wstring data, std::vector<std::wstring> allPossibleMandatoryFragments)
{
    if (!validField(data)) return false;
    for (int i = 0; i < allPossibleMandatoryFragments.size(); i++)
    {
        if (findInStr(data, allPossibleMandatoryFragments[i])) return true;
    }
    return false;
}

void MainFrame::OnButtonPress(wxCommandEvent& event)
{
    URL = URL_Field->textField->GetValue().ToStdWstring();
    artworkURL = URL_Artwork_Field->textField->GetValue().ToStdWstring();

    albumsDirectory = albumsDir_Field->textField->GetValue().ToStdWstring();
    workingDirectory = workingDir_Field->textField->GetValue().ToStdWstring();
    execName = L"yt-dlp.exe";
    configName = L"config.txt";

    artist = artist_Field->textField->GetValue().ToStdWstring();
    albumName = albumName_Field->textField->GetValue().ToStdWstring();
    albumYear = albumYear_Field->textField->GetValue().ToStdWstring();

    ValidateFilesystemString(artist);
    ValidateFilesystemString(albumName);
    ValidateFilesystemString(albumYear);


    if (!validField(albumsDirectory))
    {
        wxMessageBox("Albums directory invalid.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    if (!validField(workingDirectory))
    {
        wxMessageBox("Working directory invalid.", "Error", wxOK | wxICON_ERROR);
        return;
    }



    std::vector<std::wstring> validPlaylistURLs = {
        L"https://youtube.com/", L"https://www.youtube.com/", L"http://youtube.com/", L"http://www.youtube.com/"
    };
    if (!validField(URL, validPlaylistURLs))
    {
        std::wstring output = L"Playlist URL invalid.\n\nRequires either of:";
        for (int i = 0; i < validPlaylistURLs.size(); i++)
        {
            output += L"\n" + validPlaylistURLs[i];
        }
        wxMessageBox(output, "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::vector<std::wstring> validArtworkURLs = {
        L"https://youtube.com/playlist?", L"https://www.youtube.com/playlist?", L"http://youtube.com/playlist?", L"http://www.youtube.com/playlist?"
    };
    if (!validField(artworkURL, validArtworkURLs))
    {
        if (!validField(URL, validArtworkURLs))
        {
            std::wstring output = L"Artwork Playlist URL invalid.\n\nRequires either of:";
            for (int i = 0; i < validArtworkURLs.size(); i++)
            {
                output += L"\n" + validArtworkURLs[i];
            }
            wxMessageBox(output, "Error", wxOK | wxICON_ERROR);
            return;
        }
        else
        {
            SetStatusText("Copying Playlist URL as Artwork Playlist URL");
            URL_Artwork_Field->textField->SetValue(URL);
            artworkURL = URL;
        }
    }

    if (albumsDirectory[albumsDirectory.size() - 1] != '/')
    {
        albumsDirectory += '/';
        albumsDir_Field->textField->SetValue(albumsDirectory);
    }

    if (workingDirectory[workingDirectory.size() - 1] != '/')
    {
        workingDirectory += '/';
        workingDir_Field->textField->SetValue(workingDirectory);
    }

    trackTitles.clear();
    SetTracksField();
    artworkFilename = L"artwork.png";



    albumsDirBackslashes = L"";
    for (int i = 0; i < albumsDirectory.size(); i++)
    {
        if (albumsDirectory[i] != '/')
        {
            albumsDirBackslashes += albumsDirectory[i];
        }
        else if (!(i == albumsDirectory.size() - 1 && albumsDirectory[i] == '/'))
        {
            albumsDirBackslashes += L"\\";
        }
    }

    albumPathBackslashes = albumsDirBackslashes + L"\\" + artist + L" - " + albumName + L" (" + albumYear + L")";

    workingDirBackslashes = L"";
    for (int i = 0; i < workingDirectory.size(); i++)
    {
        if (workingDirectory[i] != '/')
        {
            workingDirBackslashes += workingDirectory[i];
        }
        else if (!(i == workingDirectory.size() - 1 && workingDirectory[i] == '/'))
        {
            workingDirBackslashes += L"\\";
        }
    }

    GetAlbum();
}



std::wstring lineSeparator = L"\n--------------------------------------------------------------------------------\n";

void MainFrame::DownloadStage()
{
    SetStatusText("Downloading tracks");



    std::wstring args = L"";
    args += L"--config-location \"" + workingDirectory + configName + L"\"";
    args += ' ';
    args += L"-o \"" + workingDirectory + L"td8_index%(playlist_index)s.mp4\" " + URL;

    std::wstring fullCommand = L""; fullCommand += workingDirectory + execName + ' ' + args;
    PrintConsole(fullCommand);
    PrintConsole("\n\n");


    unsigned long bufSize = fullCommand.size() + 1;
    wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
    for (int i = 0; i < bufSize - 1; i++)
    {
        buf[i] = fullCommand[i];
    }
    buf[bufSize - 1] = '\0';


    STARTUPINFO startupinfo = { };
    PROCESS_INFORMATION process_information = { };
    BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
    unsigned long exit_code;
    do
    {
        Update();
        GetExitCodeProcess(process_information.hProcess, &exit_code);
    } while (exit_code == STILL_ACTIVE);


    std::wstring output;
    if (rv != 0)
    {
        output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
    }
    else
    {
        output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
    }
    PrintConsole(output);
    PrintConsole(lineSeparator);


    free(buf);
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);

    SetStatusText("Finished downloading tracks (0x" + HexToStr(exit_code) + ")");
}

void MainFrame::ConvertStage()
{
    SetStatusText("Converting track files to audio");


    std::wstring cmd = L"forfiles /P \"" + workingDirBackslashes + L"\"" + L" /M td8_index*.mp4 /C \"cmd /c ffmpeg.exe -i @file -c:a mp3 -b:a 192k -ar 44100 @fname.mp3\"";
    PrintConsole(cmd);
    PrintConsole("\n\n");

    unsigned int bufSize = cmd.size() + 1;
    wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
    for (int i = 0; i < bufSize - 1; i++)
    {
        buf[i] = cmd[i];
    }
    buf[bufSize - 1] = '\0';


    STARTUPINFO startupinfo = { };
    PROCESS_INFORMATION process_information = { };
    BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
    unsigned long exit_code;
    do
    {
        Update();
        GetExitCodeProcess(process_information.hProcess, &exit_code);
    } while (exit_code == STILL_ACTIVE);

    std::wstring output;
    if (rv != 0)
    {
        output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
    }
    else
    {
        output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
    }
    PrintConsole(output);
    PrintConsole(lineSeparator);



    free(buf);
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);

    SetStatusText("Finished converting track files (0x" + HexToStr(exit_code) + ")");
}

void MainFrame::CreateTrashDir()
{
    std::wstring cmd = L"cmd /c \"MKDIR \"" + workingDirBackslashes + L"\\Trash" + L"\"\"";
    PrintConsole(cmd);
    PrintConsole("\n\n");

    
    unsigned int bufSize = cmd.size() + 1;
    wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
    for (int i = 0; i < bufSize - 1; i++)
    {
        buf[i] = cmd[i];
    }
    buf[bufSize - 1] = '\0';


    STARTUPINFO startupinfo = { };
    PROCESS_INFORMATION process_information = { };
    BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
    unsigned long exit_code;
    do
    {
        Update();
        GetExitCodeProcess(process_information.hProcess, &exit_code);
    } while (exit_code == STILL_ACTIVE);

    std::wstring output;
    if (rv != 0)
    {
        output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
    }
    else
    {
        output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
    }
    PrintConsole(output);
    PrintConsole(lineSeparator);



    free(buf);
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);
}

void MainFrame::RemoveLeftoverStage()
{
    // Execute mkdir in case /Trash/ doesn't exist
    CreateTrashDir();
    SetStatusText("Moving leftover files to: " + workingDirectory + "Trash/");

    std::wstring cmd = L"cmd /c \"MOVE \"" + workingDirBackslashes + L"\\td8_index*.mp4\" \"" + workingDirBackslashes + L"\\Trash\"\"";
    PrintConsole(cmd);
    PrintConsole("\n\n");



    unsigned int bufSize = cmd.size() + 1;
    wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
    for (int i = 0; i < bufSize - 1; i++)
    {
        buf[i] = cmd[i];
    }
    buf[bufSize - 1] = '\0';


    STARTUPINFO startupinfo = { };
    PROCESS_INFORMATION process_information = { };
    BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
    unsigned long exit_code;
    do
    {
        Update();
        GetExitCodeProcess(process_information.hProcess, &exit_code);
    } while (exit_code == STILL_ACTIVE);

    std::wstring output;
    if (rv != 0)
    {
        output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
    }
    else
    {
        output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
    }
    PrintConsole(output);
    PrintConsole(lineSeparator);



    free(buf);
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);


    SetStatusText("Finished moving leftover files (0x" + HexToStr(exit_code) + ")");
}

void MainFrame::GetTitlesStage()
{
    SetStatusText("Downloading track titles");

    std::wstring cmd = L""; cmd += workingDirectory + execName + ' ' + L"-e --print-to-file %(title)s \"tracks\" " + URL;
    PrintConsole(cmd);
    PrintConsole("\n\n");



    unsigned long bufSize = cmd.size() + 1;
    wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
    for (int i = 0; i < bufSize - 1; i++)
    {
        buf[i] = cmd[i];
    }
    buf[bufSize - 1] = '\0';


    STARTUPINFO startupinfo = { };
    PROCESS_INFORMATION process_information = { };
    BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
    unsigned long exit_code;
    do
    {
        Update();
        GetExitCodeProcess(process_information.hProcess, &exit_code);
    } while (exit_code == STILL_ACTIVE);

    std::wstring output;
    if (rv != 0)
    {
        output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
    }
    else
    {
        output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
    }
    PrintConsole(output);
    PrintConsole(lineSeparator);



    free(buf);
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);


    SetStatusText("Finished downloading track titles (0x" + HexToStr(exit_code) + ")");
}

void MainFrame::RenameFilesStage()
{
    SetStatusText("Renaming track files");

    unsigned int maxNumDigits = std::to_string(trackTitles.size()).size();

    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring num = L"";

        unsigned int numDigits = std::to_string(i + 1).size();
        for (int j = 0; j < maxNumDigits - numDigits; j++) num += L'0';
        num += std::to_wstring(i + 1);

        std::wstring cmd = L"";
        cmd += L"cmd /c \"RENAME \"" + workingDirBackslashes + L"\\td8_index" + num + L".mp3\" \"";
        cmd += std::to_string(i + 1) + L". " + artist + L" - " + trackTitles[i] + L".mp3\"\"";
        
        PrintConsole(cmd);
        PrintConsole("\n\n");

        
        
        unsigned int bufSize = cmd.size() + 1;
        wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
        for (int i = 0; i < bufSize - 1; i++)
        {
            buf[i] = cmd[i];
        }
        buf[bufSize - 1] = '\0';


        STARTUPINFO startupinfo = { };
        PROCESS_INFORMATION process_information = { };
        BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
        unsigned long exit_code;
        do
        {
            Update();
            GetExitCodeProcess(process_information.hProcess, &exit_code);
        } while (exit_code == STILL_ACTIVE);

        std::wstring output;
        if (rv != 0)
        {
            output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
        }
        else
        {
            output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
        }
        PrintConsole(output);
        PrintConsole(lineSeparator);

        free(buf);
        CloseHandle(startupinfo.hStdInput);
        CloseHandle(startupinfo.hStdOutput);
        CloseHandle(startupinfo.hStdError);
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);
    }

    SetStatusText("Finished renaming track files");
}

void MainFrame::GetArtworkStage()
{
    std::string host = "";
    std::string resource = "";

    unsigned int cFragment = 0;

    for (int i = 0; i < artworkURL.size(); i++)
    {
        if (cFragment == 0)
        {
            if (artworkURL[i] == ':')
            {
                host = "";
                i += 2;
                continue;
            }
            else if (artworkURL[i] == '/')
            {
                cFragment++;
            }
            else
            {
                host += artworkURL[i];
            }
        }

        if (cFragment == 1)
        {
            resource += artworkURL[i];
        }
    }

    PrintConsole("Downloading album artwork...\n");
    PrintConsole("host: " + host + '\n');
    PrintConsole("resource: " + resource + '\n');
    PrintConsole("\n\n");

    const char resourceFilename[] = "index.html";
    GetResource(host.c_str(), resource.c_str(), resourceFilename);
    std::string thumbnailURL = "";
    GetThumbnailURL(&thumbnailURL, resourceFilename);





    std::wstring args = L"";
    args += L"-o \"" + workingDirectory + artworkFilename + "\" \"" + thumbnailURL + "\"";

    std::wstring fullCommand = L""; fullCommand += workingDirectory + execName + ' ' + args;
    PrintConsole(fullCommand);
    PrintConsole("\n\n");


    unsigned long bufSize = fullCommand.size() + 1;
    wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
    for (int i = 0; i < bufSize - 1; i++)
    {
        buf[i] = fullCommand[i];
    }
    buf[bufSize - 1] = '\0';


    STARTUPINFO startupinfo = { };
    PROCESS_INFORMATION process_information = { };
    BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
    unsigned long exit_code;
    do
    {
        Update();
        GetExitCodeProcess(process_information.hProcess, &exit_code);
    } while (exit_code == STILL_ACTIVE);

    std::wstring output;
    if (rv != 0)
    {
        output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
    }
    else
    {
        output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
    }
    PrintConsole(output);
    PrintConsole(lineSeparator);


    free(buf);
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);

    // Erase the resource .html file data
    FILE* resourceFile;
    fopen_s(&resourceFile, resourceFilename, "w");
    if (resourceFile != nullptr) fclose(resourceFile);

    SetStatusText("Finished downloading album cover (0x" + HexToStr(exit_code) + ")");
}

void MainFrame::AttachArtworkStage()
{
    SetStatusText("Attaching cover metadata");
    PrintConsole("Attaching cover metadata\n");

    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring trackFilepath = workingDirectory + std::to_wstring(i + 1) + L". " + artist + L" - " + trackTitles[i] + L".mp3";
        AttachArtwork(trackFilepath, artworkFilename);
    }

    SetStatusText("Finished attaching cover metadata");
    PrintConsole("Finished ataching cover metadata\n");
    PrintConsole(lineSeparator);
}

void MainFrame::CreateAlbumDirectoryStage()
{
    SetStatusText("Creating album directory");
    std::wstring cmd = L"cmd /c \"MKDIR \"" + albumPathBackslashes + L"\"\"";
    PrintConsole(cmd);
    PrintConsole("\n\n");


    unsigned int bufSize = cmd.size() + 1;
    wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
    for (int i = 0; i < bufSize - 1; i++)
    {
        buf[i] = cmd[i];
    }
    buf[bufSize - 1] = '\0';


    STARTUPINFO startupinfo = { };
    PROCESS_INFORMATION process_information = { };
    BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
    unsigned long exit_code;
    do
    {
        Update();
        GetExitCodeProcess(process_information.hProcess, &exit_code);
    } while (exit_code == STILL_ACTIVE);

    std::wstring output;
    if (rv != 0)
    {
        output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
    }
    else
    {
        output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
    }
    PrintConsole(output);
    PrintConsole(lineSeparator);



    free(buf);
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);


    SetStatusText("Finished creating album directory (0x" + HexToStr(exit_code) + ")");
}

void MainFrame::MoveAudioStage()
{
    SetStatusText("Moving audio files");
    std::wstring cmd = L"cmd /c \"MOVE \"" + workingDirBackslashes + L"\\*.mp3\" \"" + albumPathBackslashes + L"\"\"";
    PrintConsole(cmd);
    PrintConsole("\n\n");



    unsigned int bufSize = cmd.size() + 1;
    wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
    for (int i = 0; i < bufSize - 1; i++)
    {
        buf[i] = cmd[i];
    }
    buf[bufSize - 1] = '\0';


    STARTUPINFO startupinfo = { };
    PROCESS_INFORMATION process_information = { };
    BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
    unsigned long exit_code;
    do
    {
        Update();
        GetExitCodeProcess(process_information.hProcess, &exit_code);
    } while (exit_code == STILL_ACTIVE);

    std::wstring output;
    if (rv != 0)
    {
        output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
    }
    else
    {
        output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
    }
    PrintConsole(output);
    PrintConsole(lineSeparator);



    free(buf);
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);


    SetStatusText("Finished moving audio files (0x" + HexToStr(exit_code) + ")");
}

void MainFrame::MoveArtworkStage()
{
    SetStatusText("Moving cover file");
    std::wstring cmd = L"cmd /c \"MOVE \"" + workingDirBackslashes + L"\\" + artworkFilename + L"\" \"" + albumPathBackslashes + L"\"\"";
    PrintConsole(cmd);
    PrintConsole("\n\n");



    unsigned int bufSize = cmd.size() + 1;
    wchar_t* buf = (wchar_t*)calloc(bufSize, sizeof(wchar_t));
    for (int i = 0; i < bufSize - 1; i++)
    {
        buf[i] = cmd[i];
    }
    buf[bufSize - 1] = '\0';


    STARTUPINFO startupinfo = { };
    PROCESS_INFORMATION process_information = { };
    BOOL rv = CreateProcess(NULL, (wchar_t*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
    unsigned long exit_code;
    do
    {
        Update();
        GetExitCodeProcess(process_information.hProcess, &exit_code);
    } while (exit_code == STILL_ACTIVE);

    std::wstring output;
    if (rv != 0)
    {
        output = L"\n\nSuccess (0x" + HexToStr(exit_code) + L")";
    }
    else
    {
        output = L"\n\nFailure\nGetLastError() returned: 0x" + HexToStr(GetLastError());
    }
    PrintConsole(output);
    PrintConsole(lineSeparator);



    free(buf);
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);


    SetStatusText("Finished moving artwork (0x" + HexToStr(exit_code) + ")");
}




void MainFrame::GetAlbum()
{
    // TODO:
    // -updating not only screen but events during console process execution
    SetStatusText("Running the script...");
    AllocConsole();
    
    // DOWNLOAD
    DownloadStage();

    // CONVERT
    ConvertStage();

    // REMOVE LEFTOVER FILES
    RemoveLeftoverStage();

    // GET TRACK TITLES
    ResetTracksFile();
    GetTitlesStage();
    LoadTrackTitles();
    ValidateTrackTitles();  // Remove unwanted & forbidden characters from the track titles
    ResetTracksFile();
    
    // RENAME
    RenameFilesStage();

    // DOWNLOAD ARTWORK
    GetArtworkStage();
    
    // ATTACH ARTWORK
    AttachArtworkStage();

    // CREATE DIRECTORY
    CreateAlbumDirectoryStage();

    // MOVE AUDIO FILES
    MoveAudioStage();
    // MOVE ARTWORK
    MoveArtworkStage();

    FreeConsole();

    SetStatusText("Resetting");
    // Reset fields & set focus
    URL_Field->textField->SetValue("");
    URL_Artwork_Field->textField->SetValue("");
    artist_Field->textField->SetValue("");
    albumName_Field->textField->SetValue("");
    albumYear_Field->textField->SetValue("");

    artist_Field->textField->SetFocus();

    SetStatusText("Done");
    if (checkAlert->GetValue() == true) MessageBoxA(NULL, "Script has finished.", "Done", MB_OK);
}





int whereSubStr(std::wstring str, std::wstring query)
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

void replaceSubStr(std::wstring& str, std::wstring query, std::wstring replacement)
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

void replaceAllSubStr(std::wstring& str, std::wstring query, std::wstring replacement)
{
    while (whereSubStr(str, query) != -1)
    {
        replaceSubStr(str, query, replacement);
    }
}

void MainFrame::ValidateFilesystemString(std::wstring& str)
{
    // Restricted by system: \ / : * ? " < > |
    // Restricted by choice: " - "

    for (int i = 0; i < str.size(); i++)
    {
        if (whereSubStr(str, L"\\") == str.size() - 1) replaceAllSubStr(str, L"\\", L"");
        else replaceAllSubStr(str, L"\\", L"; ");

        if (whereSubStr(str, L"/") == str.size() - 1) replaceAllSubStr(str, L"/", L"");
        else replaceAllSubStr(str, L"/", L"; ");

        replaceAllSubStr(str, L":", L";");
        replaceAllSubStr(str, L"*", L"");
        replaceAllSubStr(str, L"?", L"");
        replaceAllSubStr(str, L"\"", L"'");
        replaceAllSubStr(str, L"<", L"");
        replaceAllSubStr(str, L">", L"");
        replaceAllSubStr(str, L"|", L"");

        replaceAllSubStr(str, L" - ", L"; ");
    }
}

void MainFrame::ValidateTrackTitles()
{
    SetStatusText("Analysing track titles in terms of forbidden & unwanted characters...");
    for (int i = 0; i < trackTitles.size(); i++) ValidateFilesystemString(trackTitles[i]);

    SetTracksField();
    SetStatusText("Track titles validated");
}


void MainFrame::SetTracksField()
{
    tracks_Field->textField->Clear();
    for (int i = 0; i < trackTitles.size(); i++)
    {
        tracks_Field->textField->WriteText(NumToStr(i + 1) + L". " + artist + L" - " + trackTitles[i] + (wchar_t)'\n');
    }
}

void MainFrame::ResetTracksFile()
{
    FILE* tracksFile = nullptr;
    std::string path = "tracks";
    fopen_s(&tracksFile, path.c_str(), "w");
    if (tracksFile != nullptr) fclose(tracksFile);
}

void MainFrame::LoadTrackTitles()
{
    tracks_Field->textField->Clear();

    FILE* tracksFile = nullptr;
    std::string path = "tracks";
    fopen_s(&tracksFile, path.c_str(), "r");
    if (tracksFile != nullptr)
    {
        wchar_t currentChar;
        std::wstring currentWord = L"";
        do
        {
            currentChar = getc(tracksFile);
            
            // The file is UTF-8 encoded
            // wchar_t is 16-bit on Windows, so it can represent only 3-byte UTF-8 encoded characters:
            // 
            // 1-Byte encoding: (7-bit values) up to 0x7F (126)
            //  6543210
            // 0xxxxxxx
            // 
            // 2-Byte encoding: (11-bit values) up to 0x7FF (2047)
            //    09876   543210
            // 110xxxxx 10xxxxxx
            // 
            // 3-Byte encoding: (16-bit values) up to 0xFFFF (65535)
            //     5432   109876   543210
            // 1110xxxx 10xxxxxx 10xxxxxx

            // 4-Byte encoding (not supported)
            // 11110xxx ...

            if ((currentChar & 128) == 128 && (currentChar & 64) == 64)
            {
                if ((currentChar & 32) == 0)
                {
                    // 2-Byte encoded character
                    // 110xxxxx ...
                    unsigned char Byte[] = { (unsigned char)currentChar & 31, getc(tracksFile) & 63 };

                    // 16-bit decoded character:
                    currentChar = (Byte[1]) + (((unsigned short)Byte[0] << 8) >> 2);
                }
                else if ((currentChar & 32) == 32)
                {
                    if ((currentChar & 16) == 0)
                    {
                        // 3-Byte encoded character
                        // 1110xxxx ...
                        unsigned char Byte[] = { (unsigned char)currentChar & 15, getc(tracksFile) & 63, getc(tracksFile) & 63 };

                        // 16-bit decoded character:
                        currentChar = (Byte[2]) + (((unsigned short)Byte[1] << 8) >> 2) + (((unsigned int)Byte[0] << 16) >> 4);
                    }
                    else if ((currentChar & 16) == 16)
                    {
                        if ((currentChar & 8) == 0)
                        {
                            //4-Byte encoded character
                            // 11110xxx ...
                            for (int i = 0; i < 3; i++) getc(tracksFile);

                            // 16-bit substitue character:
                            currentChar = 'e';
                        }
                    }
                }
            }
            
            
            if (iswprint(currentChar))
            {
                currentWord += currentChar;
            }
            else
            {
                if (currentWord.size() > 0)
                {
                    trackTitles.push_back(currentWord);
                    SetTracksField();
                }
                currentWord = L"";
            }
        } while(currentChar != (wchar_t)EOF);

        SetStatusText("Track titles loaded");
        fclose(tracksFile);
    }
    else
    {
        SetStatusText("Failed to load track titles from file");
    }
}



int GetResource(const char* host, const char* resource, const char* outputFilename)
{
    // Figure out what does the "Retrieves configuration from the registry" mean for INTERNET_OPEN_TYPE_PRECONFIG
    // dwAccessType: INTERNET_OPEN_TYPE_PRECONFIG or INTERNET_OPEN_TYPE_DIRECT
    // dwFlags: INTERNET_FLAG_ASYNC or NULL
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
    if (hInternet == NULL)
    {
        PrintConsole("InternetOpen() failed: " + std::to_string(GetLastError()) + "\n");

        return 1;
    }


    HINTERNET hConnection = InternetConnectA(hInternet, host, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP,
                                             INTERNET_FLAG_SECURE, INTERNET_NO_CALLBACK);
    if (hConnection == NULL)
    {
        PrintConsole("InternetConnection() failed: " + std::to_string(GetLastError()) + "\n");
        InternetCloseHandle(hInternet);

        return 2;
    }


    const char* acceptTypes[] = { "text/*", "image/avif", "image/webp", "image/apng", "*/*", NULL };
    // Investigate flags
    HINTERNET hRequest = HttpOpenRequestA(hConnection, "GET", resource, "HTTP/1.1", NULL, acceptTypes,
                                          INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_SECURE, INTERNET_NO_CALLBACK);
    if (hRequest == NULL)
    {
        PrintConsole("HttpOpenRequest() failed: " + std::to_string(GetLastError()) + "\n");
        InternetCloseHandle(hConnection);
        InternetCloseHandle(hInternet);

        return 3;
    }

    // lpszHeaders: "host: en.wikipedia.org" or NULL
    BOOL bResult;
    bResult = HttpSendRequestA(hRequest, NULL, 0, NULL, 0);
    if (bResult != TRUE)
    {
        PrintConsole("HttpSendRequest() failed: " + std::to_string(GetLastError()) + "\n");
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnection);
        InternetCloseHandle(hInternet);

        return 4;
    }

    unsigned long totalBytesRead = 0;
    bool bContinue = true;

    FILE* outputFile = nullptr;
    fopen_s(&outputFile, outputFilename, "w");
    if (outputFile == nullptr)
    {
        PrintConsole("Failed to open: " + std::string(outputFilename) + "\n");
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnection);
        InternetCloseHandle(hInternet);

        return 9;
    }
    fclose(outputFile);

    PrintConsole("Bytes available:\n");
    while (bContinue)
    {
        unsigned long bytes = 0;
        bResult = InternetQueryDataAvailable(hRequest, &bytes, NULL, NULL);
        if (bResult != TRUE)
        {
            PrintConsole("InternetQueryDataAvailable() failed: " + std::to_string(GetLastError()) + "\n");
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hInternet);

            return 5;
        }
        if (bytes == 0)
        {
            bContinue = false;
            break;
        }
        PrintConsole(std::to_string(bytes) + ' ');


        unsigned char* buf = (unsigned char*)calloc(bytes, sizeof(unsigned char));
        if (buf == nullptr)
        {
            PrintConsole("Failed to allocate memory.\n");
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hInternet);

            return 6;
        }

        unsigned long bytesRead;
        bResult = InternetReadFile(hRequest, buf, bytes, &bytesRead);
        if (bResult != TRUE)
        {
            free(buf);
            PrintConsole("InternetReadFile() failed: " + std::to_string(GetLastError()) + "\n");
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hInternet);

            return 7;
        }

        FILE* outputFile = nullptr;
        fopen_s(&outputFile, outputFilename, "a");
        if (outputFile == nullptr)
        {
            free(buf);
            PrintConsole("Failed to open: " + std::string(outputFilename) + "\n");
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hInternet);

            return 8;
        }

        for (int i = 0; i < bytesRead; i++)
        {
            putc(buf[i], outputFile);
        }
        fclose(outputFile);


        totalBytesRead += bytesRead;
        free(buf);
    }


    PrintConsole("\nBytes read: " + std::to_string(totalBytesRead) + '\n');


    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnection);
    InternetCloseHandle(hInternet);
    PrintConsole("Success.\n");

    return 0;
}

int GetThumbnailURL(std::string* returnURL, const char* inputFilename)
{
    FILE* resourceFile = nullptr;
    fopen_s(&resourceFile, inputFilename, "r");
    if (resourceFile == nullptr)
    {
        PrintConsole("Failed to open: " + std::string(inputFilename) + "\n");
        return 1;
    }

    char currentChar;
    std::string currentWord = "";
    std::string searchQuery[] = { "PlaylistThumbnail", "\"url\":\"", "maxresdefault.jpg", "\\u0026" };
    unsigned int count = 0;

    unsigned int currentIndex = 0;
    unsigned int queryIndex = 0;
    do
    {
        currentChar = getc(resourceFile);

        if (searchQuery[queryIndex] == "maxresdefault.jpg")
        {
            if (currentChar == '\"')
            {
                currentIndex = 0;
                bool bFound = false;
                std::string testWord = "";
                for (int j = 0; j < currentWord.size(); j++)
                {
                    if (currentWord[j] == searchQuery[queryIndex][currentIndex])
                    {
                        testWord += currentWord[j];
                        if (testWord == searchQuery[queryIndex])
                        {
                            bFound = true;
                            queryIndex++;
                            break;
                        }
                        else
                        {
                            currentIndex++;
                        }
                    }
                    else
                    {
                        testWord = "";
                        currentIndex = 0;
                    }
                }

                if (bFound)
                {
                    break;
                }
                else
                {
                    currentWord = "";
                    currentIndex = 0;
                    queryIndex--;
                }
            }
            else
            {
                currentWord += currentChar;
            }
        }
        else if (currentChar == searchQuery[queryIndex][currentIndex])
        {
            currentWord += currentChar;
            if (currentWord == searchQuery[queryIndex])
            {
                currentWord = "";
                currentIndex = 0;

                queryIndex++;
            }
            else
            {
                currentIndex++;
            }
        }
        else
        {
            currentWord = "";
            currentIndex = 0;
        }
    } while (currentChar != EOF);

    fclose(resourceFile);

    ///*
    std::string target = currentWord;
    currentWord = "";
    currentIndex = 0;
    count = 0;
    std::string targetCorrect = "";
    for (int i = 0; i < target.size(); i++)
    {
        targetCorrect += target[i];
        if (target[i] == searchQuery[queryIndex][currentIndex])
        {
            currentWord += target[i];
            if (currentWord == searchQuery[queryIndex])
            {
                currentWord = "";
                currentIndex = 0;

                count++;

                for (int j = 0; j < searchQuery[queryIndex].size(); j++)
                {
                    targetCorrect.pop_back();
                }
                targetCorrect += '&';
            }
            else
            {
                currentIndex++;
            }
        }
        else
        {
            currentIndex = 0;
            currentWord = "";
        }
    }

    *returnURL = targetCorrect;
    PrintConsole("target:\n" + target + "\n\n");
    PrintConsole("targetCorrect:\n" + targetCorrect + "\n\n");
    PrintConsole("queryIndex: " + std::to_string(queryIndex) + '\n');
    PrintConsole("count: " + std::to_string(count) + '\n');
    //*/

    return 0;
}








unsigned int fromSynchsafe(unsigned char synchsafe[4])
{
    unsigned int return_value = 0;
    for (int i = 0; i < 4; i++)
    {
        return_value += (synchsafe[4 - 1 - i] << 7 * i);
    }
    return return_value;
}

void toSynchsafe(unsigned int data, unsigned char* ptr)
{
    for (int i = 0; i < 4; i++)
    {
        ptr[4 - 1 - i] = (data >> 7 * i) & 127;
    }
}







int FindTheTag(const wchar_t* filename, unsigned int* returnTagStartIndex = nullptr, unsigned int* returnSizeMinusHeader = nullptr,
               std::string* returnTag = nullptr)
{
    std::string fileData;
    if (GetFileData(filename, &fileData)) return 1;


    bool bFound = false;
    unsigned int tagStartIndex;

    std::string tagHeader = "ID3";
    unsigned int tagHeaderIndex = 0;
    for (int i = 0; i < fileData.size(); i++)
    {
        if (!bFound)
        {
            if (fileData[i] == tagHeader[tagHeaderIndex])
            {
                if (tagHeaderIndex == 0)
                {
                    tagStartIndex = i;
                }

                tagHeaderIndex++;
                if (tagHeaderIndex == tagHeader.size())
                {
                    bFound = true;
                }
            }
            else
            {
                tagHeaderIndex = 0;
            }
        }
        else
        {
            tagHeader += fileData[i];
            if (tagHeader.size() >= 10) break;
        }
    }


    if (!bFound)
    {
        PrintConsole("Failed to find the ID3 tag.\n");
        return 1;
    }

    PrintConsole("Found the ID3 tag at:\t" + tagStartIndex + '\n');
    PrintConsole("Header (in hex):\t");
    for (int i = 0; i < 10; i++)
    {
        if (i != 0) PrintConsole(" ");
        PrintConsole(HexToStr((unsigned char)tagHeader[i]));
    }

    unsigned char tagSizeMinusHeaderSynchsafe[4] = { 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        tagSizeMinusHeaderSynchsafe[4 - 1 - i] = tagHeader[10 - 1 - i];
    }
    unsigned int tagSizeMinusHeader = fromSynchsafe(tagSizeMinusHeaderSynchsafe);
    PrintConsole("\ntagSize (in dec):\t" + std::to_string(tagSizeMinusHeader) + "\n\n");



    std::string tag = "";
    for (int i = 0; i < tagHeader.size() + tagSizeMinusHeader; i++)
    {
        tag += fileData[tagStartIndex + i];
    }

    if (returnTagStartIndex != nullptr) *returnTagStartIndex = tagStartIndex;
    if (returnSizeMinusHeader != nullptr) *returnSizeMinusHeader = tagSizeMinusHeader;

    if (returnTag != nullptr) *returnTag = tag;
    return 0;
}


int UnsynchroData(std::string data, std::string* returnData)
{
    std::string newData = "";
    for (int i = 0; i < data.size(); i++)
    {
        newData += data[i];
        if ((unsigned char)data[i] == (unsigned char)0xFF)
        {
            if (i + 1 < data.size())
            {
                if ((data[i + 1] & 224) == 224 || (unsigned char)data[i + 1] == (unsigned char)0x00) newData += '\0';
            }
        }
    }

    *returnData = newData;
    return 0;
}

int ConstructPictureFrame(const wchar_t* filename, std::string* returnFrame,
                          std::string* returnFrameHeader = nullptr,
                          std::string* returnInQuestionData = nullptr,
                          std::string* returnPictureData = nullptr)
{
    // Constructing the APIC frame
    // frame = frameHeader + inQuestionData + pictureDataUnsynchronised;

    // 1) pictureData
    std::string pictureData = "";
    if (GetFileData(filename, &pictureData)) return 1;

    std::string pictureDataUnsynchronised;
    UnsynchroData(pictureData, &pictureDataUnsynchronised);



    // 2) inQuestionData
    std::string inQuestionData = "";



    std::string MIME_type = "image/png";	// the MIME media type

    unsigned char pictureType = 0x03; // stands for front cover

    unsigned char frameDataSizeSynchsafe[4];
    toSynchsafe(pictureData.size() + ((1) + (MIME_type.size() + 1) + (1 + 1)), frameDataSizeSynchsafe);		// raw picture data size in synchsafe 32-bit int


    // Write fields to the data and append null-termination to each field
    for (int i = 0; i < 4; i++)
    {
        inQuestionData += (unsigned char)frameDataSizeSynchsafe[i];
    }
    inQuestionData += '\0';

    for (int i = 0; i < MIME_type.size(); i++)
    {
        inQuestionData += (unsigned char)MIME_type[i];
    }
    inQuestionData += '\0';

    inQuestionData += pictureType;
    inQuestionData += '\0';



    // 3) frameHeader
    std::string frameHeader = "";
    frameHeader += "APIC";
    for (int i = 0; i < 4; i++)
    {
        frameHeader += (unsigned char)0x00;
    }
    frameHeader += (unsigned char)0x00;
    frameHeader += (unsigned char)0x03;	// (n, p) frame header format flags
    //frameHeader += (unsigned char)0x00;	// test for no flags

    unsigned int frameSize = frameHeader.size() + inQuestionData.size() + pictureDataUnsynchronised.size();

    // Frame header requires the (frameSize - frameHeaderSize) in the header
    // Check if the size does not exceed the 28-bit slot
    if (frameSize - frameHeader.size() > power(2, 28) - 1)
    {
        PrintConsole("Frame size exceeds the limit.\n");
        return 1;
    }

    unsigned char frameSizeSynchsafeMinusHeader[4] = { 0, 0, 0, 0 };	// needed for frame header
    toSynchsafe(frameSize - frameHeader.size(), frameSizeSynchsafeMinusHeader);
    for (int i = 0; i < 4; i++)
    {
        frameHeader[4 + i] = frameSizeSynchsafeMinusHeader[i];
    }






    std::string frame = "";
    frame += frameHeader + inQuestionData + pictureDataUnsynchronised;

    // Output for testing
    if (returnFrameHeader != nullptr) *returnFrameHeader = frameHeader;
    if (returnInQuestionData != nullptr) *returnInQuestionData = inQuestionData;
    if (returnPictureData != nullptr) *returnPictureData = pictureDataUnsynchronised;


    *returnFrame = frame;
    return 0;
}



int AppendFrameToTag(std::string tag, std::string frame, std::string* returnTag)
{
    // Finding the padding start
    bool bPaddingFound = false;
    unsigned int paddingStart;

    unsigned int tagHeaderOffset = 10;
    unsigned int frameHeaderOffset = 10;
    unsigned int frameHeaderID_Offset = 4;
    unsigned int frameHeaderSizeOffset = 4;

    unsigned int index = tagHeaderOffset + frameHeaderID_Offset + frameHeaderSizeOffset;
    while (index < tag.size())
    {
        PrintConsole("index: " + std::to_string(index) + "\n");
        unsigned char frameSizeMinusHeaderSynchsafe[4] = { tag[index - 4], tag[index - 3], tag[index - 2], tag[index - 1] };
        unsigned int frameSizeMinusHeader = fromSynchsafe(frameSizeMinusHeaderSynchsafe);

        index -= frameHeaderSizeOffset;
        PrintConsole("index: " + std::to_string(index) + "\n");
        index -= frameHeaderID_Offset;
        PrintConsole("index: " + std::to_string(index) + "\n");

        index += frameHeaderOffset;
        PrintConsole("index: " + std::to_string(index) + "\n");
        index += frameSizeMinusHeader;
        PrintConsole("index: " + std::to_string(index) + "\n");
        // To this point frame size is assumed to not exceed the tag size
        // check this just in case:
        if (index >= tag.size()) break;

        // Next frame or padding
        bool bPadding = true;
        for (int i = index; index + i < tag.size(); i++)
        {
            if (tag[index + i] != '\0')
            {
                bPadding = false;
                break;
            }
        }
        if (!bPadding)
        {
            index += frameHeaderID_Offset;
            index += frameHeaderSizeOffset;
        }
        else
        {
            bPaddingFound = true;
            paddingStart = index;
            break;
        }
        PrintConsole("\n\n");
    }

    // Add only the old tag data without padding
    std::string newTag = "";

    if (bPaddingFound)
    {
        PrintConsole("Padding found!\n");
        PrintConsole("Tag without padding:\n");
        for (int i = 0; i < paddingStart; i++)
        {
            newTag += tag[i];

            PrintConsole(HexToStr((unsigned char)tag[i]));
            if ((i + 1) % 16 == 0) PrintConsole("\n");
            else if (i != paddingStart - 1) PrintConsole(" ");
            else PrintConsole("\n");
        }
    }
    else
    {
        PrintConsole("Padding not found!\n");
        PrintConsole("Tag:\n");
        for (int i = 0; i < tag.size(); i++)
        {
            newTag += tag[i];

            PrintConsole(HexToStr((unsigned char)tag[i]));
            if ((i + 1) % 16 == 0) PrintConsole("\n");
            else if (i != tag.size() - 1) PrintConsole(" ");
            else PrintConsole("\n");
        }
    }

    // Append the new frame
    for (int i = 0; i < frame.size(); i++)
    {
        newTag += frame[i];
    }

    // Add 2048 bytes of padding
    for (int i = 0; i < 2048; i++)
    {
        newTag += '\0';
    }

    // Change the tag size value in header
    unsigned int newTagSizeMinusHeader = newTag.size() - 10;
    unsigned char newTagSizeMinusHeaderSynchsafe[4];
    toSynchsafe(newTagSizeMinusHeader, newTagSizeMinusHeaderSynchsafe);
    // 012 34 5 6789
    for (int i = 6; i < 10 && i < newTag.size(); i++)
    {
        newTag[i] = newTagSizeMinusHeaderSynchsafe[i - 6];
    }

    *returnTag = newTag;
    return 0;
}

int UpdateTagSize(std::string tag, std::string* returnTag)
{
    if (tag.size() < 10) return 1;
    unsigned int newSizeMinusHeader = tag.size() - 10;
    unsigned char newSizeMinusHeaderSynchsafe[4];
    toSynchsafe(newSizeMinusHeader, newSizeMinusHeaderSynchsafe);

    for (int i = 0; i < 4 && 6 + i < tag.size(); i++)
    {
        tag[6 + i] = newSizeMinusHeaderSynchsafe[i];
    }

    *returnTag = tag;
    return 0;
}

int ReplaceTag(const wchar_t* filename, std::string tag, const wchar_t* outputFilename = nullptr)
{
    std::string fileData;
    if (GetFileData(filename, &fileData)) return 1;

    std::string newFileData = "";

    unsigned int oldTagStartIndex;
    std::string oldTag;
    FindTheTag(filename, &oldTagStartIndex, nullptr, &oldTag);
    for (int i = 0; i < oldTagStartIndex; i++)
    {
        newFileData += fileData[i];
    }

    for (int i = 0; i < tag.size(); i++)
    {
        newFileData += tag[i];
    }

    for (int i = oldTagStartIndex + oldTag.size(); i < fileData.size(); i++)
    {
        newFileData += fileData[i];
    }


    if (outputFilename != nullptr) WriteDataToFile(newFileData, outputFilename);
    else WriteDataToFile(newFileData, filename);

    return 0;
}

int writeArtwork(const wchar_t* audioFilename, const wchar_t* artworkFilename, const wchar_t* outputAudioFilename = nullptr)
{
    unsigned int oldTagSizeMinusHeader, foundAtIndex;
    std::string tag;
    FindTheTag(audioFilename, &foundAtIndex, &oldTagSizeMinusHeader, &tag);




    std::string newFrame;

    std::string frameHeader;
    std::string inQuestionData;
    std::string pictureData;

    if (ConstructPictureFrame(artworkFilename, &newFrame, &frameHeader, &inQuestionData, &pictureData)) return 1;



    unsigned int newTagSizeMinusHeader = oldTagSizeMinusHeader + newFrame.size();
    unsigned char newTagSizeMinusHeaderSynchsafe[4] = { 0, 0, 0, 0 };
    toSynchsafe(newTagSizeMinusHeader, newTagSizeMinusHeaderSynchsafe);
    PrintConsole("newTagSize (synchsafe):\n\t\t\t");
    for (int i = 0; i < 4; i++)
    {
        if (i != 0) PrintConsole(" ");
        PrintConsole(HexToStr(newTagSizeMinusHeaderSynchsafe[i]));
    }
    PrintConsole("\n\n");

    PrintConsole("newTagSize (in dec):\t" + std::to_string(newTagSizeMinusHeader) + "\n");
    PrintConsole("oldTagSize (in dec):\t" + std::to_string(oldTagSizeMinusHeader) + "\n");
    PrintConsole(" frameSize (in dec):\t" + std::to_string(newFrame.size()) + "\n");
    PrintConsole("\n");

    PrintConsole(" frameSize (in dec):\t" + std::to_string(newFrame.size()) + "\n");
    PrintConsole("     Check (in dec):\t" + std::to_string(frameHeader.size() + inQuestionData.size() + pictureData.size()) + "\n");
    PrintConsole("         - (in dec):\t" + std::to_string(frameHeader.size()) + "\n");
    PrintConsole("         - (in dec):\t" + std::to_string(inQuestionData.size()) + "\n");
    PrintConsole("         - (in dec):\t" + std::to_string(pictureData.size()) + "\n\n");


    PrintConsole("Tag:\n");
    for (int i = 0; i < tag.size(); i++)
    {
        PrintConsole(HexToStr((unsigned char)tag[i]));
        if ((i + 1) % 16 == 0) PrintConsole("\n");
        else if (i != tag.size() - 1) PrintConsole(" ");
        else PrintConsole("\n");
    }

    std::string newTag;
    AppendFrameToTag(tag, newFrame, &newTag);

    // switch the unsynchro flag:
    unsigned int flagIndex = 3 + 2;
    if (flagIndex < newTag.size()) newTag[flagIndex] += 0x80;
    else
    {
        PrintConsole("Failed to switch the unsynchro tag flag.\n");
        return 1;
    }
    PrintConsole("Flag & Size test:\n");
    for (int i = 0; i < newTag.size(); i++)
    {
        if (i == 0x40)
        {
            break;
        }

        if (i == 0 || i % 16 == 0) PrintConsole(HexToStr(i, 8) + ": ");
        PrintConsole(HexToStr((unsigned char)newTag[i]));
        if ((i + 1) % 16 == 0) PrintConsole("\n");
        else if (i != newTag.size() - 1) PrintConsole(" ");
        else PrintConsole("\n");
    }
    PrintConsole("\n");


    if (outputAudioFilename == nullptr) outputAudioFilename = audioFilename;
    ReplaceTag(audioFilename, newTag, outputAudioFilename);
    PrintConsole("\nDone!\n");


    std::string outputCheck;
    if (GetFileData(outputAudioFilename, &outputCheck)) return 1;
    for (int i = 0; i < outputCheck.size(); i++)
    {
        if (i == 0x40)
        {
            break;
        }

        if (i == 0 || i % 16 == 0) PrintConsole(HexToStr(i, 8) + ": ");
        PrintConsole(HexToStr((unsigned char)outputCheck[i]));
        if ((i + 1) % 16 == 0) PrintConsole("\n");
        else if (i != outputCheck.size() - 1) PrintConsole(" ");
        else PrintConsole("\n");
    }

    return 0;
}


void printHex(std::string data, std::string stopQuery)
{
    bool bStop = false;
    for (int i = 0; i < data.size(); i++)
    {
        if (i == 0 || i % 16 == 0) PrintConsole(HexToStr(i, 8) + ": ");
        PrintConsole(HexToStr((unsigned char)data[i]));
        if ((i + 1) % 16 == 0)
        {
            PrintConsole("\n");
            if (bStop)
            {
                //PrintConsole("Found: ");
                //for (int j = 0; j < stopQuery.size(); j++) PrintConsole(HexToStr((unsigned char)stopQuery[j]));
                //PrintConsole("\n");
                while (getc(stdin) != 'x') { }
                //PrintConsole("\n");
                bStop = false;
                //break;
            }
        }
        else if (i != data.size() - 1) PrintConsole(" ");
        else PrintConsole("\n");


        for (int j = 0; j < stopQuery.size() && i + j < data.size(); j++)
        {
            if ((unsigned char)data[i + j] != (unsigned char)stopQuery[j])
            {
                break;
            }

            if (j == stopQuery.size() - 1) bStop = true;
        }
    }
}



void MainFrame::AttachArtwork(std::wstring audioFile, std::wstring artworkFile)
{
    static unsigned int index = 1;

    artworkFile = workingDirectory + artworkFile;

    PrintConsole(std::to_wstring(index) + L": " + audioFile.c_str() + L"\n");
    PrintConsole(std::to_wstring(index) + L": " + artworkFile.c_str() + L"\n\n");

    writeArtwork(audioFile.c_str(), artworkFile.c_str());

    PrintConsole("---------------------------\n");
    index++;
}
