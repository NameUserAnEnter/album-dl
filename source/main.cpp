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
    
    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);

    void SaveSettings();
    void OpenSettings();

    void LoadTrackTitles();
    void RunScript();
    void ResetTracksFile();

    void GetArtwork();
    void AttachArtwork(std::wstring, std::wstring);
private:
    std::wstring URL;

    std::wstring albumsDirectory;
    std::wstring workingDirectory;

    std::wstring execName;
    std::wstring configName;

    std::wstring artist;
    std::wstring albumName;
    std::wstring albumYear;

    std::vector<std::wstring> trackTitles;
    std::wstring artworkFilename;
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
    frame->SetPosition(wxPoint(1020, 20));
    frame->Show(true);
    return true;
}


MyFrame::MyFrame() : wxFrame(NULL, ID_Frame, "Script manager")
{
    // File:
    //      Hello
    wxMenu* menuFile = new wxMenu;
    // Characters preceded by an ampersand in menu item text are the mnemonic underlined-in-alt-mode acces-keys for these menu items
    // The \tShortcut specify the item shortcuts
    menuFile->Append(ID_Save, "&Save directories\tCtrl+S", "Save Albums directory & Working directory");
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


    tracks_Field = new TextBox("Track titles (auto-filled):", ID_tracks_Field,
                               wxPoint(mainOffset.x, mainOffset.y), LargeBoxSize, mainPanel, true);
    tracks_Field->textField->Disable();

    URL_Field = new TextBox("Playlist URL:", ID_URL_Field,
                            wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);

    URL_Artwork_Field = new TextBox("Playlist URL with proper artwork:", ID_URL_Artwork_Field,
                                    wxPoint(mainOffset.x, mainOffset.y), TextBoxSize, mainPanel);



    wxButton* button = new wxButton(mainPanel, ID_Button, "Run script",
                                    wxPoint(mainOffset.x, mainOffset.y),
                                    ButtonSize, NULL, wxDefaultValidator, "Script Button");

    checkAlert = new wxCheckBox(mainPanel, ID_AlertOnDone, "Alert on done",
                                wxPoint(mainOffset.x + ButtonSize.x + fieldBetweenSpace.x, mainOffset.y),
                                ButtonSize, 0, wxDefaultValidator, "Alert checkbox");
    mainOffset.y += ButtonSize.y + fieldBetweenSpace.y;

    Bind(wxEVT_BUTTON, &MyFrame::OnButtonPress, this, ID_Button);

    
    Bind(wxEVT_MENU, &MyFrame::OnSave, this, ID_Save);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);


    ClientWidth = mainOffset.x + TextBoxSize.x + mainOffset.x;
    ClientHeight = mainOffset.y;

    OpenSettings();

    /*
    artist_Field->textField->SetValue("Big Black");
    albumName_Field->textField->SetValue("Lungs");
    albumYear_Field->textField->SetValue("1982");
    URL_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_lSCRmY_Qw8RCNnMKHcp05O1K8fAIyqLjs");
    URL_Artwork_Field->textField->SetValue("https://www.youtube.com/playlist?list=OLAK5uy_lSCRmY_Qw8RCNnMKHcp05O1K8fAIyqLjs");
    */

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
    wxMessageBox("This program runs a series of youtube-dl (yt-dlp) scripts to download an album and moves it into given directory.", "About", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnSave(wxCommandEvent& event)
{
    SaveSettings();
    wxMessageBox("Directories have been saved.", "Script manager", wxOK | wxICON_NONE);
}

void MyFrame::OnButtonPress(wxCommandEvent& event)
{
    URL = URL_Field->textField->GetValue().ToStdWstring();

    albumsDirectory = albumsDir_Field->textField->GetValue().ToStdWstring();
    workingDirectory = workingDir_Field->textField->GetValue().ToStdWstring();
    execName = L"yt-dlp.exe";
    configName = L"config.txt";

    artist = artist_Field->textField->GetValue().ToStdWstring();
    albumName = albumName_Field->textField->GetValue().ToStdWstring();
    albumYear = albumYear_Field->textField->GetValue().ToStdWstring();

    if (albumsDirectory.size() - 1 < 0) return;
    if (workingDirectory.size() - 1 < 0) return;

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
    artworkFilename = L"artwork.png";

    RunScript();
}

void MyFrame::RunScript()
{
    // TODO:
    // -updating not only screen but events during console process execution



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

    ResetTracksFile();
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
    ResetTracksFile();

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
    
    // DOWNLOAD ARTWORK
    GetArtwork();
    // ATTACH ARTWORK
    AllocConsole();
    for (int i = 0; i < trackTitles.size(); i++)
    {
        std::wstring trackFilepath = workingDirectory + std::to_wstring(i + 1) + L". " + artist + L" - " + trackTitles[i] + L".mp3";
        AttachArtwork(trackFilepath, artworkFilename);
    }
    FreeConsole();

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

    // MOVE AUDIO FILES
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
    // MOVE ARTWORK
    {
        AllocConsole();


        std::wstring cmd = L"cmd /c \"MOVE \"" + workingDirBackslashes + L"\\" + artworkFilename + L"\" \"" + albumPathBackslashes + L"\"\"";

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
    URL_Artwork_Field->textField->SetValue("");
    artist_Field->textField->SetValue("");
    albumName_Field->textField->SetValue("");
    albumYear_Field->textField->SetValue("");

    artist_Field->textField->SetFocus();
    
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
                if (currentWord.size() > 0)
                {
                    tracks_Field->textField->WriteText(currentWord + (wchar_t)'\n');
                    trackTitles.push_back(currentWord);
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

    const char resourceFilename[] = "index.html";
    GetResource(host.c_str(), resource.c_str(), resourceFilename);
    std::string thumbnailURL = "";
    GetThumbnailURL(&thumbnailURL, resourceFilename);
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
        args += L"-o \"" + workingDirectory + artworkFilename + "\" \"" + thumbnailURL + "\"";

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

        // Erase the resource .html file
        FILE* resourceFile;
        fopen_s(&resourceFile, resourceFilename, "w");
        if (resourceFile != nullptr) fclose(resourceFile);

        SetStatusText("Finished DOWNLOAD ARTWORK stage, exit_code: " + std::to_string(exit_code));
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);
        FreeConsole();
    }
    //*/
}







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


int GetFileData(const char* filename, std::string* returnData)
{
    std::string data = "";

    FILE* file;
    fopen_s(&file, filename, "rb");
    if (file == nullptr)
    {
        PrintConsole("Cannot open: " + std::string(filename) + '\n');
        return 1;
    }

    int currentChar;
    do
    {
        currentChar = getc(file);
        if (currentChar != EOF) data += (unsigned char)currentChar;
    } while (currentChar != EOF);
    fclose(file);

    *returnData = data;
    return 0;
}

int WriteDataToFile(std::string data, const char* filename)
{
    FILE* file;
    fopen_s(&file, filename, "wb");
    if (file == nullptr)
    {
        PrintConsole("Cannot open: " + std::string(filename) + '\n');
        return 1;
    }

    for (int i = 0; i < data.size(); i++)
    {
        putc((unsigned char)data[i], file);
    }
    fclose(file);

    return 0;
}



int FindTheTag(const char* filename, unsigned int* returnTagStartIndex = nullptr, unsigned int* returnSizeMinusHeader = nullptr,
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
        PrintConsole(HexToStr((unsigned int)tagHeader[i]));
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

int ConstructPictureFrame(const char* filename, std::string* returnFrame,
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

int ReplaceTag(const char* filename, std::string tag, const char* outputFilename = nullptr)
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

int writeArtwork(const char* audioFilename, const char* artworkFilename, const char* outputAudioFilename = nullptr)
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

        if (i == 0 || i % 16 == 0) PrintConsole(": " + HexToStr(i, 8));
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

        if (i == 0 || i % 16 == 0) PrintConsole(": " + HexToStr(i, 8));
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
        if (i == 0 || i % 16 == 0) PrintConsole(": " + HexToStr(i, 8));
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

int unsynchroTest()
{
    std::string testData, outputData;
    if (GetFileData("artwork.png", &testData)) return 1;
    UnsynchroData(testData, &outputData);
    WriteDataToFile(outputData, "artwork_output.png");


    PrintConsole("Checking outputData...\n");
    std::string query = "";
    query += (unsigned char)0xFF;
    query += (unsigned char)0x00;
    query += (unsigned char)0x00;
    printHex(outputData, query);

    return 0;
}

void MyFrame::AttachArtwork(std::wstring audioFile, std::wstring artworkFile)
{
    static unsigned int index = 1;

    artworkFile = workingDirectory + artworkFile;

    std::string audioFileStr = "";
    std::string artworkFileStr = "";
    for (int i = 0; i < audioFile.size(); i++) audioFileStr += (unsigned char)audioFile[i];
    for (int i = 0; i < artworkFile.size(); i++) artworkFileStr += (unsigned char)artworkFile[i];

    PrintConsole(std::to_string(index) + ": " + audioFileStr.c_str() + "\n");
    PrintConsole(std::to_string(index) + ": " + artworkFileStr.c_str() + "\n\n");

    writeArtwork(audioFileStr.c_str(), artworkFileStr.c_str());

    PrintConsole("---------------------------\n");
    index++;
}
