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
    //std::this_thread::sleep_for(std::chrono::seconds(5));




    frame->mainConsole.PrintLogAndConsole(printTable(codepage::table_CP852));
    frame->mainConsole.PrintLogAndConsole(printTable(codepage::table_CP437));
    frame->mainConsole.PrintLogAndConsole(printTableDiff(codepage::table_CP852, codepage::table_CP437));





    return true;
}


















