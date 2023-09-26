#include "MainFrame.h"



class MainApp : public wxApp
{
private:
public:
    virtual bool OnInit();
};



// Macro containing the main() function, invoking the entry-point of the program
wxIMPLEMENT_APP(MainApp);




bool MainApp::OnInit()
{
    DumpLicense();
    DumpReadMe();


    MainFrame* frame = new MainFrame();
    frame->StartOutputUpdate();  // starts a new thread that updates the output box

    MainDialog* aboutDialog = new MainDialog(L"About", toWide(GetAbout()));


    







    return true;
}


















