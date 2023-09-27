#include "MainFrame.h"



class MainApp : public wxApp
{
private:
public:
    virtual bool OnInit();
};

bool MainApp::OnInit()
{
    DumpLicense();
    DumpReadMe();


    MainFrame* frame = new MainFrame();
    frame->StartOutputUpdate();  // starts a new thread that updates the output box


    std::wstring testText = L"";
    for (int i = 0; i < 50; i++)
    {
        testText += NumToWstr(i + 1, 10, 2, ' ') + L": abcdefghijklmnopqrstuvwxyz, abcdefghijklmnopqrstuvwxyz - abcdefghijklmnopqrstuvwxyz\n";
    }
    
    MainDialog* testDialog3 = new MainDialog(L"testDialog", testText, true);







    return true;
}

// Macro containing the main() function, invoking the entry-point of the program
wxIMPLEMENT_APP(MainApp);





