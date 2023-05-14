// wxWidgets "Hello World" Program

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include <vector>
#include <wininet.h>

// IDs for menu items except lib-predefined
enum
{
    ID_Hello = 1,
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


class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame();
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
    
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);

    void SaveSettings();
    void OpenSettings();

    void LoadTrackTitles();
    void RunScript();
    void ResetTracksFile();

    void GetArtwork();
};

// The main() function, invoking the entry-point of the program
wxIMPLEMENT_APP(MyApp);



bool MyApp::OnInit()
{
    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;

    MyFrame* frame = new MyFrame();
    frame->SetClientSize(ClientWidth, ClientHeight);
    frame->SetPosition(wxPoint(650, 20));
    frame->Show(true);
    return true;
}


MyFrame::MyFrame() : wxFrame(NULL, ID_Frame, "Main window")
{
    // File:
    //      Hello
    wxMenu* menuFile = new wxMenu;
    // Characters preceded by an ampersand in menu item text are the mnemonic underlined-in-alt-mode acces-keys for these menu items
    // The \tShortcut specify the item shortcuts
    menuFile->Append(ID_Hello, "&Hello...\tCtrl+H", "Status bar text for \"Hello\"");
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


    albumsDir_Field = new TextBox("Albums directory", ID_albumsDir_Field,
                                  wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);
    workingDir_Field = new TextBox("Working directory", ID_workingDir_Field,
                                  wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);

    mainOffset.y += TextBoxSize.y + fieldBetweenSpace.y;
    artist_Field = new TextBox("Artist", ID_artist_Field,
                                   wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);
    albumName_Field = new TextBox("Album name", ID_albumName_Field,
                               wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);
    albumYear_Field = new TextBox("Album year", ID_albumYear_Field,
                                  wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);


    tracks_Field = new TextBox("Track titles (auto-filled)", ID_tracks_Field,
                               wxPoint(mainOffset.x, mainOffset.y), LargeBoxSize, mainPanel, true);
    tracks_Field->textField->Disable();

    URL_Field = new TextBox("Playlist URL", ID_URL_Field,
                            wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);

    URL_Artwork_Field = new TextBox("Playlist URL with proper artwork", ID_URL_Artwork_Field,
                                    wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);



    wxButton* button = new wxButton(mainPanel, ID_Button, "Run script",
                                    wxPoint(mainOffset.x, mainOffset.y),
                                    ButtonSize, NULL, wxDefaultValidator, "Script Button");

    checkAlert = new wxCheckBox(mainPanel, ID_AlertOnDone, "Alert on done",
                                wxPoint(mainOffset.x + ButtonSize.x + fieldBetweenSpace.x, mainOffset.y),
                                ButtonSize, 0, wxDefaultValidator, "Alert checkbox");
    mainOffset.y += ButtonSize.y + fieldBetweenSpace.y;

    Bind(wxEVT_BUTTON, &MyFrame::OnButtonPress, this, ID_Button);

    
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);


    ClientWidth = mainOffset.x + TextBoxSize.x + mainOffset.x;
    ClientHeight = mainOffset.y;

    OpenSettings();
    URL_Artwork_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_kULt5j2pKzT5PtLz1RGW7EO-IWDwqVtHw");

    artist_Field->textField->SetFocus();
}

void MyFrame::OpenSettings()
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
            none
        };
        value_ID id = albumsDir;
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
                switch (id)
                {
                    case albumsDir:
                        albumsDir_Field->textField->SetValue(currentWord);
                        id = workingDir;
                        break;
                    case workingDir:
                        workingDir_Field->textField->SetValue(currentWord);
                        id = none;
                        break;
                }
                currentWord = "";
            }

            SetStatusText("Directory settings have beed loaded");
        } while (currentChar != EOF);


        fclose(settingsFile);
    }
    else
    {
        SetStatusText("Cannot load directory settings");
    }
}

void MyFrame::SaveSettings()
{
    FILE* settingsFile = nullptr;
    std::string path = "settings";
    fopen_s(&settingsFile, path.c_str(), "w");
    if (settingsFile != nullptr)
    {
        fputs(albumsDir_Field->textField->GetValue().c_str(), settingsFile); fputc('\n', settingsFile);
        fputs(workingDir_Field->textField->GetValue().c_str(), settingsFile); fputc('\n', settingsFile);


        fclose(settingsFile);
        SetStatusText("Directory settings have been saved");
    }
    else
    {
        SetStatusText("Cannot save directory settings");
    }
}

void MyFrame::OnClose(wxCloseEvent& event)
{
    SaveSettings();
    
    Destroy();
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This program does nothing.", "About", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world!");
}

void MyFrame::OnButtonPress(wxCommandEvent& event)
{
    //RunScript();
    GetArtwork();
}

void MyFrame::RunScript()
{
    std::wstring URL = URL_Field->textField->GetValue().ToStdWstring();

    std::wstring albumsDirectory = albumsDir_Field->textField->GetValue().ToStdWstring();
    std::wstring workingDirectory = workingDir_Field->textField->GetValue().ToStdWstring();
    std::wstring execName = L"yt-dlp.exe";
    std::wstring configName = L"config.txt";

    std::wstring artist = artist_Field->textField->GetValue().ToStdWstring();
    std::wstring albumName = albumName_Field->textField->GetValue().ToStdWstring();
    std::wstring albumYear = albumYear_Field->textField->GetValue().ToStdWstring();


    // TODO:
    // -check for slash at end in paths and add the slash if needed
    // -updating not only screen but events during console process execution
    // -playlist with artowrk URL field
    // -getting the artwork directly from requested HTML from the URL (based on cover.js)
    // -tracks field buffer width checking to avoid title wrapping



    std::wstring albumsDirBackslashes = L"";
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

    std::wstring albumPathBackslashes = albumsDirBackslashes + L"\\" + artist + L" - " + albumName + L" (" + albumYear + L")";

    std::wstring workingDirBackslashes = L"";
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
    
    std::wstring output = L"";
    unsigned long charsWritten;


    // DOWNLOAD
    ///*
    {
        FreeConsole();
        AllocConsole();

        std::wstring args = L"";
        args += L"--config-location \"" + workingDirectory + configName + L"\"";
        args += ' ';
        args += L"-o \"" + workingDirectory + L"td8_index%(playlist_index)s.mp4\" " + URL;

        std::wstring fullCommand = L""; fullCommand += workingDirectory + execName + ' ' + args;
        output = fullCommand + L"\n\n";
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);


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

        output = std::to_wstring(rv) + L", " + std::to_wstring(exit_code) + L", " + std::to_wstring(GetLastError());


        free(buf);
        CloseHandle(startupinfo.hStdInput);
        CloseHandle(startupinfo.hStdOutput);
        CloseHandle(startupinfo.hStdError);
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);

        SetStatusText("Finished DOWNLOAD stage, exit_code: " + std::to_string(exit_code));
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);
        FreeConsole();
    }
    //*/

    // CONVERT
    ///*
    {
        AllocConsole();


        std::wstring cmd = L"forfiles /P \"" + workingDirBackslashes + L"\"" + L" /M td8_index*.mp4 /C \"cmd /c ffmpeg -i @file -c:a mp3 -b:a 192k -ar 44100 @fname.mp3\"";

        output = cmd + L"\n\n";
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);

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

        if (rv != 0)
        {
            output += L"\n\nSuccess, exit_code: " + std::to_wstring(exit_code);
        }
        else
        {
            output += L"Failure, GetLastError() returned: " + std::to_wstring(GetLastError());
        }
        output += '\n';


        
        free(buf);
        CloseHandle(startupinfo.hStdInput);
        CloseHandle(startupinfo.hStdOutput);
        CloseHandle(startupinfo.hStdError);
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);

        SetStatusText("Finished CONVERT stage, exit_code: " + std::to_string(exit_code));
        FreeConsole();
    }
    //*/

    // REMOVE
    ///*
    {
        AllocConsole();


        std::wstring cmd = L"cmd /c \"MOVE \"" + workingDirBackslashes + L"\\td8_index*.mp4\" \"" + workingDirBackslashes + L"\\Trash\"\"";

        output = cmd + L"\n\n";
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);

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

        if (rv != 0)
        {
            output += L"\n\nSuccess, exit_code: " + std::to_wstring(exit_code);
        }
        else
        {
            output += L"Failure, GetLastError() returned: " + std::to_wstring(GetLastError());
        }
        output += '\n';



        free(buf);
        CloseHandle(startupinfo.hStdInput);
        CloseHandle(startupinfo.hStdOutput);
        CloseHandle(startupinfo.hStdError);
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);

        SetStatusText("Finished REMOVE stage, exit_code: " + std::to_string(exit_code));
        FreeConsole();
    }
    //*/
    
    // DOWNLOAD TITLES
    ///*
    {
        AllocConsole();

        ResetTracksFile();

        std::wstring cmd = L""; cmd += workingDirectory + execName + ' ' + L"-e --print-to-file %(title)s \"tracks\" " + URL;
        output = cmd + L"\n\n";
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);


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



        free(buf);
        CloseHandle(startupinfo.hStdInput);
        CloseHandle(startupinfo.hStdOutput);
        CloseHandle(startupinfo.hStdError);
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);

        SetStatusText("Finished DOWNLOAD TITLES stage, exit_code: " + std::to_string(exit_code));
        FreeConsole();
    }
    //*/

    LoadTrackTitles();
    std::vector<std::wstring> trackTitles;
    for (int i = 0; i < tracks_Field->textField->GetNumberOfLines() - 1; i++)
    {
        std::wstring line = L"";
        line += tracks_Field->textField->GetLineText(i).ToStdWstring();
        trackTitles.push_back(line);
    }

    // RENAME
    ///*
    {
        AllocConsole();

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



            output = cmd + L"\n\n";
            WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);

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

            if (rv != 0)
            {
                output += L"\n\nSuccess, exit_code: " + std::to_string(exit_code);
            }
            else
            {
                output += L"Failure, GetLastError() returned: " + std::to_string(GetLastError());
            }
            output += '\n';

            free(buf);
            CloseHandle(startupinfo.hStdInput);
            CloseHandle(startupinfo.hStdOutput);
            CloseHandle(startupinfo.hStdError);
            CloseHandle(process_information.hProcess);
            CloseHandle(process_information.hThread);
        }



        SetStatusText("Finished RENAME stage");
        FreeConsole();
    }
    //*/

    // CREATE DIRECTORY
    ///*
    {
        AllocConsole();

        std::wstring cmd = L"cmd /c \"MKDIR \"" + albumPathBackslashes + L"\"\"";

        output = cmd + L"\n\n";
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);

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

        if (rv != 0)
        {
            output += L"\n\nSuccess, exit_code: " + std::to_string(exit_code);
        }
        else
        {
            output += L"Failure, GetLastError() returned: " + std::to_string(GetLastError());
        }
        output += '\n';



        free(buf);
        CloseHandle(startupinfo.hStdInput);
        CloseHandle(startupinfo.hStdOutput);
        CloseHandle(startupinfo.hStdError);
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);

        SetStatusText("Finished CREATE DIRECTORY stage, exit_code: " + std::to_string(exit_code));
        FreeConsole();
    }
    //*/

    // MOVE
    ///*
    {
        AllocConsole();


        std::wstring cmd = L"cmd /c \"MOVE \"" + workingDirBackslashes + L"\\*.mp3\" \"" + albumPathBackslashes + L"\"\"";

        output = cmd + L"\n\n";
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);

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

        if (rv != 0)
        {
            output += L"\n\nSuccess, exit_code: " + std::to_string(exit_code);
        }
        else
        {
            output += L"Failure, GetLastError() returned: " + std::to_string(GetLastError());
        }
        output += '\n';



        free(buf);
        CloseHandle(startupinfo.hStdInput);
        CloseHandle(startupinfo.hStdOutput);
        CloseHandle(startupinfo.hStdError);
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);
        SetStatusText("Finished MOVE stage, exit_code: " + std::to_string(exit_code));

        FreeConsole();
    }
    //*/


    URL_Field->textField->SetValue("");
    artist_Field->textField->SetValue("");
    albumName_Field->textField->SetValue("");
    albumYear_Field->textField->SetValue("");
    
    SetStatusText("Done");
    if (checkAlert->GetValue() == true) MessageBoxA(NULL, "Script has finished.", "Done", MB_OK);
}

void MyFrame::ResetTracksFile()
{
    FILE* tracksFile = nullptr;
    std::string path = "tracks";
    fopen_s(&tracksFile, path.c_str(), "w");
    if (tracksFile != nullptr) fclose(tracksFile);
}

void MyFrame::LoadTrackTitles()
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
            currentChar = getwc(tracksFile);
            if (iswprint(currentChar))
            {
                currentWord += currentChar;
            }
            else
            {
                if (currentWord.size() > 0) tracks_Field->textField->WriteText(currentWord + (wchar_t)'\n');
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

    ResetTracksFile();
}



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

int GetThumbnailURL(std::string* URL, const char* inputFilename)
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

    *URL = targetCorrect;
    PrintConsole("target:\n" + target + "\n\n");
    PrintConsole("targetCorrect:\n" + targetCorrect + "\n\n");
    PrintConsole("queryIndex: " + std::to_string(queryIndex) + '\n');
    PrintConsole("count: " + std::to_string(count) + '\n');
    //*/

    return 0;
}

void MyFrame::GetArtwork()
{
    FreeConsole();
    AllocConsole();

    std::string URL = URL_Artwork_Field->textField->GetValue().ToStdString();
    std::string host = "";
    std::string resource = "";

    unsigned int cFragment = 0;

    for (int i = 0; i < URL.size(); i++)
    {
        if (cFragment == 0)
        {
            if (URL[i] == ':')
            {
                host = "";
                i += 2;
                continue;
            }
            else if (URL[i] == '/')
            {
                cFragment++;
            }
            else
            {
                host += URL[i];
            }
        }

        if (cFragment == 1)
        {
            resource += URL[i];
        }
    }

    PrintConsole("host: " + host + '\n');
    PrintConsole("resource: " + resource + '\n');
    PrintConsole("\n");

    GetResource(host.c_str(), resource.c_str(), "index2.html");
    std::string thumbnailURL = "";
    GetThumbnailURL(&thumbnailURL, "index2.html");
    PrintConsole("\n\n");




    std::wstring albumsDirectory = albumsDir_Field->textField->GetValue().ToStdWstring();
    std::wstring workingDirectory = workingDir_Field->textField->GetValue().ToStdWstring();
    std::wstring execName = L"yt-dlp.exe";

    std::wstring artist = artist_Field->textField->GetValue().ToStdWstring();
    std::wstring albumName = albumName_Field->textField->GetValue().ToStdWstring();
    std::wstring albumYear = albumYear_Field->textField->GetValue().ToStdWstring();


    // TODO:
    // -tracks field buffer width checking to avoid title wrapping



    std::wstring albumsDirBackslashes = L"";
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

    std::wstring albumPathBackslashes = albumsDirBackslashes + L"\\" + artist + L" - " + albumName + L" (" + albumYear + L")";

    std::wstring workingDirBackslashes = L"";
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

    std::wstring output = L"";
    unsigned long charsWritten;


    // DOWNLOAD
    ///*
    {
        FreeConsole();
        AllocConsole();

        std::wstring args = L"";
        args += L"-o \"" + workingDirectory + L"artwork.png\" \"" + thumbnailURL + "\"";

        std::wstring fullCommand = L""; fullCommand += workingDirectory + execName + ' ' + args;
        output = fullCommand + L"\n\n";
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);


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

        output = L"\n\n" + std::to_wstring(rv) + L", " + std::to_wstring(exit_code) + L", " + std::to_wstring(GetLastError()) + L"\n";


        free(buf);
        CloseHandle(startupinfo.hStdInput);
        CloseHandle(startupinfo.hStdOutput);
        CloseHandle(startupinfo.hStdError);
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);

        SetStatusText("Finished DOWNLOAD ARTWORK stage, exit_code: " + std::to_string(exit_code));
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);
        //FreeConsole();
    }
    //*/
}
