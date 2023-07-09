#include "MainFrame.h"
#include "network_utils.h"
#include "tag_utils.h"


// ID's for GUI-elements
enum
{
    ID_Frame = 1,
    ID_Save,

    ID_albumsDir_Field,
    ID_workingDir_Field,
   
    ID_artist_Field,
    ID_albumName_Field,
    ID_albumYear_Field,
    
    ID_URL_Field,
    ID_URL_Artwork_Field,

    ID_Button,
    ID_AlertOnDone,
    ID_Bitrate,

    ID_output_Field
};


void MainFrame::SizeFields(wxSize TextBoxSize, wxSize ButtonSize, wxSize OutputBoxSize)
{
    fields.clear();

    fields.push_back(Field(20, 40, TextBoxSize));
    fields.push_back(Field(20, 80, TextBoxSize));

    fields.push_back(Field(20, 140, TextBoxSize));
    fields.push_back(Field(20, 180, TextBoxSize));
    fields.push_back(Field(20, 220, TextBoxSize));

    fields.push_back(Field(20, 260, TextBoxSize));
    fields.push_back(Field(20, 300, TextBoxSize));

    fields.push_back(Field(20, 340, ButtonSize));
    fields.push_back(Field(20 + ButtonSize.x + 10, 340, ButtonSize));
    fields.push_back(Field(20 + ButtonSize.x + 10 + ButtonSize.x + 10, 342, ButtonSize));

    fields.push_back(Field(20, 405, OutputBoxSize));
}

void MainFrame::InitValues()
{
    bDone = true;
    bResetFields = true;

    initialOutput = L"";
    uMaxOutputLines = 150;


    converterExec = L"ffmpeg.exe";
    downloaderExec = L"yt-dlp.exe";

    configName = L"config.txt";
    consoleLogFilepath = L"log";
    artworkFilename = L"artwork.png";

    resourceFilename = "index.html";
    tracksFilename = "tracks";

    thumbnailURL = "";

    bitrate = 0;



    // VARIABLE SIZE & POS RANGE:
    // defaultPos.x: 0 - 350
    // ClientHeight: 640 - 955
    // OutputBoxSize.y: 215 - 530

    float screenResX = GetSystemMetrics(SM_CXSCREEN);
    float screenResY = GetSystemMetrics(SM_CYSCREEN);
    
    defaultPos.x = 350;
    defaultPos.y = 0;

    wxSize TextBoxSize;
    wxSize OutputBoxSize;
    wxSize ButtonSize;

    TextBoxSize = wxSize(800, 20);
    OutputBoxSize = wxSize(800, 530);
    ButtonSize = wxSize(100, 25);
    
    SizeFields(TextBoxSize, ButtonSize, OutputBoxSize);


    // ADJUST FIELDS WIDTH IF TOO LARGE
    for (int i = 0; i < fields.size(); i++)
    {
        if (defaultPos.x + fields[i].pos.x * 2 + fields[i].size.x > screenResX)
        {
            if (screenResX - fields[i].pos.x * 2 - fields[i].size.x >= 0)
            {
                defaultPos.x = screenResX - fields[i].pos.x * 2 - fields[i].size.x;
            }
            else
            {
                defaultPos.x = 0;
                if (fields[i].pos.x * 2 + fields[i].size.x > screenResX)
                {
                    if (screenResX - fields[i].pos.x * 2 > 250)
                    {
                        fields[i].size.x = screenResX - fields[i].pos.x * 2;
                    }
                    else fields[i].size.x = 250;
                }
            }
        }
    }

    // ADJUST LAST FIELD (OUTPUT-BOX) HEIGHT IF TOO LARGE
    if (fields.back().pos.y + fields.back().size.y + 20 > screenResY)
    {
        if (screenResY - fields.back().pos.y - 20 < 215)
        {
            fields.back().size.y = 215;
        }
        else
        {
            fields.back().size.y = screenResY - fields.back().pos.y - 20;
        }
    }


    // GET WIDEST FIELD TO BASE CLIENT WIDTH ON
    int index = 0;
    for (int i = 0; i < fields.size(); i++)
    {
        int currentWidth = fields[i].pos.x * 2 + fields[i].size.x;
        int maxWidth = fields[index].pos.x * 2 + fields[index].size.x;

        if (currentWidth > maxWidth) index = i;
    }

    ClientWidth = fields[index].pos.x + fields[index].size.x + fields[index].pos.x;
    ClientHeight = fields.back().pos.y + fields.back().size.y + 20; // BASE CLIENT HEIGHT ON THE LAST FIELD (OUTPUT-BOX)
}

void MainFrame::InitFields()
{
    // PANEL:
    // default sizes and pos because it's automatically stretched to the frame anyway
    mainPanel.Create(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2621440L, "Main Panel");

    unsigned int index = 0;

    // FIELDS:
    fAlbumsDir.Init(    "Albums directory:",    ID_albumsDir_Field,     fields[index].pos, fields[index].size, &mainPanel);   index++;
    fWorkingDir.Init(   "Working directory:",   ID_workingDir_Field,    fields[index].pos, fields[index].size, &mainPanel);   index++;
    // extra separation
    fArtist.Init(   "Artist:",      ID_artist_Field,    fields[index].pos, fields[index].size, &mainPanel);   index++;
    fAlbumName.Init("Album name:",  ID_albumName_Field, fields[index].pos, fields[index].size, &mainPanel);   index++;
    fAlbumYear.Init("Album year:",  ID_albumYear_Field, fields[index].pos, fields[index].size, &mainPanel);   index++;
    fURL.Init(          "Playlist URL:",                        ID_URL_Field,           fields[index].pos, fields[index].size, &mainPanel);   index++;
    fArtworkURL.Init(   "Playlist URL with proper artwork:",    ID_URL_Artwork_Field,   fields[index].pos, fields[index].size, &mainPanel);   index++;

    bnRunScript.Create( &mainPanel, ID_Button,      "Run",              fields[index].pos, fields[index].size, NULL, wxDefaultValidator, "Run button");     index++;
    checkAlert.Create(  &mainPanel, ID_AlertOnDone, "Alert on done",    fields[index].pos, fields[index].size, NULL, wxDefaultValidator, "Alert checkbox"); index++;
    //fBitrate.Create(&mainPanel, ID_Bitrate, "Bitrate", fields[index].pos, fields[index].size, 0, NULL, NULL, wxDefaultValidator, "Bitrate dropdown");  index++;
    fBitrate.Init( "Bitrate:", L"----", ID_Bitrate, fields[index].pos, fields[index].size, &mainPanel);    index++;
    // extra separation
    fOutput.Init("Output:", ID_output_Field, fields[index].pos, fields[index].size, &mainPanel, wxTE_MULTILINE | wxTE_READONLY);    index++;
}

void MainFrame::InitConsole()
{
    // pass the output mutex address
    fOutput.SetMutex(mainConsole.GetPrintMutex());

    mainConsole.InitConsole(consoleLogFilepath, fOutput.GetBuf(), false);
    tag::SetConsole(&mainConsole);
    net::SetConsole(&mainConsole);

    if (bLog) mainConsole.OpenLog();
}

void MainFrame::InitThemes()
{
    unsigned long uForeground = 0xC0C0C0;
    unsigned long uBackground = 0xFF0050;
    fOutput.SetForeground(wxColour(uForeground));
    fOutput.SetBackground(wxColour(uBackground));

    // TO DO:
    
    // -Add a blinking cursor at insertion point (e.g. char: _)
    // -Custom selection color
    // -Directory selection boxes instead of fields for both dir fields
    // -Format & bitrate drop-down lists
    // 
}

void MainFrame::InitFonts()
{
    // Considered fonts:
    // Notepad++:   Courier New, 10 | 0xFFFFFF, 0x1E1E1E
    // Console:     Terminal, 8x12  | 0xC0C0C0, 0x000000

    

    wxFont outputFont;
    //outputFont = wxFont(wxFontInfo(wxSize(8, 16)).FaceName("Courier New").Bold());
    //outputFont = wxFont(wxFontInfo(wxSize(8, 12)).FaceName("Consolas").Bold());
    //outputFont = wxFont(wxFontInfo(wxSize(8, 12)).FaceName("The One True Font (System 8x12)"));

    //outputFont = wxFont(wxFontInfo(wxSize(8, 12)).FaceName("Terminal"));
    //fOutput.SetEncoding(CP852);

    // FIXEDSYS VARIANTS:
    // no ligatures
    outputFont = wxFont(wxFontInfo(12).FaceName("Fixedsys"));

    // few ligatures
    //outputFont.AddPrivateFont("FSEX300.ttf");
    //outputFont = wxFont(wxFontInfo(12).FaceName("Fixedsys Excelsior 3.01"));

    // most ligatures
    //outputFont.AddPrivateFont("FSEX302.ttf");
    //outputFont.AddPrivateFont("FSEX302-alt.ttf");
    //outputFont = wxFont(wxFontInfo(12).FaceName("Fixedsys Excelsior"));

    
    fOutput.SetFont(outputFont);
    
    

    std::wstring outputFaceName = fOutput.GetFontFaceName();

    //mainConsole.PrintLogAndConsole(testUnicode(outputFaceName));
    //mainConsole.PrintLogAndConsole(testNoLigature());
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

    //fOutput.SetEditable(false);

    ///*
    fBitrate.AppendItem("128 kbit/s");
    fBitrate.AppendItem("144 kbit/s");
    fBitrate.AppendItem("160 kbit/s");
    fBitrate.AppendItem("192 kbit/s");
    fBitrate.AppendItem("224 kbit/s");
    fBitrate.AppendItem("256 kbit/s");
    fBitrate.AppendItem("320 kbit/s");
    //*/

    fBitrate.SetSelected(3);
    initialOutput += L"selected: " + toWide(fBitrate.GetSelected()) + '\n';


    fWorkingDir.SetText(L"workfolder/");
    fArtist.SetFocus();
}

void MainFrame::InitClientSize()
{
    SetClientSize(ClientWidth, ClientHeight);
}

void MainFrame::InitTestValues()
{
    bResetFields = false;
    // SAMPLE TEST VALUES FOR CONVENIENCE:

    // SHORT PLAYLIST
    /*
    fArtist.SetText(L"Big Black");
    fAlbumName.SetText(L"Lungs");
    fAlbumYear.SetText(L"1982");
    fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_lSCRmY_Qw8RCNnMKHcp05O1K8fAIyqLjs");
    */

    // RARE UNICODE CHAR THAT SHOWS DIFFERENCE BETWEEN WINDOWS-1250 AND WINDOWS-1252
    /*
    fArtist.SetText(L"The Jesus Lizard");
    fAlbumName.SetText(L"Down");
    fAlbumYear.SetText(L"1994");
    fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_kULt5j2pKzT5PtLz1RGW7EO-IWDwqVtHw");
    */

    // RARE UNICODE CODE POINTS
    /*
    fArtist.SetText(L"Death in June");
    fAlbumName.SetText(L"Discriminate: A Compilation of Personal Choice 1981-97");
    fAlbumYear.SetText(L"1997");
    fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_ll7VmeyNV0J4d4HroMPrLrRfBcjiLIVLo");
    */

    // TYPICAL UNICODE TITLES
    /*
    fArtist.SetText(L"O.S.T.R.");
    fAlbumName.SetText(L"Tylko Dla Doros³ych");
    fAlbumYear.SetText(L"2010");
    fURL.SetText(L"https://www.youtube.com/playlist?list=PLIKxxmyVA3HZ5vCNl3b0gQXDhuMWLz-mG");
    fArtworkURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_l6DSlExq2EbVR7ILChbL9ZHn-1SbyKRO8");
    */

    // TYPICAL UNICODE TITLES
    ///*
    fArtist.SetText(L"Goat");
    fAlbumName.SetText(L"World Music");
    fAlbumYear.SetText(L"2012");
    fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_nMsUDBQ3_Xsjdz62NkJ_g1HnEirKtRkZg");
    //fArtworkURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_nMsUDBQ3_Xsjdz62NkJ_g1HnEirKtRkZg");
    //*/
}

MainFrame::MainFrame() : wxFrame(NULL, ID_Frame, "album-dl")
{
    InitValues();
    InitFields();

    InitConsole();

    InitThemes();
    InitFonts();

    InitBindings();
    InitControls();

    InitClientSize();

    InitTestValues();

    SetPosition(wxPoint(defaultPos.x, defaultPos.y));   // SET WINDOW POS TO DEFAULT POS
    OpenSettings();                                     // LOAD SETTINGS (MAY REPOS WINDOW)
    Show(true);                                         // SHOW WINDOW

    mainConsole.PrintLogAndConsole(initialOutput);
    initialOutput.clear();
}

MainFrame::~MainFrame()
{
    // ExitProcess() exits the calling process and all it's threads
    ExitProcess(ERR_SUCCESS);
}



void MainFrame::OnClose(wxCloseEvent& event)
{
    //SaveSettings();
    if (bLog) mainConsole.CloseLog();

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


    DisableFields();
    bDone = false;

    if (workingThread.joinable()) workingThread.join();
    workingThread = std::move(std::thread(&MainFrame::GetAlbum, this));
}





void MainFrame::StartOutputUpdate()
{
    if (outputThread.joinable()) outputThread.join();
    outputThread = std::move(std::thread(&MainFrame::UpdateOutput, this));
}

void MainFrame::UpdateOutput()
{
    while (true)
    {
        fOutput.RemoveExceeding(uMaxOutputLines);
        fOutput.FlushBuf();
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
    //if (bLog) mainConsole.OpenLog();
    if (!fOutput.IsEmpty()) mainConsole.PrintLogAndConsoleNarrow("\n\n");

    SetStatusText("Running the program...");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Program start.      ----------------------------\n");


    //--------------------------------------------------
    //mainConsole.AddCmd(DownloadStage(), WINDOWS1250);
    //ExecuteBatchSession();

    
    //--------------------------------------------------
    //ResetTracksFile();
    //mainConsole.AddCmd(GetTitlesStage(), WINDOWS1250);
    //ExecuteBatchSession();

    //LoadTrackTitles();
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
    if (bResetFields)
    {
        SetStatusText("Resetting");

        // Reset fields & set focus
        fURL.SetText(L"");
        fArtworkURL.SetText(L"");
        fArtist.SetText(L"");
        fAlbumName.SetText(L"");
        fAlbumYear.SetText(L"");

        fArtist.SetFocus();
    }

    
    
    SetStatusText("Done");
    if (checkAlert.GetValue() == true) MessageDialog("Script has finished.", "Done");
    mainConsole.PrintLogAndConsoleNarrow("\n----------------------------   Program finished.   ----------------------------\n");

    //if (bLog) mainConsole.CloseLog();

    bDone = true;
    EnableFields();
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
    ClearFileData(resourceFilename);

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

    ClearFileData(tracksFilename);

    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}

void MainFrame::LoadTrackTitles()
{
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "LoadTrackTitles()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");



    std::string data = "";
    if (GetFileData(tracksFilename, &data))
    {
        SetStatusText("Failed to load track titles from file");
    }
    else
    {
        std::wstring wdata = DecodeFromUTF8(data);
        std::wstring currentWord = L"";

        for (int i = 0; i < wdata.size(); i++)
        {
            wchar_t currentChar = wdata[i];

            if (iswprint(currentChar))
            {
                currentWord += currentChar;
            }
            else if (currentChar == '\n')
            {
                if (currentWord.size() > 0)
                {
                    trackTitles.push_back(currentWord);
                }
                currentWord = L"";
            }
        }
        SetStatusText("Track titles loaded");

        
        mainConsole.PrintLogAndConsoleNarrow("Unvalidated tracknames:\n");
        for (int i = 0; i < trackTitles.size(); i++)
        {
            std::wstring newTrack = toWide(NumToStr(i + 1)) + L". " + artist + L" - " + trackTitles[i] + L'\n';
            mainConsole.PrintLogAndConsole(newTrack);
        }
    }

    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}







void MainFrame::DisableFields()
{
    fAlbumsDir.Disable();
    fWorkingDir.Disable();

    fArtist.Disable();
    fAlbumName.Disable();
    fAlbumYear.Disable();

    fURL.Disable();
    fArtworkURL.Disable();

    fBitrate.Disable();
}

void MainFrame::EnableFields()
{
    fAlbumsDir.Enable();
    fWorkingDir.Enable();

    fArtist.Enable();
    fAlbumName.Enable();
    fAlbumYear.Enable();

    fURL.Enable();
    fArtworkURL.Enable();

    fBitrate.Enable();
}



bool MainFrame::ValidateFields()
{
    // VALIDATING DIRECTORIES
    albumsDirectory = fAlbumsDir.GetText();
    workingDirectory = fWorkingDir.GetText();

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
        fAlbumsDir.SetText(albumsDirectory);
    }

    if (workingDirectory[workingDirectory.size() - 1] != '/')
    {
        workingDirectory += '/';
        fWorkingDir.SetText(workingDirectory);
    }


    // VALIDATING ALBUM DATA
    artist = fArtist.GetText();
    albumName = fAlbumName.GetText();
    albumYear = fAlbumYear.GetText();

    ValidateFilesystemString(artist);
    ValidateFilesystemString(albumName);
    ValidateFilesystemString(albumYear);



    // VALIDATING URLs
    URL = fURL.GetText();
    artworkURL = fArtworkURL.GetText();

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
            fArtworkURL.SetText(URL);
            artworkURL = URL;
        }
    }




    // GET BITRATE
    for (int i = 0; i < fBitrate.GetItems().size(); i++)
    {
        MessageDialog(fBitrate.GetItem(i) + ": " + NumToStr(GetNumFromFirstWord(fBitrate.GetItem(i))));
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
                    if (currentId == albumsDir) fAlbumsDir.SetText(currentWord);
                    if (currentId == workingDir) fWorkingDir.SetText(currentWord);

                    if (currentId == windowX && isStrNum(currentWord)) SetPosition(wxPoint(std::stoi(currentWord), GetPosition().y));
                    if (currentId == windowY && isStrNum(currentWord)) SetPosition(wxPoint(GetPosition().x, std::stoi(currentWord)));

                    if (currentId == alertDone)
                    {
                        if (currentWord == "0") checkAlert.SetValue(false);
                        if (currentWord == "1") checkAlert.SetValue(true);
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
    decoded += fAlbumsDir.GetText() + L"\n";
    decoded += fWorkingDir.GetText() + L"\n";
    decoded += toWide(NumToStr(GetPosition().x)) + L"\n";
    decoded += toWide(NumToStr(GetPosition().y)) + L"\n";

    if (checkAlert.GetValue()) decoded += L'1';
    else decoded += L'0';

    std::string encoded = EncodeToUTF8(decoded);


    if (WriteDataToFile(encoded, path))
    {
        SetStatusText("Cannot save settings");
        return;
    }
    SetStatusText("Settings have been saved");
}




