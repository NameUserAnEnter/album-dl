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


    MainDialog* testDialog3 = new MainDialog(L"Read me", toWide(GetReadMe()), true);







    return true;
}

// Macro containing the main() function, invoking the entry-point of the program
wxIMPLEMENT_APP(MainApp);





