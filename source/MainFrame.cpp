#include "MainFrame.h"
#include "network_utils.h"
#include "tag_utils.h"


// ID's for GUI-elements
enum
{
    ID_Save = 1,
    ID_albumsDir_Field,
    ID_workingDir_Field,
    ID_artist_Field,
    ID_albumName_Field,
    ID_albumYear_Field,
    ID_output_Field,
    ID_URL_Field,
    ID_Button,
    ID_Frame,
    ID_AlertOnDone,
    ID_URL_Artwork_Field
};


void MainFrame::InitGUI()
{
    // default sizes and pos because it's automatically stretched to the frame anyway
    wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2621440L, "Main Panel");


    albumsDir_Field = new TextBox("Albums directory:", ID_albumsDir_Field,
                                  wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel, false,
                                  labelOffset, mainOffset, fieldBetweenSpace);
    workingDir_Field = new TextBox("Working directory:", ID_workingDir_Field,
                                   wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel, false,
                                   labelOffset, mainOffset, fieldBetweenSpace);

    mainOffset.y += fieldBetweenSpace.y;
    artist_Field = new TextBox("Artist:", ID_artist_Field,
                               wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel, false,
                               labelOffset, mainOffset, fieldBetweenSpace);

    albumName_Field = new TextBox("Album name:", ID_albumName_Field,
                                  wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel, false,
                                  labelOffset, mainOffset, fieldBetweenSpace);

    albumYear_Field = new TextBox("Album year:", ID_albumYear_Field,
                                  wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel, false,
                                  labelOffset, mainOffset, fieldBetweenSpace);

    URL_Field = new TextBox("Playlist URL:", ID_URL_Field,
                            wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel, false,
                            labelOffset, mainOffset, fieldBetweenSpace);


    URL_Artwork_Field = new TextBox("Playlist URL with proper artwork:", ID_URL_Artwork_Field,
                                    wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel, false,
                                    labelOffset, mainOffset, fieldBetweenSpace);




    wxButton* button = new wxButton(mainPanel, ID_Button, "Run",
                                    wxPoint(mainOffset.x, mainOffset.y),
                                    ButtonSize, NULL, wxDefaultValidator, "Run button");

    checkAlert = new wxCheckBox(mainPanel, ID_AlertOnDone, "Alert on done",
                                wxPoint(mainOffset.x + ButtonSize.x + fieldBetweenSpace.x, mainOffset.y),
                                ButtonSize, 0, wxDefaultValidator, "Alert checkbox");
    mainOffset.y += ButtonSize.y + fieldBetweenSpace.y;



    mainOffset.y += fieldBetweenSpace.y;
    output_Field = new TextBox("Output:", ID_output_Field,
                               wxPoint(mainOffset.x, mainOffset.y), LargeBoxSize, mainPanel, true,
                               labelOffset, mainOffset, fieldBetweenSpace);
}

void MainFrame::InitBindings()
{
    Bind(wxEVT_BUTTON, &MainFrame::OnButtonPress, this, ID_Button);


    Bind(wxEVT_MENU, &MainFrame::OnSave, this, ID_Save);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
}

void MainFrame::InitControls()
{
    InitGUI();

    ClientWidth = mainOffset.x + TextBoxSize.x + mainOffset.x;
    ClientHeight = mainOffset.y;

    InitBindings();


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
}

void MainFrame::InitValues()
{
    // FROM GLOBAL VARS
    bDone = true;

    mainOffset = wxSize(20, 40);
    fieldBetweenSpace = wxSize(10, 20);

    TextBoxSize = wxSize(800, 20);
    LargeBoxSize = wxSize(800, 500);
    ButtonSize = wxSize(100, 25);

    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;


    resourceFilename = "index.html";
    thumbnailURL = "";
    consoleLogFilepath = L"log";
    consoleOutputBuf.clear();

    mainConsole = nullptr;
    mainConsole = new Console(consoleLogFilepath, &consoleOutputBuf);
    tag::SetConsole(mainConsole);
    net::SetConsole(mainConsole);
}

MainFrame::MainFrame() : wxFrame(NULL, ID_Frame, "album-dl")
{
    InitValues();
    InitControls();

    //output_Field->textField->Disable();
    output_Field->textField->SetEditable(false);


    // Sample test values for convenience
    ///*
    artist_Field->textField->SetValue("Big Black");
    albumName_Field->textField->SetValue("Lungs");
    albumYear_Field->textField->SetValue("1982");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_lSCRmY_Qw8RCNnMKHcp05O1K8fAIyqLjs");
    URL_Artwork_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_lSCRmY_Qw8RCNnMKHcp05O1K8fAIyqLjs");
    //*/

    /*
    artist_Field->textField->SetValue("O.S.T.R.");
    albumName_Field->textField->SetValue("Tylko Dla Doros³ych");
    albumYear_Field->textField->SetValue("2010");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=PLIKxxmyVA3HZ5vCNl3b0gQXDhuMWLz-mG");
    URL_Artwork_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_l6DSlExq2EbVR7ILChbL9ZHn-1SbyKRO8");
    */

    artist_Field->textField->SetFocus();
}

MainFrame::~MainFrame()
{
    //if (mainConsole != nullptr) delete mainConsole;
    if (consoleThread.joinable()) consoleThread.join();
}



void MainFrame::OnClose(wxCloseEvent& event)
{
    //SaveSettings();

    Destroy();
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    if (consoleThread.joinable()) consoleThread.join();
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    std::string output = "";

#define VER 1.2
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

void MainFrame::OnButtonPress(wxCommandEvent& event)
{
    if (!bDone) return;

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



    std::vector<std::wstring> validPlaylistURLs ={
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

    std::vector<std::wstring> validArtworkURLs ={
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



    bDone = false;
    if (consoleThread.joinable()) consoleThread.join();
    consoleThread = std::move(std::thread(&MainFrame::GetAlbum, this));
}





void MainFrame::ExecuteBatchSession(bool addPadding)
{
    std::thread sub_thread(&Console::RunSession, mainConsole);
    mainConsole->bConsoleDone = false;

    bool bLastIter = false;
    for (;;)
    {
        {
            std::lock_guard<std::mutex> bufLock(mainConsole->outputBufMutex);
            output_Field->AddFromStream(consoleOutputBuf);
        }

        if (bLastIter) break;
        if (mainConsole->bConsoleDone) bLastIter = true;
    }

    sub_thread.join();
    mainConsole->TrashCmds();

    if (addPadding) mainConsole->PrintLogAndConsoleNarrow("\n\n");
}

void MainFrame::GetAlbum()
{
    output_Field->SetText(L"");
    SetStatusText("Running the script...");
    /*
    mainConsole->AddCmd(DownloadStage());
    mainConsole->AddCmd(ConvertStage());
    mainConsole->AddCmd(CreateTrashDirStage());
    mainConsole->AddCmd(RemoveLeftoverStage());
    ExecuteBatchSession();
    
    
    //--------------------------------------------------
    ResetTracksFile();

    mainConsole->AddCmd(GetTitlesStage());
    ExecuteBatchSession();

    LoadTrackTitles();
    ValidateTrackTitles();
    ResetTracksFile();
    
    
    //--------------------------------------------------
    mainConsole->AddCmd(RenameFilesStage());
    ExecuteBatchSession();
    
    */
    //--------------------------------------------------
    GetArtworkPre();

    mainConsole->AddCmd(GetArtworkStage());
    ExecuteBatchSession();

    GetArtworkPost();
    
    /*
    //--------------------------------------------------
    AttachArtworkToAll();

    mainConsole->AddCmd(CreateAlbumDirectoryStage());
    mainConsole->AddCmd(MoveAudioStage());
    mainConsole->AddCmd(MoveArtworkStage());
    //ExecuteBatchSession(false);
    ExecuteBatchSession();
    */
    
    // FIELDS VALUE RESET
    /*
    SetStatusText("Resetting");
    
    // Reset fields & set focus
    URL_Field->textField->SetValue("");
    URL_Artwork_Field->textField->SetValue("");
    artist_Field->textField->SetValue("");
    albumName_Field->textField->SetValue("");
    albumYear_Field->textField->SetValue("");

    artist_Field->textField->SetFocus();
    */

    
    
    SetStatusText("Done");
    if (checkAlert->GetValue() == true) MessageBoxA(NULL, "Script has finished.", "Done", MB_OK);

    bDone = true;
}







std::wstring MainFrame::DownloadStage()
{
    std::wstring args = L"";
    args += L"--config-location \"" + workingDirectory + configName + L"\"";
    args += ' ';
    args += L"-o \"" + workingDirectory + L"td8_index%(playlist_index)s.mp4\"" + " \"" + URL + "\"";

    std::wstring fullCommand = L""; fullCommand += workingDirectory + execName + ' ' + args;



    return fullCommand;
}

std::wstring MainFrame::ConvertStage()
{
    std::wstring cmd = L"forfiles /P \"" + workingDirBackslashes + L"\"" + L" /M td8_index*.mp4 /C \"cmd /u /c ffmpeg.exe -i @file -c:a mp3 -b:a 192k -ar 44100 @fname.mp3\"";
    return cmd;
}

std::wstring MainFrame::CreateTrashDirStage()
{
    // Execute mkdir in case /Trash/ doesn't exist
    std::wstring cmd = L"cmd /u /c \"MKDIR \"" + workingDirBackslashes + L"\\Trash" + L"\"\"";
    return cmd;
}

std::vector<std::wstring> MainFrame::RemoveLeftoverStage()
{
    std::vector<std::wstring> rv;
    rv.push_back(L"cmd /u /c \"MOVE \"" + workingDirBackslashes + L"\\td8_index*.mp4\" \"" + workingDirBackslashes + L"\\Trash\"\"");
    //rv.push_back(L"cmd /u /c \"MOVE \"" + workingDirBackslashes + L"\\Trash\\td8_index*.mp4\" \"" + workingDirBackslashes + L"\"\"");
    return rv;
}

std::wstring MainFrame::GetTitlesStage()
{
    std::wstring cmd = L""; cmd += workingDirectory + execName + ' ' + L"-e --print-to-file %(title)s \"tracks\" " + URL;
    return cmd;
}

std::vector<std::wstring> MainFrame::RenameFilesStage()
{
    std::vector<std::wstring> cmds;
    cmds.clear();
    
    unsigned int maxNumDigits = std::to_string(trackTitles.size()).size();
    
    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring num = L"";

        unsigned int numDigits = std::to_string(i + 1).size();
        for (int j = 0; j < maxNumDigits - numDigits; j++) num += L'0';
        num += std::to_wstring(i + 1);

        std::wstring cmd = L"";
        cmd += L"cmd /u /c \"RENAME \"" + workingDirBackslashes + L"\\td8_index" + num + L".mp3\" \"";
        cmd += std::to_string(i + 1) + L". " + artist + L" - " + trackTitles[i] + L".mp3\"\"";

        cmds.push_back(cmd);
    }

    return cmds;
}

void MainFrame::GetArtworkPre()
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

    mainConsole->PrintLogAndConsoleNarrow("Downloading album artwork...\n");
    mainConsole->PrintLogAndConsoleNarrow("host: " + host + '\n');
    mainConsole->PrintLogAndConsoleNarrow("resource: " + resource + '\n');
    mainConsole->PrintLogAndConsoleNarrow("\n\n");



    using namespace net;

    GetResource(host.c_str(), resource.c_str(), resourceFilename.c_str());
    thumbnailURL = "";
    GetThumbnailURL(&thumbnailURL, resourceFilename.c_str());
}

void MainFrame::GetArtworkPost()
{
    // Erase the resource .html file data
    FILE* resourceFile;
    fopen_s(&resourceFile, resourceFilename.c_str(), "w");
    if (resourceFile != nullptr) fclose(resourceFile);
}

std::wstring MainFrame::GetArtworkStage()
{
    std::wstring args = L"";
    args += L"-o \"" + workingDirectory + artworkFilename + "\" \"" + thumbnailURL + "\"";

    std::wstring fullCommand = L""; fullCommand += workingDirectory + execName + ' ' + args;
    return fullCommand;
}


std::wstring MainFrame::CreateAlbumDirectoryStage()
{
    std::wstring cmd = L"cmd /u /c \"MKDIR \"" + albumPathBackslashes + L"\"\"";
    return cmd;
}

std::wstring MainFrame::MoveAudioStage()
{
    std::wstring cmd = L"cmd /u /c \"MOVE \"" + workingDirBackslashes + L"\\*.mp3\" \"" + albumPathBackslashes + L"\"\"";
    return cmd;
}

std::wstring MainFrame::MoveArtworkStage()
{
    std::wstring cmd = L"cmd /u /c \"MOVE \"" + workingDirBackslashes + L"\\" + artworkFilename + L"\" \"" + albumPathBackslashes + L"\"\"";
    return cmd;
}

void MainFrame::AttachArtworkToAll()
{
    mainConsole->PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole->PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "AttachArtworkToAll()" "\n");
    mainConsole->PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");
    
    SetStatusText("Attaching cover metadata");
    mainConsole->PrintLogAndConsoleNarrow("Attaching cover metadata\n");

    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring trackFilepath = workingDirectory + std::to_wstring(i + 1) + L". " + artist + L" - " + trackTitles[i] + L".mp3";
        //std::wstring trackFilepath = albumsDirectory + std::to_wstring(i + 1) + L". " + artist + L" - " + trackTitles[i] + L".mp3";
        AttachArtwork(trackFilepath, artworkFilename);
    }

    SetStatusText("Finished attaching cover metadata");
    mainConsole->PrintLogAndConsoleNarrow("Finished ataching cover metadata\n");
    mainConsole->PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole->PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
}






void MainFrame::AttachArtwork(std::wstring audioFile, std::wstring artworkFile)
{
    static unsigned int index = 1;

    artworkFile = workingDirectory + artworkFile;

    mainConsole->PrintLogAndConsole(std::to_wstring(index) + L": " + audioFile.c_str() + L"\n");
    mainConsole->PrintLogAndConsole(std::to_wstring(index) + L": " + artworkFile.c_str() + L"\n\n");

    using namespace tag;
    writeArtwork(audioFile.c_str(), artworkFile.c_str());

    mainConsole->PrintLogAndConsoleNarrow("---------------------------\n");
    index++;
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
    output_Field->AddText(L"\n\n");

    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring current(NumToStr(i + 1) + L". " + artist + L" - " + trackTitles[i] + (wchar_t)'\n');
        output_Field->AddText(current);
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
    output_Field->textField->Clear();

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
                    unsigned char Byte[] ={ (unsigned char)currentChar & 31, getc(tracksFile) & 63 };

                    // 16-bit decoded character:
                    currentChar = (Byte[1]) + (((unsigned short)Byte[0] << 8) >> 2);
                }
                else if ((currentChar & 32) == 32)
                {
                    if ((currentChar & 16) == 0)
                    {
                        // 3-Byte encoded character
                        // 1110xxxx ...
                        unsigned char Byte[] ={ (unsigned char)currentChar & 15, getc(tracksFile) & 63, getc(tracksFile) & 63 };

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
        } while (currentChar != (wchar_t)EOF);

        SetStatusText("Track titles loaded");
        fclose(tracksFile);
    }
    else
    {
        SetStatusText("Failed to load track titles from file");
    }
}






void MainFrame::OpenSettings()
{
    std::string path = "settings";
    std::string encoded;
    if (GetFileData(toWide(path).c_str(), &encoded))
    {
        SetStatusText("Settings file not found");
        return;
    }
    encoded += EOF;

    std::wstring decoded = DecodeFromUTF8(encoded);



    std::wstring currentWord = L"";
    enum value_ID {
        albumsDir = 1,
        workingDir,
        windowX,
        windowY,
        alertDone,
        none
    };
    unsigned int currentId = albumsDir;

    for (auto currentChar : decoded)
    {
        if (iswprint(currentChar)) currentWord += currentChar;
        else
        {
            if (currentWord.size() > 0)
            {
                try
                {
                    if (currentId == albumsDir) albumsDir_Field->textField->SetValue(currentWord);
                    if (currentId == workingDir) workingDir_Field->textField->SetValue(currentWord);

                    if (currentId == windowX && isStrNum(currentWord)) SetPosition(wxPoint(std::stoi(currentWord), GetPosition().y));
                    if (currentId == windowY && isStrNum(currentWord)) SetPosition(wxPoint(GetPosition().x, std::stoi(currentWord)));

                    if (currentId == alertDone)
                    {
                        if (currentWord == "0") checkAlert->SetValue(false);
                        if (currentWord == "1") checkAlert->SetValue(true);
                    }
                }
                catch (std::exception& e)
                {
                    MessageDialog("OpenSettings():\nCaught an exception: " + std::string(e.what()), "Error");
                    ExitProcess(1);
                }
                catch (...)
                {
                    MessageDialog("OpenSettings():\nCaught an unknown exception.", "Error");
                    ExitProcess(1);
                }

                // useful for testing:
                //MessageDialog(decoded + L"\n\n\n" + currentWord);
                currentWord = L"";
                currentId++;
            }
        }
    }

    SetStatusText("Settings have beed loaded");
}

void MainFrame::SaveSettings()
{
    std::string path = "settings";

    std::wstring decoded = L"";
    decoded += albumsDir_Field->textField->GetValue().ToStdWstring() + L"\n";
    decoded += workingDir_Field->textField->GetValue().ToStdWstring() + L"\n";
    decoded += toWide(NumToStr(GetPosition().x)) + L"\n";
    decoded += toWide(NumToStr(GetPosition().y)) + L"\n";

    if (checkAlert->GetValue()) decoded += L'1';
    else decoded += L'0';

    std::string encoded = EncodeToUTF8(decoded);


    if (WriteDataToFile(encoded, toWide(path).c_str()))
    {
        SetStatusText("Cannot save settings");
        return;
    }
    SetStatusText("Settings have been saved");
}




