// wxWidgets "Hello World" Program

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include <vector>

// IDs for menu items except lib-predefined
enum
{
    ID_Hello = 1,
    ID_TextBox,
    ID_OutputTextBox,
    ID_Button
};

unsigned int ClientWidth, ClientHeight;






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
    void OnButtonPress(wxCommandEvent& event);
    wxTextCtrl* textBox;
    wxTextCtrl* outputTextBox;

    STARTUPINFOA startupinfo;
    PROCESS_INFORMATION process_information;
    
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

// The main() function, invoking the entry-point of the program
wxIMPLEMENT_APP(MyApp);




bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->SetClientSize(ClientWidth, ClientHeight);
    frame->SetPosition(wxPoint(50, 700));
    frame->Show(true);
    return true;
}



MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "Main window")
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
    SetStatusText("Default status bar text.");

    // default sizes and pos because it's automatically stretched to the frame anyway
    wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2621440L, "Main Panel");

    wxSize interval_1(20, 50);
    wxSize interval_2(10, 10);

    wxSize TextBoxSize(500, 20);
    wxSize ButtonSize(100, 25);

    ClientWidth = interval_1.x + TextBoxSize.x + interval_1.x;
    ClientHeight = interval_1.y + TextBoxSize.y + interval_2.y + ButtonSize.y + interval_2.y + TextBoxSize.y + interval_1.y;
    
    textBox = new wxTextCtrl(mainPanel, ID_TextBox, "", wxPoint(interval_1.x, interval_1.y), TextBoxSize, NULL, wxDefaultValidator, "Text Box");
    outputTextBox = new wxTextCtrl(mainPanel, ID_OutputTextBox, "", wxPoint(interval_1.x, interval_1.y + TextBoxSize.y + interval_2.y + ButtonSize.y + interval_2.y), TextBoxSize, NULL, wxDefaultValidator, "Output Text Box");

    wxButton* button = new wxButton(mainPanel, ID_Button, "Run script", wxPoint(interval_1.x, interval_1.y + TextBoxSize.y + interval_2.y), ButtonSize, NULL, wxDefaultValidator, "Script Button");
    Bind(wxEVT_BUTTON, &MyFrame::OnButtonPress, this, ID_Button);

    
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    CloseHandle(startupinfo.hStdInput);
    CloseHandle(startupinfo.hStdOutput);
    CloseHandle(startupinfo.hStdError);
    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);
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
    std::string upperInput = textBox->GetValue().ToStdString();

    textBox->SetValue("");
    outputTextBox->SetValue(upperInput);

    //std::string args = "--config-location \"D:/conversion/tbdl/config.txt\" -o td8_index%(playlist_index)s.mp4 " + URL;
    //std::string args = "--config-location \"D:/conversion/tbdl/config.txt\" -o td8_index%%(playlist_index)s.mp4 " + URL;

    FreeConsole();
    AllocConsole();

    bool run = true;
    while(run)
    {
        unsigned long count = 80;
        char* buf = (char*)calloc(count, sizeof(char));

        unsigned long charsRead;
        ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buf, count, &charsRead, NULL);

        // buf[charsRead - 2] = \r (CR)
        // buf[charsRead - 1] = \n (LF)
        // printableRead = charsRead - 2
        unsigned long newSize = (charsRead - 2) + 1;
        buf = (char*)realloc(buf, newSize * sizeof(char));
        buf[newSize - 1] = '\0';

        std::string output = "";

        std::string command(buf);
        if (command == "exit")
        {
            run = false;
        }
        else
        {
            startupinfo = { };
            process_information = { };
            BOOL rv = CreateProcessA(NULL, (char*)buf, NULL, NULL, true, STARTF_USESTDHANDLES, NULL, NULL, &startupinfo, &process_information);
            if (rv != 0)
            {
                output += std::string("Success") + std::string(", ") + std::to_string(rv);
            }
            else
            {
                output += std::string("Failure") + std::string(", ") + std::to_string(rv) + ": " + std::to_string(GetLastError());
            }
        }
        output += "\n\n\0";

        unsigned long charsWritten;
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), &charsWritten, NULL);

        outputTextBox->SetValue(buf + std::string(", ") + std::to_string(newSize));
        free(buf);
    }
    FreeConsole();
}
