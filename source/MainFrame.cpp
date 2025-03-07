#include "MainFrame.h"
#include "network_utils.h"
#include "tag_utils.h"





wxSize MainFrame::AreaTaken(std::vector<Rectang> controls = std::vector<Rectang>())
{
    // This method returns the coordinates of the lowermost and rightmost corner in given controls

    wxSize AreaTaken = wxSize(0, 0);

    for (int i = 0; i < controls.size(); i++)
    {
        Rectang control = controls[i];
        if (control.pos.x + control.size.x > AreaTaken.x) AreaTaken.x = control.pos.x + control.size.x;
        if (control.pos.y + control.size.y > AreaTaken.y) AreaTaken.y = control.pos.y + control.size.y;
    }

    return AreaTaken;
}

wxSize MainFrame::AreaTakenFields()
{
    return AreaTaken(GetRectangsFields());
}

wxSize MainFrame::AreaTakenInput()
{
    return AreaTaken(GetRectangsInput());
}

wxSize MainFrame::AreaTakenAll()
{
    return AreaTaken(GetRectangsAll());
}

std::vector<Rectang> MainFrame::GetRectangsFields()
{
    std::vector<Rectang> controls;
    controls.push_back(fWorkingDir.rectang);
    controls.push_back(fConverterDir.rectang);
    controls.push_back(fAlbumsDir.rectang);
    controls.push_back(fPreview.rectang);

    controls.push_back(fArtist.rectang);
    controls.push_back(fAlbumName.rectang);
    controls.push_back(fAlbumYear.rectang);

    controls.push_back(fURL.rectang);
    controls.push_back(fArtworkURL.rectang);

    return controls;
}

std::vector<Rectang> MainFrame::GetRectangsInput()
{
    std::vector<Rectang> controls = GetRectangsFields();

    controls.push_back(fBitrate.rectang);
    controls.push_back(Rectang(checkAlert.GetPosition().x, checkAlert.GetPosition().y, checkAlert.GetSize().x, checkAlert.GetSize().y));
    controls.push_back(Rectang(checkUpdate.GetPosition().x, checkUpdate.GetPosition().y, checkUpdate.GetSize().x, checkUpdate.GetSize().y));
    controls.push_back(Rectang(buttonDownload.GetPosition().x, buttonDownload.GetPosition().y, buttonDownload.GetSize().x, buttonDownload.GetSize().y));

    return controls;
}

std::vector<Rectang> MainFrame::GetRectangsAll()
{
    std::vector<Rectang> controls = GetRectangsInput();
    controls.push_back(fOutput.rectang);

    return controls;
}

wxSize MainFrame::GetAreaLeftForTerminal(int terminalWidth)
{
    return wxSize(terminalWidth, AreaTakenInput().y - clientMargin.top);
}



void MainFrame::SetWindowSize()
{
    wxSize area = AreaTakenAll();

    int ClientWidth = area.x + clientMargin.right;
    int ClientHeight = area.y + clientMargin.bottom;

    SetClientSize(ClientWidth, ClientHeight);
}

void MainFrame::AdjustWindowSize()
{
    float devScreenResX = 1920;
    float devScreenResY = 1080;

    bool bTestEndUser = false;
    float userScreenResX, userScreenResY;
    if (!bTestEndUser)
    {
        userScreenResX = GetSystemMetrics(SM_CXSCREEN);
        userScreenResY = GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        userScreenResX = 1366;      // sample low screen resolution for testing
        userScreenResY = 768;
    }

    float devClientResX = GetMinClientSize().x;
    float devClientResY = 600;

    // These calculations attempt to preserve the proportions of client area seen by the developer building the program and the end user
    SetClientSize((devClientResX * userScreenResX) / devScreenResX, (devClientResY * userScreenResY) / devScreenResY);
}

void MainFrame::SetControlPositions(int sepH, int sepV)
{
    // Also sets the terminal size
    //  _________________________________
    // | input       | terminal          |
    // | ____________|__________________ |    sepV
    // ||directories |                  ||      |
    // ||____________|/_________________||______|
    // ||data        |\                 ||
    // ||____________|                  ||
    // ||buttons     |                  ||
    // ||____________|__________________||
    // |_____________|___________________|
    //               ^
    //               |_______ sepH

    fWorkingDir.SetPosition(clientMargin.left,      clientMargin.top);
    fConverterDir.SetPosition(clientMargin.left,    clientMargin.top + fieldHeight * 1);
    fAlbumsDir.SetPosition(clientMargin.left,       clientMargin.top + fieldHeight * 2);
    fPreview.SetPosition(clientMargin.left,         clientMargin.top + fieldHeight * 3);

    fArtist.SetPosition(clientMargin.left,          clientMargin.top + fieldHeight * 3 + sepV + fieldHeight * 1);
    fAlbumName.SetPosition(clientMargin.left,       clientMargin.top + fieldHeight * 3 + sepV + fieldHeight * 2);
    fAlbumYear.SetPosition(clientMargin.left,       clientMargin.top + fieldHeight * 3 + sepV + fieldHeight * 3);

    fURL.SetPosition(clientMargin.left,             clientMargin.top + fieldHeight * 3 + sepV + fieldHeight * 4);
    fArtworkURL.SetPosition(clientMargin.left,      clientMargin.top + fieldHeight * 3 + sepV + fieldHeight * 5);

    fBitrate.SetPosition(clientMargin.left,                                                                             clientMargin.top + fieldHeight * 3 + sepV + fieldHeight * 6 + verticalDropDownOffset);
    
    checkAlert.SetPosition(wxPoint(clientMargin.left + fBitrate.GetSize().x + buttonBreak,                              clientMargin.top + fieldHeight * 3 + sepV + fieldHeight * 6 + verticalCheckBoxOffset));
    checkUpdate.SetPosition(wxPoint(clientMargin.left + fBitrate.GetSize().x + buttonBreak + checkAlert.GetSize().x,    clientMargin.top + fieldHeight * 3 + sepV + fieldHeight * 6 + verticalCheckBoxOffset));

    buttonDownload.SetPosition(wxPoint(0,   clientMargin.top + fieldHeight * 3 + sepV + fieldHeight * 6));

    fOutput.SetPosition(wxPoint(AreaTakenInput().x + sepH, clientMargin.top));
    fOutput.SetSize(GetAreaLeftForTerminal(minTerminalWidth));
    // Force minimal width, but adjust to height given by the input section

    buttonDownload.SetPosition(wxPoint(fOutput.GetPosition().x  - sepH - buttonDownload.GetSize().x, buttonDownload.GetPosition().y));
}





// -- INIT METHODS - CALLED BY CONSTRUCTOR
void MainFrame::InitValues()
{
    bDone = true;
    bResetFields = true;
    bLoadedSettings = false;

    uMaxOutputLines = 150;

    bitrate = 0;

    workingDirectoryDefault = L"workfolder/";

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




    defaultPos.x = 0;
    defaultPos.y = 0;



    fieldHeight = 40;

    fieldBreakV = 20;
    fieldBreakH = 10;

    buttonBreak = 10;

    // offsets relative to a button, since some controls are not aligned evenly
    verticalCheckBoxOffset = 0;
    verticalDropDownOffset = 2;

    minTerminalWidth = 700;

    //clientMargin = { 28, 20, 20, 20 };
    clientMargin = { 18, 10, 10, 10 };

    minTextBoxSize = wxSize(420, 20);
    maxTextBoxSize = wxSize(600, minTextBoxSize.y);

    minButtonSize = wxSize(100, 25);

    minCheckSize = wxSize(25, 25);
}

void MainFrame::InitMenuAndStatusBar()
{
    // Characters preceeded by an ampersand in the menu item text are the mnemonics underlined in alt-mode/acces-keys for these menu items
    // The \t<Shortcut> specify the item shortcuts
    // The third arg is the status bar text displayed on hovering the item

    
    // File          | Info
    // -----------------------
    // Save settings | About
    // Exit          | License

    
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_menuSave, "&Save settings\tCtrl+S", "Saves: directories, window position/size, notify-on-done and bitrate.");
    menuFile->AppendSeparator();
    menuFile->Append(ID_menuExit, "&Exit\tEsc", "Close this program.");

    wxMenu* menuInfo = new wxMenu;
    menuInfo->Append(ID_menuAbout, "&About", "About this program.");
    menuInfo->Append(ID_menuLicense, "&License", "See license.");



    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuInfo, "&Info");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("");
}



void MainFrame::InitControls()
{
    // default sizes and pos because it's automatically stretched to the frame anyway
    mainPanel.Create(this, ID_framePanel, wxDefaultPosition, wxDefaultSize, 2621440L, "Main Panel");
    wxWindow* parent = &mainPanel;

    // Here control dimensions are set to minimal sizes and positions the program will allow during runtime
    fWorkingDir.Init(ID_fWorkingDir,        minTextBoxSize, parent);
    fConverterDir.Init(ID_fConverterDir,    minTextBoxSize, parent);
    fAlbumsDir.Init(ID_fAlbumsDir,          minTextBoxSize, parent);
    fPreview.Init(ID_fPreview,              minTextBoxSize, parent);

    fArtist.Init(ID_fArtist,                minTextBoxSize, parent);
    fAlbumName.Init(ID_fAlbumName,          minTextBoxSize, parent);
    fAlbumYear.Init(ID_fAlbumYear,          minTextBoxSize, parent);

    fURL.Init(ID_fURL,                      minTextBoxSize, parent);
    fArtworkURL.Init(ID_fArtworkURL,        minTextBoxSize, parent);

    fBitrate.Init(ID_fBitrate,              minButtonSize, parent);

    checkAlert.Create(parent,           ID_checkAlert, "", wxDefaultPosition, wxSize(120, 25));
    checkUpdate.Create(parent,          ID_checkUpdate,   "", wxDefaultPosition, wxSize(60, 25));
    buttonDownload.Create(parent,       ID_buttonDownload, "");

    buttonDownload.SetSize(minButtonSize);
    
    fOutput.Init(ID_fOutput, minTextBoxSize, parent, wxTE_MULTILINE | wxTE_READONLY);
}

void MainFrame::InitControlLabels()
{
    fWorkingDir.SetLabel(L"Working directory:");
    fConverterDir.SetLabel(L"ffmpeg.exe directory:");
    fAlbumsDir.SetLabel(L"Albums directory:");
    fPreview.SetLabel(L"Album path (preview):");

    fArtist.SetLabel(L"Artist");
    fAlbumName.SetLabel(L"Album name:");
    fAlbumYear.SetLabel(L"Album year:");

    fURL.SetLabel(L"Playlist URL:");
    fArtworkURL.SetLabel(L"Playlist URL with artwork:");

    fBitrate.SetLabel(L"Audio bitrate:");
    fBitrate.SetValue(L"----");

    checkAlert.SetLabel(L"Notify when done");
    checkUpdate.SetLabel(L"Update");

    buttonDownload.SetLabel(L"Download");

    fOutput.SetLabel(L"Output:");



    // Set hints:
    fConverterDir.SetHint(L"This location has to contain ffmpeg.exe");
    fAlbumsDir.SetHint(L"The album will be moved to this location");
    fPreview.SetEditable(false);
    //fPreview.Disable();
    fArtist.SetHint(L"optional: used for album path and filenames");
    fAlbumName.SetHint(L"optional: used for album path");
    fAlbumYear.SetHint(L"optional: used for album path");
    fURL.SetHint(L"https://youtube.com/playlist?list=...");
    fArtworkURL.SetHint(L"optional: use if the other URL is a normal playlist with no artwork thumbnail");
}



void MainFrame::InitControlPositions()
{
    SetControlPositions(fieldBreakH, fieldBreakV);
}

void MainFrame::InitControlDimensionRanges()
{
    // FIELDS MIN SIZES
    fWorkingDir.SetMinSize(minTextBoxSize);
    fConverterDir.SetMinSize(minTextBoxSize);
    fAlbumsDir.SetMinSize(minTextBoxSize);
    fPreview.SetMinSize(minTextBoxSize);
    
    fArtist.SetMinSize(minTextBoxSize);
    fAlbumName.SetMinSize(minTextBoxSize);
    fAlbumYear.SetMinSize(minTextBoxSize);
    fURL.SetMinSize(minTextBoxSize);
    fArtworkURL.SetMinSize(minTextBoxSize);
    
    fOutput.SetMinSize(fOutput.GetSize());


    // FIELDS MAX SIZES
    fWorkingDir.SetMaxSize(maxTextBoxSize);
    fConverterDir.SetMaxSize(maxTextBoxSize);
    fAlbumsDir.SetMaxSize(maxTextBoxSize);
    fPreview.SetMaxSize(maxTextBoxSize);
    
    fArtist.SetMaxSize(maxTextBoxSize);
    fAlbumName.SetMaxSize(maxTextBoxSize);
    fAlbumYear.SetMaxSize(maxTextBoxSize);
    fURL.SetMaxSize(maxTextBoxSize);
    fArtworkURL.SetMaxSize(maxTextBoxSize);

    fOutput.SetMaxSize(-1, -1);
}



void MainFrame::InitBindings()
{
    Bind(wxEVT_BUTTON, &MainFrame::OnButtonDownload, this, ID_buttonDownload);

    mainPanel.Bind(wxEVT_SIZE, &MainFrame::OnPanelResize, this, ID_framePanel);

    Bind(wxEVT_MENU, &MainFrame::OnSave, this, ID_menuSave);
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, ID_menuExit);

    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, ID_menuAbout);
    Bind(wxEVT_MENU, &MainFrame::OnLicense, this, ID_menuLicense);

    Bind(wxEVT_TEXT, &MainFrame::OnAlbumPathPreview, this, ID_fAlbumsDir);
    Bind(wxEVT_TEXT, &MainFrame::OnAlbumPathPreview, this, ID_fArtist);
    Bind(wxEVT_TEXT, &MainFrame::OnAlbumPathPreview, this, ID_fAlbumName);
    Bind(wxEVT_TEXT, &MainFrame::OnAlbumPathPreview, this, ID_fAlbumYear);

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
    
    // - add blinking cursor at insertion point (e.g. char: _)
    // - custom foreground, background and selection colors
    // - directory boxes instead of text boxes for dir fields
    // 
    // 2025-01-21
    // - create a common base for TextBox and DropDown
    // - refactor OnPanelResize
    // - swap update button to a checkbox
    // 
}

void MainFrame::InitFonts()
{
    // Considered fonts:
    // Notepad++:   Courier New, 10 | 0xFFFFFF, 0x1E1E1E
    // Console:     Terminal, 8x12  | 0xC0C0C0, 0x000000

    

    wxFont outputFont;
    // Some monospace fonts:
    //outputFont = wxFont(wxFontInfo(wxSize(8, 16)).FaceName("Courier New").Bold());
    //outputFont = wxFont(wxFontInfo(wxSize(8, 12)).FaceName("Consolas").Bold());
    //outputFont = wxFont(wxFontInfo(wxSize(8, 12)).FaceName("The One True Font (System 8x12)"));
    //
    //outputFont = wxFont(wxFontInfo(wxSize(8, 12)).FaceName("Terminal"));      // requires code-page 852 encoding
    //fOutput.SetEncoding(CP852);

    // FIXEDSYS VARIANTS:
    // no ligatures
    outputFont = wxFont(wxFontInfo(12).FaceName("Fixedsys"));

    // a few ligatures
    //outputFont.AddPrivateFont("FSEX300.ttf");
    //outputFont = wxFont(wxFontInfo(12).FaceName("Fixedsys Excelsior 3.01"));

    // the most ligatures
    //outputFont.AddPrivateFont("FSEX302.ttf");
    //outputFont.AddPrivateFont("FSEX302-alt.ttf");
    //outputFont = wxFont(wxFontInfo(12).FaceName("Fixedsys Excelsior"));


    
    
    // Attempt to set the primary font
    fOutput.SetFont(outputFont);
    
    // Set alternative font in case of failure to set the primary font
    std::wstring outputFaceName = fOutput.GetFontFaceName();
    if (outputFaceName != "Fixedsys")
    {
        outputFont = wxFont(wxFontInfo(wxSize(8, 16)).FaceName("Courier New").Bold());
        fOutput.SetFont(outputFont);
    }
    
    // Font testing:
    //mainConsole.PrintLogAndConsole(testUnicode(outputFaceName));
    //mainConsole.PrintLogAndConsole(testNoLigature());
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

void MainFrame::InitTestValues()
{
    //bResetFields = false;
    //fBitrate.SetSelected(3);

    
    // -- SAMPLE TEST VALUES FOR CONVENIENCE:
    
    // SHORT PLAYLISTS
    //fArtist.SetText(L"Big Black");
    //fAlbumName.SetText(L"Racer-X");
    //fAlbumYear.SetText(L"1985");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_nrAFOfF6ITDAEJ-BuHYWpHYOwsKNTZ994");

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
    //fAlbumName.SetText(L"Tylko Dla Doros�ych");
    //fAlbumYear.SetText(L"2010");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=PLIKxxmyVA3HZ5vCNl3b0gQXDhuMWLz-mG");
    //fArtworkURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_l6DSlExq2EbVR7ILChbL9ZHn-1SbyKRO8");

    // TYPICAL UNICODE TITLES
    //fArtist.SetText(L"Goat");
    //fAlbumName.SetText(L"World Music");
    //fAlbumYear.SetText(L"2012");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_nMsUDBQ3_Xsjdz62NkJ_g1HnEirKtRkZg");
    ////fArtworkURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_nMsUDBQ3_Xsjdz62NkJ_g1HnEirKtRkZg");

    // NORMAL PLAYLIST
    // Proves GetTrackTitlesAlt() to be incomplete, with titles ending up containing unparsed code-points like: "... \u0026 ..."
    //fArtist.SetText(L"Supergrass");
    //fAlbumName.SetText(L"Road To Rouen");
    //fAlbumYear.SetText(L"2005");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=PLHTo__bpnlYURCrPK2lf1onaXhWKlzcYl");
    //fArtworkURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_l-QlUzRsn3KV4PvzkGxWgeUbiae67USgo");

    //fArtist.SetText(L"Deftones");
    //fAlbumName.SetText(L"Around The Fur");
    //fAlbumYear.SetText(L"1997");
    //fURL.SetText(L"https://www.youtube.com/playlist?list=PLbUIPZJL6vw-3om2INMgmkAkKeaZnkag2");
    //fArtworkURL.SetText(L"https://www.youtube.com/playlist?list=OLAK5uy_nRSkRJt8rcj69IxpxI4GP1ilibtxJmoBg");

    // --
}



void MainFrame::InitWindowSize()
{
    // Initialize minimal window size first
    SetWindowSize();
    SetMinSize(GetSize());

    // Adjust a larger window size relative to user screen
    //AdjustWindowSize();
}

void MainFrame::InitPosition()
{
    SetPosition(wxPoint(defaultPos.x, defaultPos.y));   // SET WINDOW POS TO DEFAULT POS
}

void MainFrame::InitSettings()
{
    // Load settings from file, may repos & resize window.
    OpenSettings();
}



void MainFrame::InitFocus()
{
    if (!bLoadedSettings) fConverterDir.SetFocus();
    else
    {
        //buttonDownload.SetFocus();
        fArtist.SetFocus();
        //fURL.SetFocus();
    }
}



void MainFrame::InitDimensionsInfo()
{
    dimensionsInfo = L"";

    float screenX, screenY;
    screenX = GetSystemMetrics(SM_CXSCREEN);
    screenY = GetSystemMetrics(SM_CYSCREEN);

    dimensionsInfo += L"screen res: " + NumToWstr((int)screenX) + L"x" + NumToWstr((int)screenY) + L"\n";

    char whitespace = ' ';

    std::vector<Rectang> controls = GetRectangsAll();
    for (int i = 0; i < controls.size(); i++)
    {
        dimensionsInfo += L"rectangs[" + NumToWstr(i, 10, 2, whitespace) + L"]: (";
        dimensionsInfo += NumToWstr(controls[i].pos.x, 10, 3, ' ') + L", " + NumToWstr(controls[i].pos.y, 10, 3, ' ') + L"), (";
        dimensionsInfo += NumToWstr(controls[i].size.x, 10, 3, ' ') + L", " + NumToWstr(controls[i].size.y, 10, 3, ' ') + L")\n";
    }

    dimensionsInfo += L"\n";
    dimensionsInfo += L"GetClientSize():     " + NumToWstr(GetClientSize().x) + L"x" + NumToWstr(GetClientSize().y) + L"\n";
    dimensionsInfo += L"GetMinClientSize():  " + NumToWstr(GetMinClientSize().x) + L"x" + NumToWstr(GetMinClientSize().y) + L"\n";
    dimensionsInfo += L"GetSize():           " + NumToWstr(GetSize().x) + L"x" + NumToWstr(GetSize().y) + L"\n";
    dimensionsInfo += L"GetMinSize():        " + NumToWstr(GetMinSize().x) + L"x" + NumToWstr(GetMinSize().y) + L"\n";
}

void MainFrame::InitVerifyExecutables()
{
    downloaderExecInfo = L"";
    converterExecInfo = L"";

    // verify execs only if settings have been loaded, otherwise empty fields might be the user not setting any directories yet
    if (!bLoadedSettings) return;


    workingDirectory = fWorkingDir.GetText();
    converterDirectory = fConverterDir.GetText();

    // verify downloader exec
    if (!VerifyFile(workingDirectory, downloaderExec))
    {
        downloaderExecInfo += L"Failed to locate '" + downloaderExec + L"' in:\n";
        downloaderExecInfo += L"\"" + workingDirectory + L"\"\n";

        fWorkingDir.SetFocus();
    }

    // verify converter exec
    if (!VerifyFile(converterDirectory, converterExec))
    {
        converterExecInfo += L"Failed to locate '" + converterExec + L"' in:\n";
        converterExecInfo += L"\"" + converterDirectory + L"\"\n";

        if (downloaderExecInfo.empty()) fConverterDir.SetFocus();
    }
}

void MainFrame::InitTerminalOutput()
{
    initialOutput = L"";
    initialOutput += L"--------------------------------------------------------------------------\n";
    initialOutput += L"|            Welcome to album-dl's output terminal, stand-by.            |\n";
    initialOutput += L"--------------------------------------------------------------------------\n\n";

    if (downloaderExecInfo.empty() && converterExecInfo.empty())
    {
        initialOutput += L"Enter an album playlist URL and optional data.\n";
        initialOutput += L"Press 'Download' to attempt downloading the album.\n";

        if (!bLoadedSettings) initialOutput += L"\nPress 'Ctrl + S' to save directories, window position/size & preferred bitrate.\n";
        initialOutput += L"--------------------------------------------------------------------------\n\n";
    }
    else
    {
        initialOutput += downloaderExecInfo;
        if (!downloaderExecInfo.empty() && !converterExecInfo.empty()) initialOutput += L"\n";
        initialOutput += converterExecInfo;

        if (!converterExecInfo.empty())
        {
            initialOutput += L"\nalbum-dl requires FFmpeg to work, if you don't have FFmpeg installed, visit:\n";
            initialOutput += L"https://ffmpeg.org/download.html\n";
        }
        initialOutput += L"--------------------------------------------------------------------------\n\n";
    }

    if (!dimensionsInfo.empty()) initialOutput += dimensionsInfo;
    

    mainConsole.PrintLogAndConsole(initialOutput);
    initialOutput.clear();
}
// --



MainFrame::MainFrame() : wxFrame(NULL, ID_mainFrame, "album-dl")
{
    // -- INIT METHODS
    InitValues();
    InitMenuAndStatusBar();

    InitControls();                   // SET LABELS AND CONSTRUCT FIELDS
    InitControlPositions();
    InitControlDimensionRanges();

    InitControlLabels();

    InitBindings();

    InitConsole();

    InitThemes();
    InitFonts();

    InitBitrates();
    InitTestValues();

    InitWindowSize();
    InitPosition();
    InitSettings();

    InitFocus();
    
    //InitDimensionsInfo();
    InitVerifyExecutables();
    InitTerminalOutput();
    // --


    // Window is ready to be shown.
    Show(true);
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

void MainFrame::OnAlbumPathPreview(wxCommandEvent& event)
{
    //MessageDialog("OnAlbumPathPreview()");
    std::wstring albumsDirPreview = fAlbumsDir.GetText();
    if (!validDir(albumsDirPreview))
    {
        fPreview.SetText(L"<invalid-directory>");
        return;
    }

    std::wstring artistPreview = fArtist.GetText();
    std::wstring albumNamePreview = fAlbumName.GetText();
    std::wstring albumYearPreview = fAlbumYear.GetText();

    ValidateFilenameStr(artistPreview);
    ValidateFilenameStr(albumNamePreview);
    ValidateFilenameStr(albumYearPreview);

    std::wstring albumPathPreview = albumsDirPreview + artistPreview + L" - " + albumNamePreview + L" " + L"(" + albumYearPreview + L")" + L"/";
    fPreview.SetText(albumPathPreview);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    MainDialog* newDialog = new MainDialog(L"About", toWide(GetAbout()));
    // Center the dialog, relative to the main-frame
    wxPoint dialogPosition(GetPosition().x + GetSize().x / 2.f - newDialog->GetSize().x / 2.f,
                           GetPosition().y + GetSize().y / 2.f - newDialog->GetSize().y / 2.f);
    if (dialogPosition.x < 0) dialogPosition.x = 0;
    if (dialogPosition.y < 0) dialogPosition.y = 0;


    newDialog->SetPosition(dialogPosition);
    newDialog->Show(true);
}

void MainFrame::OnLicense(wxCommandEvent& event)
{
    MainDialog* newDialog = new MainDialog(L"License", toWide(GetLicense()));
    wxPoint dialogPosition(GetPosition().x + GetSize().x / 2.f - newDialog->GetSize().x / 2.f,
                           GetPosition().y + GetSize().y / 2.f - newDialog->GetSize().y / 2.f);
    if (dialogPosition.x < 0) dialogPosition.x = 0;
    if (dialogPosition.y < 0) dialogPosition.y = 0;


    newDialog->SetPosition(dialogPosition);
    newDialog->Show(true);
}

void MainFrame::OnSave(wxCommandEvent& event)
{
    // File / Save settings
    SaveSettings();
}

void MainFrame::OnButtonDownload(wxCommandEvent& event)
{
    if (!bDone) return;
    if (!ValidateFields()) return;


    DisableFields();
    bDone = false;

    if (workingThread.joinable()) workingThread.join();
    workingThread = std::move(std::thread(&MainFrame::GetAlbum, this));
}

void MainFrame::OnPanelResize(wxSizeEvent& event)
{
    // direction doesn't matter
    int newClientWidth = event.GetSize().x;
    int newClientHeight = event.GetSize().y;

    
    // CELL 1
    // | clientMargin.left | fExtra.GetMinSize().x + <increase> | fieldBreakHorizontal | fOutput.GetMinSize().x + <increase> | clientMargin.right |
    int hIncreaseTotal = newClientWidth;
    hIncreaseTotal -= clientMargin.right;
    hIncreaseTotal -= fOutput.GetMinSize().x;
    hIncreaseTotal -= fieldBreakH;
    hIncreaseTotal -= minTextBoxSize.x;
    hIncreaseTotal -= clientMargin.left;

    int hIncreaseCell1 = hIncreaseTotal;
    int hIncreaseCell2 = 0;
    if (minTextBoxSize.x + hIncreaseCell1 > maxTextBoxSize.x)
    {
        hIncreaseCell1 = maxTextBoxSize.x - minTextBoxSize.x;
        hIncreaseCell2 = hIncreaseTotal - hIncreaseCell1;
    }
    
    int vIncreaseTotal = newClientHeight;

    int vIncreaseCell2 = vIncreaseTotal;
    vIncreaseCell2 -= clientMargin.bottom;
    vIncreaseCell2 -= fOutput.GetMinSize().y;
    vIncreaseCell2 -= clientMargin.top;
    

    fWorkingDir.SetSize(fWorkingDir.GetMinSize().x + hIncreaseCell1, fWorkingDir.GetSize().y);
    fConverterDir.SetSize(fConverterDir.GetMinSize().x + hIncreaseCell1, fConverterDir.GetSize().y);
    fAlbumsDir.SetSize(fAlbumsDir.GetMinSize().x + hIncreaseCell1, fAlbumsDir.GetSize().y);
    fPreview.SetSize(fPreview.GetMinSize().x + hIncreaseCell1, fPreview.GetSize().y);
    
    fArtist.SetSize(fArtist.GetMinSize().x + hIncreaseCell1, fArtist.GetSize().y);
    fAlbumName.SetSize(fAlbumName.GetMinSize().x + hIncreaseCell1, fAlbumName.GetSize().y);
    fAlbumYear.SetSize(fAlbumYear.GetMinSize().x + hIncreaseCell1, fAlbumYear.GetSize().y);
    fURL.SetSize(fURL.GetMinSize().x + hIncreaseCell1, fURL.GetSize().y);
    fArtworkURL.SetSize(fArtworkURL.GetMinSize().x + hIncreaseCell1, fArtworkURL.GetSize().y);

    wxSize maxDistance1 = AreaTakenFields();



    // CELL 2
    fOutput.SetPosition(maxDistance1.x + fieldBreakH, fOutput.GetPosition().y);
    fOutput.SetSize(fOutput.GetMinSize().x + hIncreaseCell2, fOutput.GetMinSize().y + vIncreaseCell2);



    // CELL 3
    int vOffset3 = newClientHeight;
    vOffset3 -= clientMargin.bottom;
    
    vOffset3 -= std::max({verticalDropDownOffset, verticalCheckBoxOffset});

    vOffset3 -= buttonDownload.GetSize().y;

    vOffset3 -= fieldHeight;        // fArtworkURL
    vOffset3 -= fieldHeight;        // fURL

    vOffset3 -= fieldHeight;        // fAlbumYear
    vOffset3 -= fieldHeight;        // fAlbumName
    vOffset3 -= fieldHeight;        // fArtist

    if (vOffset3 >= clientMargin.top + fieldHeight * 3 + fieldBreakV)
    {
        fArtist.SetPosition(fArtist.GetPosition().x, vOffset3 + fieldHeight * 0);
        fAlbumName.SetPosition(fAlbumName.GetPosition().x, vOffset3 + fieldHeight * 1);
        fAlbumYear.SetPosition(fAlbumYear.GetPosition().x, vOffset3 + fieldHeight * 2);

        fURL.SetPosition(fURL.GetPosition().x, vOffset3 + fieldHeight * 3);
        fArtworkURL.SetPosition(fArtworkURL.GetPosition().x, vOffset3 + fieldHeight * 4);


        buttonDownload.SetPosition(wxPoint(buttonDownload.GetPosition().x, vOffset3 + fieldHeight * 5));
        checkUpdate.SetPosition(wxPoint(checkUpdate.GetPosition().x, vOffset3 + fieldHeight * 5));
        checkAlert.SetPosition(wxPoint(checkAlert.GetPosition().x, vOffset3 + fieldHeight * 5 + verticalCheckBoxOffset));
        fBitrate.SetPosition(fBitrate.GetPosition().x, vOffset3 + fieldHeight * 5 + verticalDropDownOffset);
    }





    // CELL 4
    Rect b1_1, b1_2, b2_1, b2_2;
    wxSize refreshMargin(0, 0);

    int hOffset4 = newClientWidth - clientMargin.right - fOutput.GetSize().x - fieldBreakH - buttonDownload.GetSize().x;
    int buttonX = hOffset4;

    if (hOffset4 <= clientMargin.left + minTextBoxSize.x - buttonDownload.GetSize().x)
    {
        buttonX = clientMargin.left + minTextBoxSize.x - buttonDownload.GetSize().x;
    }

    b1_1 = Rect{    buttonDownload.GetPosition().x - refreshMargin.x,
                    buttonDownload.GetPosition().y - refreshMargin.y,
                    buttonDownload.GetSize().x + refreshMargin.x * 2,
                    buttonDownload.GetSize().y + refreshMargin.y * 2 };
    buttonDownload.SetPosition(wxPoint(buttonX, buttonDownload.GetPosition().y));
    b1_2 = Rect { buttonDownload.GetPosition().x - refreshMargin.x,
                    buttonDownload.GetPosition().y - refreshMargin.y,
                    buttonDownload.GetSize().x + refreshMargin.x * 2,
                    buttonDownload.GetSize().y + refreshMargin.y * 2 };

    // Compare smoothness with this bool
    bool bRefreshWholeClientArea = true;
    if (bRefreshWholeClientArea)
    {
        // Refresh whole client area
        Refresh();
        return;
    }
    //buttonDownload.Refresh();
    //return;
    
    // Refresh only dirty regions around the button
    Rect reg = RectUnion(b1_1, b1_2);
    RefreshRect(wxRect(reg.x, reg.y, reg.width, reg.height));
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

    if (checkUpdate.GetValue() == true)
    {
        mainConsole.AddCmd(L"\"" + workingDirectory + downloaderExec + L"\" --update", WINDOWS1250);
        ExecuteBatchSession();
    }

    //--------------------------------------------------
    mainConsole.AddCmd(DownloadStage(), WINDOWS1250);
    ExecuteBatchSession();

    
    //--------------------------------------------------
    GetArtworkStage();



    //--------------------------------------------------
    ResetTracksFile();
    mainConsole.AddCmd(GetTitlesStage());
    ExecuteBatchSession();

    LoadTrackTitles();
    ValidateTrackTitles();
    ResetTracksFile();
    
    
    //--------------------------------------------------
    mainConsole.AddCmd(ConvertStage(), UTF8);
    mainConsole.AddCmd(CreateTrashDirStage());
    mainConsole.AddCmd(RemoveLeftoverStage());
    mainConsole.AddCmd(RenameFilesStage());
    ExecuteBatchSession();


    //--------------------------------------------------
    AttachArtworkToAll();

    mainConsole.AddCmd(CreateAlbumDirectoryStage());
    mainConsole.AddCmd(MoveAudioStage());
    mainConsole.AddCmd(MoveArtworkStage());
    ExecuteBatchSession();

    
    // FIELDS VALUE RESET
    if (bResetFields)
    {
        SetStatusText("Resetting...");
    
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

std::wstring MainFrame::GetArtworkStageAlt(std::wstring targetURL)
{
    std::wstring args = L"";
    args += L"-o \"" + workingDirectory + artworkFilename + "\" \"" + targetURL + "\"";

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
    
    SetStatusText("Attaching cover metadata.");
    mainConsole.PrintLogAndConsoleNarrow("Attaching cover metadata\n");

    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring trackFilepath = workingDirectory + std::to_wstring(i + 1) + L". " + artist + L" - " + trackTitles[i] + L".mp3";
        AttachArtwork(trackFilepath, artworkFilename);
    }

    SetStatusText("Finished attaching cover metadata.");
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

    SetStatusText("Checking for unwanted and forbidden characters in track-titles...");
    for (int i = 0; i < trackTitles.size(); i++) ValidateFilenameStr(trackTitles[i]);

    PrintTracks();
    SetStatusText("Track-titles validated.");
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



std::wstring MainFrame::GetTitlesStage()
{
    std::wstring tracksPath = workingDirectory + tracksFilename;
    std::wstring cmd = L"cmd /a /c \"\"" + workingDirectory + downloaderExec + L"\"";
    std::wstring args = L" --no-warnings --print title --print-to-file %(title)s \"" + tracksPath + L"\" \"" + URL + L"\"\"";
    
    cmd += args;
    return cmd;
}

void MainFrame::GetTrackTitlesAlt()
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
        SetStatusText("Failed to load track-titles from file.");
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
        SetStatusText("Track-titles loaded.");

        
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
    if (!VerifyFile(workingDirectory, downloaderExec))
    {
        MessageDialog(L"Failed to locate " + downloaderExec + L" in:\n\"" + workingDirectory + L"\"", L"Error");
        return false;
    }
    if (!VerifyFile(converterDirectory, converterExec))
    {
        MessageDialog(L"Failed to locate " + converterExec + L" in:\n\"" + converterDirectory + L"\"", L"Error");
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
            MessageDialog("Invalid bitrate.\nChoose a bitrate from the drop-down list.", "Error");
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
            SetStatusText("Copying artwork-playlist-URL from playlist-URL.");
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
        MessageDialog(L"Failed to locate " + downloaderExec + L" in:\n\"" + workingDirectory + L"\"", L"Error");
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
            fWorkingDir.SetText(workingDirectoryDefault);

            bLoadedSettings = false;
            return;
        }
        decoded = DecodeFromUTF8(encoded);

        bLoadedSettings = true;
    }
    else
    {
        fWorkingDir.SetText(workingDirectoryDefault);

        bLoadedSettings = false;
        return;
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
        checkedNotify,
        checkedUpdate,
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

                    if (currentId == checkedNotify)
                    {
                        if (currentWord == "0") checkAlert.SetValue(false);
                        if (currentWord == "1") checkAlert.SetValue(true);
                    }

                    if (currentId == checkedUpdate)
                    {
                        if (currentWord == "0") checkUpdate.SetValue(false);
                        if (currentWord == "1") checkUpdate.SetValue(true);
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

    SetStatusText("Settings have beed loaded.");
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
    decoded += NumToWstr(checkUpdate.GetValue()) + L"\n";

    decoded += toWide(fBitrate.GetSelected()) + L"\n";



    std::string encoded = EncodeToUTF8(decoded);


    if (WriteDataToFile(encoded, path))
    {
        SetStatusText("Cannot save settings.");
        return;
    }
    SetStatusText("Settings have been saved.");
    MessageDialog("Settings have been saved.", "album-dl");
}




