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


void MainFrame::InitValues()
{
    bDone = true;
    bResetFields = true;

    mainOffset = wxSize(20, 40);
    fieldBetweenSpace = wxSize(10, 20);

    TextBoxSize = wxSize(800, 20);
    LargeBoxSize = wxSize(800, 500);
    ButtonSize = wxSize(100, 25);

    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;


    converterExec = L"ffmpeg.exe";
    downloaderExec = L"yt-dlp.exe";
    configName = L"config.txt";
    resourceFilename = "index.html";
    consoleLogFilepath = L"log";
    artworkFilename = L"artwork.png";

    thumbnailURL = "";
    consoleOutputBuf.clear();

    mainConsole.InitConsole(consoleLogFilepath, &consoleOutputBuf, &printMutex, false);
    tag::SetConsole(&mainConsole);
    net::SetConsole(&mainConsole);

    uMaxOutputLines = 150;

    defaultPos.x = 720;
    defaultPos.y = 0;
}

void MainFrame::InitFields()
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

void MainFrame::InitDefaultSize()
{
    ClientWidth = mainOffset.x + TextBoxSize.x + mainOffset.x;
    ClientHeight = mainOffset.y;

    SetClientSize(ClientWidth, ClientHeight);
}

void MainFrame::InitThemes()
{
    unsigned long uForeground = 0xC0C0C0;
    unsigned long uBackground = 0xFF0000;
    output_Field->textField->SetForegroundColour(wxColour(uForeground));
    output_Field->textField->SetBackgroundColour(wxColour(uBackground));

    // July 3rd, 2023:
    // TO DO:
    // -Investigate encoding-font compatibility
    // -Implement window auto-scaling relative to screen resolution
    // -Implement GUI auto-scaling relative to window size
    // -Disable License/ReadMe dumping in the release build
    //

    // Notepad++:   Courier New, 10 | 0xFFFFFF, 0x1E1E1E
    // Console:     Terminal, 8x12  | 0xC0C0C0, 0x000000
    wxFont outputFont(wxFontInfo(wxSize(8, 12)).FaceName("Terminal"));
    //wxFont outputFont(wxFontInfo(wxSize(8, 16)).FaceName("Courier New").Bold());
    output_Field->textField->SetFont(outputFont);
    output_Field->fieldEncoding = CP852;
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

    //output_Field->textField->Disable();
    output_Field->textField->SetEditable(false);
}

void MainFrame::InitTestValues()
{
    bResetFields = false;
    // SAMPLE TEST VALUES FOR CONVENIENCE:

    // SHORT PLAYLIST
    /*
    artist_Field->textField->SetValue("Big Black");
    albumName_Field->textField->SetValue("Lungs");
    albumYear_Field->textField->SetValue("1982");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_lSCRmY_Qw8RCNnMKHcp05O1K8fAIyqLjs");
    */

    // RARE UNICODE CHAR THAT SHOWS DIFFERENCE BETWEEN WINDOWS-1250 AND WINDOWS-1252
    /*
    artist_Field->textField->SetValue("The Jesus Lizard");
    albumName_Field->textField->SetValue("Down");
    albumYear_Field->textField->SetValue("1994");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_kULt5j2pKzT5PtLz1RGW7EO-IWDwqVtHw");
    */

    // TITLES WITH PROBABLY ACCIDENTAL MOJIBAKE, USEFUL DURING ENCODING TESTING
    ///*
    artist_Field->textField->SetValue("Death in June");
    albumName_Field->textField->SetValue("Discriminate: A Compilation of Personal Choice 1981-97");
    albumYear_Field->textField->SetValue("1997");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_ll7VmeyNV0J4d4HroMPrLrRfBcjiLIVLo");
    //*/

    // TYPICAL UNICODE TITLES
    /*
    artist_Field->textField->SetValue("O.S.T.R.");
    albumName_Field->textField->SetValue("Tylko Dla Doros³ych");
    albumYear_Field->textField->SetValue("2010");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=PLIKxxmyVA3HZ5vCNl3b0gQXDhuMWLz-mG");
    URL_Artwork_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_l6DSlExq2EbVR7ILChbL9ZHn-1SbyKRO8");
    */

    // TYPICAL UNICODE TITLES
    /*
    artist_Field->textField->SetValue("Goat");
    albumName_Field->textField->SetValue("World Music");
    albumYear_Field->textField->SetValue("2012");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_nMsUDBQ3_Xsjdz62NkJ_g1HnEirKtRkZg");
    //URL_Artwork_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_nMsUDBQ3_Xsjdz62NkJ_g1HnEirKtRkZg");
    */

    //
}

MainFrame::MainFrame() : wxFrame(NULL, ID_Frame, "album-dl")
{
    InitValues();
    InitFields();

    InitThemes();
    InitBindings();
    InitControls();

    InitDefaultSize();
    InitTestValues();

    artist_Field->textField->SetFocus();

    SetPosition(wxPoint(defaultPos.x, defaultPos.y));   // SET WINDOW POS TO DEFAULT POS
    OpenSettings();                                     // LOAD SETTINGS (MAY REPOS WINDOW)
    Show(true);                                         // SHOW WINDOW
}

MainFrame::~MainFrame()
{
    // ExitProcess() exits the calling process and all it's threads
    ExitProcess(ERR_SUCCESS);
}



void MainFrame::OnClose(wxCloseEvent& event)
{
    //SaveSettings();

    Destroy();
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    // File / Exit
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    // Help / About
    wxMessageBox(GetReadMe(), "About", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnSave(wxCommandEvent& event)
{
    // File / Save settings
    SaveSettings();
    wxMessageBox("Settings have been saved.", "album-dl", wxOK | wxICON_NONE);
}

void MainFrame::OnButtonPress(wxCommandEvent& event)
{
    if (!bDone) return;

    if (!ValidateFields()) return;


    bDone = false;
    if (workingThread.joinable()) workingThread.join();
    workingThread = std::move(std::thread(&MainFrame::GetAlbum, this));

    if (outputThread.joinable()) outputThread.join();
    outputThread = std::move(std::thread(&MainFrame::UpdateOutput, this));
}





void MainFrame::UpdateOutput()
{
    bool bLastIter = false;
    for (;;)
    {
        {
            if (output_Field->textField->GetNumberOfLines() >= uMaxOutputLines)
            {
                output_Field->PopFirstLine();
            }

            std::lock_guard<std::mutex> bufLock(printMutex);
            output_Field->AddText(consoleOutputBuf);
            consoleOutputBuf.clear();
        }

        if (bLastIter) break;

        std::lock_guard<std::mutex> switchLock(doneSwitchMutex);
        if (bDone) bLastIter = true;
    }
}

void MainFrame::ExecuteBatchSession(bool addPadding)
{
    mainConsole.RunSession();
    mainConsole.TrashCmds();

    if (addPadding) mainConsole.PrintLogAndConsoleNarrow("\n\n");
}

void MainFrame::GetAlbum()
{
    if (bLog) mainConsole.OpenLog();

    output_Field->SetText(L"");
    SetStatusText("Running the script...");


    //mainConsole.AddCmd(DownloadStage(), WINDOWS1250);
    //ExecuteBatchSession();

    
    //--------------------------------------------------
    //ResetTracksFile();
    //mainConsole.AddCmd(GetTitlesStage(), WINDOWS1250);
    //ExecuteBatchSession();

    LoadTrackTitles();
    //ValidateTrackTitles();
    //ResetTracksFile();
    
    
    //--------------------------------------------------
    //mainConsole.AddCmd(ConvertStage(), UTF8);
    //mainConsole.AddCmd(CreateTrashDirStage());
    //mainConsole.AddCmd(RemoveLeftoverStage());
    //mainConsole.AddCmd(RenameFilesStage());
    //ExecuteBatchSession();


    //--------------------------------------------------
    //GetArtworkPre();

    //mainConsole.AddCmd(GetArtworkStage(), WINDOWS1250);
    //ExecuteBatchSession();

    //GetArtworkPost();


    //--------------------------------------------------
    //AttachArtworkToAll();

    //mainConsole.AddCmd(CreateAlbumDirectoryStage());
    //mainConsole.AddCmd(MoveAudioStage());
    //mainConsole.AddCmd(MoveArtworkStage());
    //ExecuteBatchSession();

    
    // FIELDS VALUE RESET
    ///*
    if (bResetFields)
    {
        SetStatusText("Resetting");

        // Reset fields & set focus
        URL_Field->textField->SetValue("");
        URL_Artwork_Field->textField->SetValue("");
        artist_Field->textField->SetValue("");
        albumName_Field->textField->SetValue("");
        albumYear_Field->textField->SetValue("");

        artist_Field->textField->SetFocus();
    }
    //*/

    
    
    SetStatusText("Done");
    if (checkAlert->GetValue() == true) MessageBoxA(NULL, "Script has finished.", "Done", MB_OK);
    mainConsole.PrintLogAndConsoleNarrow("\n----------------------------   Program finished.   ----------------------------\n");

    if (bLog) mainConsole.CloseLog();
    std::lock_guard<std::mutex> switchLock(doneSwitchMutex);
    bDone = true;
}







std::wstring MainFrame::DownloadStage()
{
    std::wstring args = L"";
    args += L"--config-location \"" + workingDirectory + configName + L"\"";
    args += ' ';
    args += L"-o \"" + workingDirectory + L"td8_index%(playlist_index)s.mp4\"" + " \"" + URL + "\"";

    std::wstring fullCommand = L"";
    fullCommand += L"\"" + workingDirectory + downloaderExec + L"\"" + ' ' + args;



    return fullCommand;
}

std::vector<std::wstring> MainFrame::ConvertStage()
{
    std::vector<std::wstring> cmds;

    unsigned int maxNumDigits = std::to_string(trackTitles.size()).size();

    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring num = L"";

        unsigned int numDigits = std::to_string(i + 1).size();
        for (int j = 0; j < maxNumDigits - numDigits; j++) num += L'0';
        num += std::to_wstring(i + 1);

        std::wstring fname = L"td8_index" + num;
        std::wstring file = fname + L".mp4";

        std::wstring cmd = L"";
        cmd += L"\"" + workingDirectory + converterExec + L"\"";
        cmd += " -i \"" + workingDirectory + L"/" + file + L"\" -c:a mp3 -b:a 192k -ar 44100 \"" + workingDirectory + L"/" + fname + L".mp3\"";

        cmds.push_back(cmd);
    }
    return cmds;
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
    std::wstring cmd = L"cmd /a /c \"\"" + workingDirectory + downloaderExec + L"\" -e --print-to-file %(title)s \"tracks\" \"" + URL + L"\"\"";
    return cmd;
}

std::vector<std::wstring> MainFrame::RenameFilesStage(std::wstring ext)
{
    ext = L".mp4";  // stage testing: use whatever format is available
    std::vector<std::wstring> cmds;
    
    unsigned int maxNumDigits = std::to_string(trackTitles.size()).size();
    
    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring num = L"";

        unsigned int numDigits = std::to_string(i + 1).size();
        for (int j = 0; j < maxNumDigits - numDigits; j++) num += L'0';
        num += std::to_wstring(i + 1);

        std::wstring cmd = L"";
        cmd += L"cmd /u /c \"RENAME \"" + workingDirBackslashes +  L"\\";
        cmd += L"td8_index" + num + ext + L"\"";
        cmd += L" \"";
        cmd += std::to_string(i + 1) + L". " + artist + L" - " + trackTitles[i] + ext + L"\"";
        cmd += L"\"";

        cmds.push_back(cmd);
    }

    // stage testing: reverse the renaming, so it can be performed again instantly
    ///*
    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring num = L"";

        unsigned int numDigits = std::to_string(i + 1).size();
        for (int j = 0; j < maxNumDigits - numDigits; j++) num += L'0';
        num += std::to_wstring(i + 1);

        std::wstring cmd = L"";
        cmd += L"cmd /u /c \"RENAME \"" + workingDirBackslashes +  L"\\";
        cmd += std::to_string(i + 1) + L". " + artist + L" - " + trackTitles[i] + ext + L"\"";
        cmd += L" \"";
        cmd += L"td8_index" + num + ext + L"\"";
        cmd += L"\"";

        cmds.push_back(cmd);
    }
    //*/

    return cmds;
}

void MainFrame::GetArtworkPre()
{
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "GetArtworkPre()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");

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

    mainConsole.PrintLogAndConsoleNarrow("Downloading album artwork...\n");
    mainConsole.PrintLogAndConsoleNarrow("host: " + host + '\n');
    mainConsole.PrintLogAndConsoleNarrow("resource: " + resource + '\n');
    mainConsole.PrintLogAndConsoleNarrow("\n\n");



    using namespace net;

    GetResource(host.c_str(), resource.c_str(), resourceFilename.c_str());
    thumbnailURL = "";
    GetThumbnailURL(&thumbnailURL, resourceFilename.c_str());
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}

void MainFrame::GetArtworkPost()
{
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "GetArtworkPost()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");
    // Erase the resource .html file data
    FILE* resourceFile;
    fopen_s(&resourceFile, resourceFilename.c_str(), "w");
    if (resourceFile != nullptr) fclose(resourceFile);
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}

std::wstring MainFrame::GetArtworkStage()
{
    std::wstring args = L"";
    args += L"-o \"" + workingDirectory + artworkFilename + "\" \"" + thumbnailURL + "\"";

    std::wstring fullCommand = L"\""; fullCommand += workingDirectory + downloaderExec + L"\" " + args;
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
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "AttachArtworkToAll()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");
    
    SetStatusText("Attaching cover metadata");
    mainConsole.PrintLogAndConsoleNarrow("Attaching cover metadata\n");

    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring trackFilepath = workingDirectory + std::to_wstring(i + 1) + L". " + artist + L" - " + trackTitles[i] + L".mp3";
        AttachArtwork(trackFilepath, artworkFilename);
    }

    SetStatusText("Finished attaching cover metadata");
    mainConsole.PrintLogAndConsoleNarrow("Finished ataching cover metadata\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}






void MainFrame::AttachArtwork(std::wstring audioFile, std::wstring artworkFile)
{
    static unsigned int index = 1;

    artworkFile = workingDirectory + artworkFile;

    mainConsole.PrintLogAndConsole(std::to_wstring(index) + L": " + audioFile.c_str() + L"\n");
    mainConsole.PrintLogAndConsole(std::to_wstring(index) + L": " + artworkFile.c_str() + L"\n\n");

    using namespace tag;
    writeArtwork(audioFile.c_str(), artworkFile.c_str());

    mainConsole.PrintLogAndConsoleNarrow("---------------------------\n");
    index++;
}




void MainFrame::ValidateFilesystemString(std::wstring& str)
{
    // Restricted by system: \ / : * ? " < > |
    // Restricted by choice: " - "

    for (int i = 0; i < str.size(); i++)
    {
        replaceAllSubStr(str, L" \\ ", L"; ");
        replaceAllSubStr(str, L" / ", L"; ");

        if (whereSubStr(str, L"/") == str.size() - 1) replaceAllSubStr(str, L"/", L"");
        else replaceAllSubStr(str, L"/", L"; ");

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
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "ValidateTrackTitles()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");

    SetStatusText("Analysing track titles in terms of forbidden & unwanted characters...");
    for (int i = 0; i < trackTitles.size(); i++) ValidateFilesystemString(trackTitles[i]);

    PrintTracks();
    SetStatusText("Track titles validated");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}


void MainFrame::PrintTracks()
{
    mainConsole.PrintLogAndConsoleNarrow("\n\nValidated tracknames:\n");

    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring current(toWide(NumToStr(i + 1)) + L". " + artist + L" - " + trackTitles[i] + (wchar_t)'\n');
        mainConsole.PrintLogAndConsole(current);
    }
}

void MainFrame::ResetTracksFile()
{
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "ResetTracksFile()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");

    FILE* tracksFile = nullptr;
    std::string path = "tracks";
    fopen_s(&tracksFile, path.c_str(), "w");
    if (tracksFile != nullptr) fclose(tracksFile);

    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}

void MainFrame::LoadTrackTitles()
{
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "LoadTrackTitles()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");

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
            else if (currentChar == '\n')
            {
                if (currentWord.size() > 0)
                {
                    trackTitles.push_back(currentWord);
                    
                    int i = trackTitles.size() - 1;
                    std::wstring newTrack = toWide(NumToStr(i + 1)) + L". " + artist + L" - " + trackTitles[i] + (wchar_t)'\n';
                    if (i == 0) mainConsole.PrintLogAndConsoleNarrow("Unvalidated tracknames:\n");
                    mainConsole.PrintLogAndConsole(newTrack);
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

    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}







bool MainFrame::ValidateFields()
{
    // VALIDATING DIRECTORIES
    albumsDirectory = albumsDir_Field->textField->GetValue().ToStdWstring();
    workingDirectory = workingDir_Field->textField->GetValue().ToStdWstring();

    if (!validField(albumsDirectory))
    {
        wxMessageBox("Albums directory invalid.", "Error", wxOK | wxICON_ERROR);
        return false;
    }
    if (!validField(workingDirectory))
    {
        wxMessageBox("Working directory invalid.", "Error", wxOK | wxICON_ERROR);
        return false;
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


    // VALIDATING ALBUM DATA
    artist = artist_Field->textField->GetValue().ToStdWstring();
    albumName = albumName_Field->textField->GetValue().ToStdWstring();
    albumYear = albumYear_Field->textField->GetValue().ToStdWstring();

    ValidateFilesystemString(artist);
    ValidateFilesystemString(albumName);
    ValidateFilesystemString(albumYear);



    // VALIDATING URLs
    URL = URL_Field->textField->GetValue().ToStdWstring();
    artworkURL = URL_Artwork_Field->textField->GetValue().ToStdWstring();

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
        return false;
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
            return false;
        }
        else
        {
            SetStatusText("Copying Playlist URL as Artwork Playlist URL");
            URL_Artwork_Field->textField->SetValue(URL);
            artworkURL = URL;
        }
    }



    // GET BACKSLASH DIRS
    albumsDirBackslashes = GetBackslashPath(albumsDirectory);
    workingDirBackslashes = GetBackslashPath(workingDirectory);

    albumPathBackslashes = albumsDirBackslashes + L"\\" + artist + L" - " + albumName + L" (" + albumYear + L")";

    // RESET TRACK TITLES
    trackTitles.clear();
    return true;
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
        else if (currentChar == '\n')
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




