#include "MainFrame.h"
#include "network_utils.h"
#include "tag_utils.h"


// ID's for GUI-elements
enum
{
    ID_Frame = 1,
    ID_Panel,
    ID_Save,

    ID_albumsDir_Field,
    ID_workingDir_Field,
    ID_converterDir_Field,
   
    ID_artist_Field,
    ID_albumName_Field,
    ID_albumYear_Field,
    
    ID_URL_Field,
    ID_URL_Artwork_Field,

    ID_ButtonDownload,
    ID_AlertOnDone,
    ID_Bitrate,
    ID_ButtonUpdate,

    ID_output_Field,
    ID_output_Label,
    ID_extra_Field
};



void MainFrame::FindMaxDistanceFields()
{
    for (int i = 0; i < fields.size(); i++)
    {
        if (fields[i].pos.x + fields[i].size.x >= fields[horizontalMax].pos.x + fields[horizontalMax].size.x) horizontalMax = i;
        if (fields[i].pos.y + fields[i].size.y >= fields[verticalMax].pos.y + fields[verticalMax].size.y) verticalMax = i;
    }

    horizontalMaxDistance = fields[horizontalMax].pos.x + fields[horizontalMax].size.x;
    verticalMaxDistance = fields[verticalMax].pos.y + fields[verticalMax].size.y;
}

void MainFrame::SetFullSize()
{
    FindMaxDistanceFields();
    int ClientWidth = horizontalMaxDistance + clientMargin.right;
    int ClientHeight = verticalMaxDistance + clientMargin.bottom;

    SetClientSize(ClientWidth, ClientHeight);
}





// -- INIT METHODS - CALLED BY CONSTRUCTOR
void MainFrame::InitValues()
{
    bDone = true;
    bResetFields = true;

    uMaxOutputLines = 150;


    converterExec = L"ffmpeg.exe";
    downloaderExec = L"yt-dlp.exe";

    configName = L"config.txt";
    consoleLogFilepath = L"log";
    artworkFilename = L"artwork.png";
    artworkBrokenFilename = L"artwork_broken.png";

    playlistPageFilename = L"playlist.html";
    playlistArtPageFilename = L"playlist_art.html";

    trashFoldername = L"Trash";

    tracksFilename = L"tracks";
    settingsFilename = L"settings";
    defaultSettings = L"";

    defaultSettings += L"\n";
    defaultSettings += L"workfolder/\n";
    defaultSettings += L"\n";

    defaultSettings += L"-\n";
    defaultSettings += L"-\n";
    defaultSettings += L"0\n";
    defaultSettings += L"192 kbit/s\n";

    thumbnailURL = L"";

    bitrate = 0;


    defaultPos.x = 0;
    defaultPos.y = 0;

    horizontalMax = 0;
    verticalMax = 0;
}

void MainFrame::InitMenuAndStatusBar()
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
}



void MainFrame::InitFieldsDimensions()
{
    //  _________________________________
    // | fields      |terminal           |
    // | ____________|__________________ |
    // ||directories |                  ||
    // ||____________|                  ||
    // ||data        |                  ||
    // ||____________|                  ||
    // ||buttons     |                  ||
    // ||____________|__________________||
    // |_____________|___________________|

    wxSize TextBoxSize;
    wxSize OutputBoxSize;
    wxSize ButtonSize;

    TextBoxSize = wxSize(550, 20);
    ButtonSize = wxSize(100, 25);

    clientMargin = { 20, 20, 20, 20 };

    fields.clear();

    fields.push_back(Field(clientMargin.left, clientMargin.top, TextBoxSize));
    fields.push_back(Field(clientMargin.left, 60, TextBoxSize));
    fields.push_back(Field(clientMargin.left, 100, TextBoxSize));

    fields.push_back(Field(clientMargin.left, 160, TextBoxSize));
    fields.push_back(Field(clientMargin.left, 200, TextBoxSize));
    fields.push_back(Field(clientMargin.left, 240, TextBoxSize));

    fields.push_back(Field(clientMargin.left, 280, TextBoxSize));
    fields.push_back(Field(clientMargin.left, 320, TextBoxSize));

    fields.push_back(Field(clientMargin.left, 360, ButtonSize));
    fields.push_back(Field(clientMargin.left + ButtonSize.x + 10, 360, ButtonSize));
    fields.push_back(Field(clientMargin.left + ButtonSize.x + 10 + ButtonSize.x + 10, 362, ButtonSize));
    fields.push_back(Field(clientMargin.left + ButtonSize.x + 10 + ButtonSize.x + 10 + ButtonSize.x + 10, 360, ButtonSize));


    FindMaxDistanceFields();
    OutputBoxSize = wxSize(700, verticalMaxDistance - clientMargin.top);
    fields.push_back(Field(horizontalMaxDistance + 10, clientMargin.top, OutputBoxSize));
}

void MainFrame::InitFields()
{
    // PANEL:
    // default sizes and pos because it's automatically stretched to the frame anyway
    mainPanel.Create(this, ID_Panel, wxDefaultPosition, wxDefaultSize, 2621440L, "Main Panel");
    wxWindow* parent = &mainPanel;

    unsigned int index = 0;

    // FIELDS:
    fAlbumsDir.Init(    "Albums directory:",    ID_albumsDir_Field,     fields[index].pos, fields[index].size, parent);   index++;
    fWorkingDir.Init(   "Working directory:",   ID_workingDir_Field,    fields[index].pos, fields[index].size, parent);   index++;
    fConverterDir.Init( "ffmpeg.exe directory:", ID_converterDir_Field, fields[index].pos, fields[index].size, parent);   index++;
    // extra separation
    fArtist.Init(   "Artist:",      ID_artist_Field,    fields[index].pos, fields[index].size, parent);   index++;
    fAlbumName.Init("Album name:",  ID_albumName_Field, fields[index].pos, fields[index].size, parent);   index++;
    fAlbumYear.Init("Album year:",  ID_albumYear_Field, fields[index].pos, fields[index].size, parent);   index++;
    fURL.Init(          "Playlist URL:",                        ID_URL_Field,           fields[index].pos, fields[index].size, parent);   index++;
    fArtworkURL.Init(   "Playlist URL with proper artwork:",    ID_URL_Artwork_Field,   fields[index].pos, fields[index].size, parent);   index++;

    bnRunScript.Create(parent, ID_ButtonDownload, "Run",           fields[index].pos, fields[index].size, NULL, wxDefaultValidator, "Run button");     index++;
    checkAlert.Create (parent, ID_AlertOnDone, "Alert on done",    fields[index].pos, fields[index].size, NULL, wxDefaultValidator, "Alert checkbox"); index++;
    fBitrate.Init( "Bitrate:", L"----", ID_Bitrate,                     fields[index].pos, fields[index].size, parent);    index++;
    bnUpdateDownloader.Create(parent, ID_ButtonUpdate, "Update YT-DLP", fields[index].pos, fields[index].size, NULL, wxDefaultValidator, "Update button"); index++;

    fOutput.Init("Output:", ID_output_Field, ID_output_Label, fields[index].pos, fields[index].size, parent, wxTE_MULTILINE | wxTE_READONLY);    index++;

    fExtra.Init(
        "Output:", ID_extra_Field, wxPoint(clientMargin.left, clientMargin.top), wxSize(fAlbumsDir.GetSize().x, 320), parent, wxTE_MULTILINE | wxTE_READONLY);
    fAlbumsDir.Hide();
    fWorkingDir.Hide();
    fConverterDir.Hide();

    fArtist.Hide();
    fAlbumName.Hide();
    fAlbumYear.Hide();
    fURL.Hide();
    fArtworkURL.Hide();
}

void MainFrame::InitFieldsDimensionRanges()
{
    // FIELDS MIN SIZES
    //fAlbumsDir.SetMinSize(fAlbumsDir.GetSize());
    //fWorkingDir.SetMinSize(fAlbumsDir.GetSize());
    //fConverterDir.SetMinSize(fAlbumsDir.GetSize());

    //fArtist.SetMinSize(fAlbumsDir.GetSize());
    //fAlbumName.SetMinSize(fAlbumsDir.GetSize());
    //fAlbumYear.SetMinSize(fAlbumsDir.GetSize());
    //fURL.SetMinSize(fAlbumsDir.GetSize());
    //fArtworkURL.SetMinSize(fAlbumsDir.GetSize());
    
    fExtra.SetMinSize(fExtra.GetSize());
    fOutput.SetMinSize(fOutput.GetSize());


    // FIELDS MAX SIZES
    //fAlbumsDir.SetMaxSize(800, fAlbumsDir.GetSize().y);
    //fWorkingDir.SetMaxSize(800, fAlbumsDir.GetSize().y);
    //fConverterDir.SetMaxSize(800, fAlbumsDir.GetSize().y);

    //fArtist.SetMaxSize(800, fAlbumsDir.GetSize().y);
    //fAlbumName.SetMaxSize(800, fAlbumsDir.GetSize().y);
    //fAlbumYear.SetMaxSize(800, fAlbumsDir.GetSize().y);
    //fURL.SetMaxSize(800, fAlbumsDir.GetSize().y);
    //fArtworkURL.SetMaxSize(800, fAlbumsDir.GetSize().y);

    fExtra.SetMaxSize(800, fExtra.GetSize().y);
    fOutput.SetMaxSize(-1, -1);
}



void MainFrame::InitBindings()
{
    Bind(wxEVT_BUTTON, &MainFrame::OnButtonGet, this, ID_ButtonDownload);
    Bind(wxEVT_BUTTON, &MainFrame::OnButtonUpdate, this, ID_ButtonUpdate);

    mainPanel.Bind(wxEVT_SIZE, &MainFrame::OnPanelResize, this, ID_Panel);

    Bind(wxEVT_MENU, &MainFrame::OnSave, this, ID_Save);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
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
    
    // -Add blinking cursor at insertion point (e.g. char: _)
    // -Custom foreground, background and selection colors
    // -Directory boxes instead of text boxes for dir fields
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
    fExtra.SetFont(wxFont(wxFontInfo(9).FaceName("Courier New")));
    
    

    std::wstring outputFaceName = fOutput.GetFontFaceName();
    if (outputFaceName != "Fixedsys")
    {
        outputFont = wxFont(wxFontInfo(wxSize(8, 16)).FaceName("Courier New").Bold());
        fOutput.SetFont(outputFont);
    }
    

    //mainConsole.PrintLogAndConsole(testUnicode(outputFaceName));
    //mainConsole.PrintLogAndConsole(testNoLigature());
}



void MainFrame::InitTestValues()
{
    bResetFields = false;
    // SAMPLE TEST VALUES FOR CONVENIENCE:

    // SHORT PLAYLISTS
    fArtist.SetText(L"Big Black");
    fAlbumName.SetText(L"Racer-X");
    fAlbumYear.SetText(L"1985");
    fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_nrAFOfF6ITDAEJ-BuHYWpHYOwsKNTZ994");

    //fArtist.SetText(L"Big Black");
    //fAlbumName.SetText(L"Lungs");
    //fAlbumYear.SetText(L"1982");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_lSCRmY_Qw8RCNnMKHcp05O1K8fAIyqLjs");




    // RARE UNICODE CHAR THAT SHOWS DIFFERENCE BETWEEN WINDOWS-1250 AND WINDOWS-1252
    //fArtist.SetText(L"The Jesus Lizard");
    //fAlbumName.SetText(L"Down");
    //fAlbumYear.SetText(L"1994");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_kULt5j2pKzT5PtLz1RGW7EO-IWDwqVtHw");

    // RARE UNICODE CODE POINTS
    //fArtist.SetText(L"Death in June");
    //fAlbumName.SetText(L"Discriminate: A Compilation of Personal Choice 1981-97");
    //fAlbumYear.SetText(L"1997");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_ll7VmeyNV0J4d4HroMPrLrRfBcjiLIVLo");

    // TYPICAL UNICODE TITLES
    //fArtist.SetText(L"O.S.T.R.");
    //fAlbumName.SetText(L"Tylko Dla Doros³ych");
    //fAlbumYear.SetText(L"2010");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=PLIKxxmyVA3HZ5vCNl3b0gQXDhuMWLz-mG");
    //fArtworkURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_l6DSlExq2EbVR7ILChbL9ZHn-1SbyKRO8");

    // TYPICAL UNICODE TITLES
    //fArtist.SetText(L"Goat");
    //fAlbumName.SetText(L"World Music");
    //fAlbumYear.SetText(L"2012");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_nMsUDBQ3_Xsjdz62NkJ_g1HnEirKtRkZg");
    ////fArtworkURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_nMsUDBQ3_Xsjdz62NkJ_g1HnEirKtRkZg");
}

void MainFrame::InitBitrates()
{
    fBitrate.AppendItem("128 kbit/s");
    fBitrate.AppendItem("144 kbit/s");
    fBitrate.AppendItem("160 kbit/s");
    fBitrate.AppendItem("192 kbit/s");
    fBitrate.AppendItem("224 kbit/s");
    fBitrate.AppendItem("256 kbit/s");
    fBitrate.AppendItem("320 kbit/s");
}



void MainFrame::InitWindowSize()
{
    SetFullSize();
    SetMinSize(GetSize());

    SetClientSize(1600, 408);
}

void MainFrame::InitPosition()
{
    SetPosition(wxPoint(defaultPos.x, defaultPos.y));   // SET WINDOW POS TO DEFAULT POS
}

void MainFrame::InitSettings()
{
    OpenSettings();                                     // LOAD SETTINGS (MAY REPOS WINDOW)
    Show(true);                                         // SHOW WINDOW
}



void MainFrame::InitDimensionsInfo()
{
    dimensionsInfo = L"";

    float screenX, screenY;
    screenX = GetSystemMetrics(SM_CXSCREEN);
    screenY = GetSystemMetrics(SM_CYSCREEN);

    dimensionsInfo += L"screen res: " + NumToWstr((int)screenX) + L"x" + NumToWstr((int)screenY) + L"\n";

    char whitespace = ' ';
    for (int i = 0; i < fields.size(); i++)
    {
        dimensionsInfo += L"fields[" + NumToWstr(i, 10, 2, whitespace) + L"]: (";
        dimensionsInfo += NumToWstr(fields[i].pos.x, 10, 3, ' ') + L", " + NumToWstr(fields[i].pos.y, 10, 3, ' ') + L"), (";
        dimensionsInfo += NumToWstr(fields[i].size.x, 10, 3, ' ') + L", " + NumToWstr(fields[i].size.y, 10, 3, ' ') + L")\n";
    }

    dimensionsInfo += L"\n";
    dimensionsInfo += L"GetClientSize():     " + NumToWstr(GetClientSize().x) + L"x" + NumToWstr(GetClientSize().y) + L"\n";
    dimensionsInfo += L"GetMinClientSize():  " + NumToWstr(GetMinClientSize().x) + L"x" + NumToWstr(GetMinClientSize().y) + L"\n";
    dimensionsInfo += L"GetSize():           " + NumToWstr(GetSize().x) + L"x" + NumToWstr(GetSize().y) + L"\n";
    dimensionsInfo += L"GetMinSize():        " + NumToWstr(GetMinSize().x) + L"x" + NumToWstr(GetMinSize().y) + L"\n";
    dimensionsInfo += L"\n";
}

void MainFrame::InitVerifyExecutables()
{
    albumsDirectory = fAlbumsDir.GetText();
    workingDirectory = fWorkingDir.GetText();
    converterDirectory = fConverterDir.GetText();

    if (!VerifyFile(workingDirectory, downloaderExec))
    {
        execsInfo +=    L"Failed to locate '" + downloaderExec + L"' in:\n";

        if (workingDirectory.empty()) execsInfo += L"<no directory provided>";
        else execsInfo += workingDirectory;

        execsInfo += L"\n";
        execsInfo += L"--------------------------------------------------------------------------\n\n";
    }

    if (!VerifyFile(converterDirectory, converterExec))
    {
        execsInfo +=    L"Failed to locate '" + converterExec + L"' in:\n";

        if (converterDirectory.empty()) execsInfo += L"<no directory provided>";
        else execsInfo += converterDirectory;

        execsInfo += L"\n";


        execsInfo +=    L"\nAlbum-dl requires FFmpeg to work, if you don't have FFmpeg installed, visit:\n"
            L"https://ffmpeg.org/download.html\n\n";

        execsInfo += L"Remember to get 'ffmpeg.exe' and provide it's directory in the field above.\n";
        execsInfo += L"If album-dl still can't locate 'ffmpeg.exe', try running as administrator.\n";
        execsInfo += L"--------------------------------------------------------------------------\n\n";
    }
}

void MainFrame::InitTerminalOutput()
{
    initialOutput = L"";
    initialOutput += L"--------------------------------------------------------------------------\n";
    initialOutput += L"|            Welcome to album-dl's output terminal, stand-by.            |\n";
    initialOutput += L"--------------------------------------------------------------------------\n\n";

    if (!dimensionsInfo.empty())    initialOutput += dimensionsInfo + L"\n\n";
    if (!execsInfo.empty())         initialOutput += execsInfo + L"\n\n";

    mainConsole.PrintLogAndConsole(initialOutput);
    initialOutput.clear();
}



void MainFrame::InitFocus()
{
    bnRunScript.SetFocus();
    //fArtist.SetFocus();
}
// --



MainFrame::MainFrame() : wxFrame(NULL, ID_Frame, "album-dl")
{
    // -- INIT METHODS
    InitValues();
    InitMenuAndStatusBar();

    InitFieldsDimensions();         // SET INITIAL FIELDS DIMENSIONS
    InitFields();                   // SET LABELS AND CONSTRUCT FIELDS
    InitFieldsDimensionRanges();

    InitBindings();

    InitConsole();

    InitThemes();
    InitFonts();

    InitTestValues();
    InitBitrates();

    InitWindowSize();
    InitPosition();
    InitSettings();             // LAST METHOD TO ADJUST WINDOW PARAMETERS, SHOWS THE WINDOW
    
    InitDimensionsInfo();
    InitVerifyExecutables();
    InitTerminalOutput();

    InitFocus();
    // --
}

MainFrame::~MainFrame()
{
    // ExitProcess() exits the calling process and all it's threads
    ExitProcess(ERR_SUCCESS);
}



// -- EVENT METHODS
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
    MessageDialog(GetAbout(), "About");
}

void MainFrame::OnSave(wxCommandEvent& event)
{
    // File / Save settings
    SaveSettings();
}

void MainFrame::OnButtonGet(wxCommandEvent& event)
{
    if (!bDone) return;
    if (!ValidateFields()) return;


    DisableFields();
    bDone = false;

    if (workingThread.joinable()) workingThread.join();
    workingThread = std::move(std::thread(&MainFrame::GetAlbum, this));
}

void MainFrame::OnButtonUpdate(wxCommandEvent& event)
{
    if (!bDone) return;
    if (!ValidateFieldsUpdate()) return;


    DisableFields();
    bDone = false;

    if (workingThread.joinable()) workingThread.join();
    workingThread = std::move(std::thread(&MainFrame::UpdateDownloader, this));
}

void MainFrame::OnPanelResize(wxSizeEvent& event)
{
    //event.Skip();
    int newClientWidth = event.GetSize().x;
    int newClientHeight = event.GetSize().y;

    static int oldClientWidth = 0;
    static int oldClientHeight = 0;
    if (oldClientWidth == 0 || oldClientHeight == 0)
    {
        oldClientWidth = mainPanel.GetClientSize().x;
        oldClientHeight = mainPanel.GetClientSize().y;

        fExtra.AddText(L"PanelResize-oldClient: init\n\n");
        return;
    }





    // direction doesn't matter
    // fExtra.GetSize().x = fExtra.GetMinSize().x + <increase>
    // fOutput.GetSize().x = fOutput.GetMinSize().x + <increase>
    // | clientMargin.left | fExtra.GetMinSize().x + <increase> | 10 | fOutput.GetMinSize().x + <increase> | clientMargin.right |
    int totalIncrease = newClientWidth;
    totalIncrease -= clientMargin.right;
    totalIncrease -= fOutput.GetMinSize().x;
    totalIncrease -= 10;
    totalIncrease -= fExtra.GetMinSize().x;
    totalIncrease -= clientMargin.left;

    int fExtraIncrease = totalIncrease;
    int fOutputIncrease = 0;
    if (fExtra.GetMinSize().x + fExtraIncrease > fExtra.GetMaxSize().x)
    {
        fExtraIncrease = fExtra.GetMaxSize().x - fExtra.GetMinSize().x;
        fOutputIncrease = totalIncrease - fExtraIncrease;
    }


    std::wstring textFieldPosBeforeStr = std::to_wstring(fOutput.textField.GetPosition().x) + L"x" + std::to_wstring(fOutput.textField.GetPosition().y);
    fExtra.SetSize(fExtra.GetMinSize().x + fExtraIncrease, fExtra.GetSize().y);
    
    fOutput.SetPosition(fExtra.GetPosition().x + fExtra.GetSize().x + 10, fOutput.GetPosition().y);
    fOutput.SetSize(fOutput.GetSize().x, fOutput.GetSize().y + newClientHeight - oldClientHeight);
    fOutput.SetSize(fOutput.GetMinSize().x + fOutputIncrease, fOutput.GetSize().y);

    //fOutput.textField.SetPosition(wxPoint(3, 15));


    std::wstring oldClientSizeStr = std::to_wstring(oldClientWidth) + L"x" + std::to_wstring(oldClientHeight);
    std::wstring newClientSizeStr = std::to_wstring(newClientWidth) + L"x" + std::to_wstring(newClientHeight);
    std::wstring textFieldPosAfterStr = std::to_wstring(fOutput.textField.GetPosition().x) + L"x" + std::to_wstring(fOutput.textField.GetPosition().y);

    fExtra.AddText(L"PanelResize-oldClientSize: " + oldClientSizeStr + L"\n");
    fExtra.AddText(L"PanelResize-newClientSize: " + newClientSizeStr + L"\n");
    fExtra.AddText(L"PanelResize-textFieldPosBeforeStr: " + textFieldPosBeforeStr + L"\n");
    fExtra.AddText(L"PanelResize-textFieldPosAfterStr: " + textFieldPosAfterStr + L"\n");
    fExtra.AddText(L"PanelResize-totalIncrease: " + std::to_wstring(totalIncrease) + L"\n");
    fExtra.AddText(L"PanelResize-fExtraIncrease: " + std::to_wstring(fExtraIncrease) + L"\n");
    fExtra.AddText(L"PanelResize-fOutputIncrease: " + std::to_wstring(fOutputIncrease) + L"\n");
    fExtra.AddText(L"\n");

    oldClientWidth = mainPanel.GetClientSize().x;
    oldClientHeight = mainPanel.GetClientSize().y;
}
// --







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
    //--------------------------------------------------
    if (!fOutput.IsEmpty()) mainConsole.PrintLogAndConsoleNarrow("\n\n");

    SetStatusText("Running the program...");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Program start.      ----------------------------\n");
    //--------------------------------------------------


    //--------------------------------------------------
    //mainConsole.AddCmd(DownloadStage(), WINDOWS1250);
    //ExecuteBatchSession();

    //
    ////--------------------------------------------------
    //GetArtworkStage();



    //--------------------------------------------------
    ResetTracksFile();
    GetTrackTitles();

    //LoadTrackTitles();
    //ValidateTrackTitles();
    //ResetTracksFile();
    //
    //
    ////--------------------------------------------------
    //mainConsole.AddCmd(ConvertStage(), UTF8);
    //mainConsole.AddCmd(CreateTrashDirStage());
    //mainConsole.AddCmd(RemoveLeftoverStage());
    //mainConsole.AddCmd(RenameFilesStage());
    //ExecuteBatchSession();


    ////--------------------------------------------------
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
    mainConsole.PrintLogAndConsoleNarrow("\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Program finished.   ----------------------------\n");


    bDone = true;
    EnableFields();
}

void MainFrame::UpdateDownloader()
{
    //--------------------------------------------------
    if (!fOutput.IsEmpty()) mainConsole.PrintLogAndConsoleNarrow("\n\n");

    SetStatusText("Running the program...");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Program start.      ----------------------------\n");
    //--------------------------------------------------

    std::wstring args = L"--update";
    std::wstring updateCmd = L"";
    updateCmd += L"\"" + workingDirectory + downloaderExec + L"\"" + L' ' + args;

    mainConsole.AddCmd(updateCmd, WINDOWS1250);
    ExecuteBatchSession();


    SetStatusText("Done");
    if (checkAlert.GetValue() == true) MessageDialog("Script has finished.", "Done");
    mainConsole.PrintLogAndConsoleNarrow("\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Program finished.   ----------------------------\n");


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
        cmd += L"\"" + converterDirectory + converterExec + L"\"";
        cmd += " -i \"" + workingDirectory + file + L"\" ";
        cmd += L"-c:a mp3 ";
        cmd += L"-b:a " + NumToStr(bitrate) + L"k ";
        cmd += L"-ar 44100 ";
        cmd += L"\"" + workingDirectory + fname + L".mp3\"";

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
    std::wstring artworkBrokenPath = workingDirectory + artworkBrokenFilename;
    std::wstring artworkBrokenPathBackslashes = GetBackslashPath(artworkBrokenPath);

    std::wstring playlistPagePath = workingDirectory + playlistPageFilename;
    std::wstring playlistPagePathBackslashes = GetBackslashPath(playlistPagePath);

    std::wstring playlistArtPagePath = workingDirectory + playlistArtPageFilename;
    std::wstring playlistArtPagePathBackslashes = GetBackslashPath(playlistArtPagePath);

    std::wstring trashPath = workingDirectory + trashFoldername + L"/";
    std::wstring trashPathBackslashes = GetBackslashPath(trashPath);

    std::wstring tracksPath = workingDirectory + tracksFilename;
    std::wstring tracksPathBackslashes = GetBackslashPath(tracksPath);

    std::vector<std::wstring> rv;
    rv.push_back(L"cmd /u /c \"MOVE \"" + workingDirBackslashes + L"\\td8_index*.mp4\" \"" + trashPathBackslashes + L"\"");
    rv.push_back(L"cmd /u /c \"MOVE \"" + artworkBrokenPathBackslashes      + L"\" \"" + trashPathBackslashes + L"\"");
    rv.push_back(L"cmd /u /c \"MOVE \"" + playlistPagePathBackslashes       + L"\" \"" + trashPathBackslashes + L"\"");
    rv.push_back(L"cmd /u /c \"MOVE \"" + playlistArtPagePathBackslashes    + L"\" \"" + trashPathBackslashes + L"\"");
    rv.push_back(L"cmd /u /c \"MOVE \"" + tracksPathBackslashes             + L"\" \"" + trashPathBackslashes + L"\"");
    

    // useful testing command that reverses the previous command so that the method can be run again:
    //rv.push_back(L"cmd /u /c \"MOVE \"" + workingDirBackslashes + L"\\Trash\\td8_index*.mp4\" \"" + trashPathBackslashes + L"\"\"");
    return rv;
}




std::vector<std::wstring> MainFrame::RenameFilesStage(std::wstring ext)
{
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
    /*
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
    */

    return cmds;
}




void MainFrame::GetArtworkStage()
{
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "GetArtworkPre()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");

    std::wstring playlistArtPageHost, playlistArtPageResource;
    SplitHostResourceURL(artworkURL, playlistArtPageHost, playlistArtPageResource);

    unsigned int cFragment = 0;



    mainConsole.PrintLogAndConsoleNarrow("Downloading album artwork...\n");
    mainConsole.PrintLogAndConsole(L"playlistArtPageHost: " + playlistArtPageHost + L"\n");
    mainConsole.PrintLogAndConsole(L"playlistArtPageResource: " + playlistArtPageResource + L"\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");


    std::wstring artworkBrokenPath = workingDirectory + artworkBrokenFilename;
    std::wstring artworkPath = workingDirectory + artworkFilename;

    std::wstring playlistArtPagePath = workingDirectory + playlistArtPageFilename;

    using namespace net;

    GetImage(playlistArtPageHost.c_str(), playlistArtPageResource.c_str(), playlistArtPagePath.c_str(), artworkBrokenPath.c_str());
    PrintConsole("\n\n");
    FixImageData(artworkBrokenPath.c_str(), artworkPath.c_str());

    // Erase the page (resource) .html file data, disabled, since now it's moved to trash dir instead
    //ClearFileData(playlistArtPagePath);

    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}

std::wstring MainFrame::GetArtworkStageAlt()
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





void MainFrame::ValidateFilenameStr(std::wstring& str)
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
    for (int i = 0; i < trackTitles.size(); i++) ValidateFilenameStr(trackTitles[i]);

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



void MainFrame::GetTrackTitles()
{
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "GetTrackTitles()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");
    std::wstring playlistPagePath = workingDirectory + playlistPageFilename;
    std::wstring host, resource;
    SplitHostResourceURL(URL, host, resource);
    GetResource(host.c_str(), resource.c_str(), playlistPagePath.c_str());

    std::string data = "";
    GetFileData(playlistPagePath, &data);
    std::string query = "]},\"title\":{\"runs\":[{\"text\":\"";

    std::vector<std::string> split = splitByStr(data, query, false);

    std::vector<std::string> titles;
    titles.clear();
    for (int i = 1; i < split.size(); i++)
    {
        for (int j = 0; j < split[i].size(); j++)
        {
            if (j == 0) titles.push_back("");
            if (split[i][j] == '"')
            {
                if (j + 2 < split[i].size())
                {
                    if (split[i][j + 1] != '}') titles.pop_back();
                    else if (split[i][j + 2] != ']') titles.pop_back();
                }
                break;
            }
            titles.back() += split[i][j];
        }
    }

    std::string output = "";
    mainConsole.PrintLogAndConsole(L"\n---- Track titles:\n");
    for (auto title : titles)
    {
        std::string current = title + "\n";
        output += current;
        mainConsole.PrintLogAndConsole(DecodeFromUTF8(current));
    }

    std::wstring tracksPath = workingDirectory + tracksFilename;
    WriteDataToFile(output, tracksPath);

    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}

void MainFrame::ResetTracksFile()
{
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "ResetTracksFile()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");

    std::wstring tracksPath = workingDirectory + tracksFilename;
    ClearFileData(tracksPath);

    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   End of function.    ----------------------------\n");
    mainConsole.PrintLogAndConsoleNarrow("\n\n");
}

void MainFrame::LoadTrackTitles()
{
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Executing function:\n" "LoadTrackTitles()" "\n");
    mainConsole.PrintLogAndConsoleNarrow("----------------------------   Start of function.  ----------------------------\n\n");

    std::wstring tracksPath = workingDirectory + tracksFilename;
    std::string data = "";
    if (GetFileData(tracksPath, &data))
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



std::wstring MainFrame::GetTitlesStageAlt()
{
    //std::wstring cmd = L"cmd /a /c \"\"" + workingDirectory + downloaderExec + L"\" -e --print-to-file %(title)s \"tracks\" \"" + URL + L"\"\"";
    std::wstring cmd = L"cmd /a /c \"\"" + workingDirectory + downloaderExec + L"\" -e --print-to-file %(title)s \"" + tracksFilename + L"\" \"" + URL + L"\"\"";
    return cmd;
}





void MainFrame::DisableFields()
{
    fAlbumsDir.Disable();
    fWorkingDir.Disable();
    fConverterDir.Disable();

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
    fConverterDir.Enable();

    fArtist.Enable();
    fAlbumName.Enable();
    fAlbumYear.Enable();

    fURL.Enable();
    fArtworkURL.Enable();

    fBitrate.Enable();
}



bool MainFrame::VerifyFile(std::wstring dir, std::wstring filename)
{
    std::wstring path = dir + filename;

    if (!FileExist(path.c_str())) return false;
    return true;
}



bool MainFrame::ValidateFields()
{
    // VALIDATE DIRECTORIES
    albumsDirectory = fAlbumsDir.GetText();
    workingDirectory = fWorkingDir.GetText();
    converterDirectory = fConverterDir.GetText();

    if (!validDir(albumsDirectory))
    {
        MessageDialog(L"Invalid albums directory:\n\"" + albumsDirectory + L"\"", L"Error");
        return false;
    }
    if (!validDir(workingDirectory))
    {
        MessageDialog(L"Invalid working directory:\n\"" + workingDirectory + L"\"", L"Error");
        return false;
    }
    if (!validDir(converterDirectory))
    {
        MessageDialog(L"Invalid FFmpeg directory:\n\"" + converterDirectory + L"\"", L"Error");
        return false;
    }

    
    // UPDATE FIELDS IN CASE A SLASH AT THE END IS APPENDED OR BACKSLASHES HAVE BEEN REPLACED WITH FORWARDSLASHES
    fConverterDir.SetText(converterDirectory);
    fAlbumsDir.SetText(albumsDirectory);
    fWorkingDir.SetText(workingDirectory);


    // LOCATE EXECUTABLES
    if (!VerifyFile(converterDirectory, converterExec))
    {
        MessageDialog(L"Failed to locate " + converterExec + L" in:\n" + converterDirectory, L"Error");
        return false;
    }

    if (!VerifyFile(workingDirectory, downloaderExec))
    {
        MessageDialog(L"Failed to locate " + downloaderExec + L" in:\n" + workingDirectory, L"Error");
        return false;
    }



    // VALIDATE ALBUM DATA
    artist = fArtist.GetText();
    albumName = fAlbumName.GetText();
    albumYear = fAlbumYear.GetText();

    ValidateFilenameStr(artist);
    ValidateFilenameStr(albumName);
    ValidateFilenameStr(albumYear);




    // GET BITRATE
    unsigned int unvalidatedBitrate = GetNumFromFirstWord(fBitrate.GetSelected());
    if (!fBitrate.GetItems().empty())
    {
        unsigned int lowestBitrate = GetNumFromFirstWord(fBitrate.GetItems()[0]);
        unsigned int highestBitrate = GetNumFromFirstWord(fBitrate.GetItems().back());

        if (unvalidatedBitrate >= lowestBitrate && unvalidatedBitrate <= highestBitrate)
        {
            bitrate = unvalidatedBitrate;
        }
        else
        {
            MessageDialog("Invalid bitrate.\nPlease choose a bitrate from the drop-down list.", "Error");
            return false;
        }
    }
    else bitrate = 192;



    // VALIDATE URLs
    URL = fURL.GetText();
    artworkURL = fArtworkURL.GetText();

    std::vector<std::wstring> validPlaylistURLs = {
        L"https://youtube.com/", L"https://www.youtube.com/", L"http://youtube.com/", L"http://www.youtube.com/"
    };
    if (!validField(URL, validPlaylistURLs))
    {
        std::wstring output = L"Invalid playlist-URL.\n\nRequires either of:";
        for (int i = 0; i < validPlaylistURLs.size(); i++)
        {
            output += L"\n" + validPlaylistURLs[i];
        }

        MessageDialog(output, L"Error");
        return false;
    }

    std::vector<std::wstring> validArtworkURLs = {
        L"https://youtube.com/playlist?", L"https://www.youtube.com/playlist?", L"http://youtube.com/playlist?", L"http://www.youtube.com/playlist?"
    };
    if (!validField(artworkURL, validArtworkURLs))
    {
        if (!validField(URL, validArtworkURLs))
        {
            std::wstring output = L"Invalid artwork-playlist-URL.\n\nRequires either of:";
            for (int i = 0; i < validArtworkURLs.size(); i++)
            {
                output += L"\n" + validArtworkURLs[i];
            }

            MessageDialog(output, L"Error");
            return false;
        }
        else
        {
            SetStatusText("Copying artwork-playlist-URL from playlist-URL");
            fArtworkURL.SetText(URL);
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

bool MainFrame::ValidateFieldsUpdate()
{
    // VALIDATE DIRECTORIES
    workingDirectory = fWorkingDir.GetText();

    if (!validDir(workingDirectory))
    {
        MessageDialog(L"Invalid working directory:\n\"" + workingDirectory + L"\"", L"Error");
        return false;
    }


    // UPDATE FIELDS IN CASE A SLASH AT THE END IS APPENDED OR BACKSLASHES HAVE BEEN REPLACED WITH FORWARDSLASHES
    fWorkingDir.SetText(workingDirectory);


    // LOCATE EXECUTABLES
    if (!VerifyFile(workingDirectory, downloaderExec))
    {
        MessageDialog(L"Failed to locate " + downloaderExec + L" in:\n" + workingDirectory, L"Error");
        return false;
    }

    
    
    // GET BACKSLASH DIRS
    workingDirBackslashes = GetBackslashPath(workingDirectory);
    return true;
}




void MainFrame::OpenSettings()
{
    std::string encoded;
    std::wstring decoded;

    // use FileExist to avoid alert if possible, and silently use default settings instead
    if (FileExist(settingsFilename.c_str()))
    {
        if (GetFileData(settingsFilename.c_str(), &encoded))
        {
            SetStatusText("Settings file not found");
            return;
        }
        decoded = DecodeFromUTF8(encoded);
    }
    else
    {
        decoded = defaultSettings;

        initialOutput += L"\n";
        initialOutput += L"INFO:\n";
        initialOutput += L"You can save settings like set directories and bitrate with (Ctrl+S).\n";
        initialOutput += L"--------------------------------------------------------------------------\n\n";
    }




    std::wstring currentWord = L"";
    enum value_ID {
        albumsDir = 1,
        workingDir,
        converterDir,
        windowX,
        windowY,
        windowWidth,
        windowHeight,
        alertDone,
        bitrateValue,
        none
    };
    unsigned int currentId = 1;

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
                    if (currentId == converterDir) fConverterDir.SetText(currentWord);

                    if (currentId == windowX && isStrNum(currentWord)) SetPosition(wxPoint(std::stoi(currentWord), GetPosition().y));
                    if (currentId == windowY && isStrNum(currentWord)) SetPosition(wxPoint(GetPosition().x, std::stoi(currentWord)));

                    if (currentId == windowWidth  && isStrNum(currentWord)) SetSize(std::stoi(currentWord), GetSize().y);
                    if (currentId == windowHeight && isStrNum(currentWord)) SetSize(GetSize().x, std::stoi(currentWord));

                    if (currentId == alertDone)
                    {
                        if (currentWord == "0") checkAlert.SetValue(false);
                        if (currentWord == "1") checkAlert.SetValue(true);
                    }

                    if (currentId == bitrateValue)
                    {
                        for (int i = 0; i < fBitrate.GetItems().size(); i++)
                        {
                            std::string bitrateOption = fBitrate.GetItem(i);
                            if (currentWord == bitrateOption)
                            {
                                fBitrate.SetSelected(i);
                            }
                        }
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
            }

            // useful for testing:
            //MessageDialog(decoded + L"\n\n\n" + currentWord);
            currentWord = L"";
            currentId++;
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
    decoded += fConverterDir.GetText() + L"\n";


    decoded += toWide(NumToStr(GetPosition().x)) + L"\n";
    decoded += toWide(NumToStr(GetPosition().y)) + L"\n";

    decoded += toWide(NumToStr(GetSize().x)) + L"\n";
    decoded += toWide(NumToStr(GetSize().y)) + L"\n";


    decoded += NumToWstr(checkAlert.GetValue()) + L"\n";

    decoded += toWide(fBitrate.GetSelected()) + L"\n";



    std::string encoded = EncodeToUTF8(decoded);


    if (WriteDataToFile(encoded, path))
    {
        SetStatusText("Cannot save settings");
        return;
    }
    SetStatusText("Settings have been saved");
    MessageDialog("Settings have been saved.", "album-dl");
}




