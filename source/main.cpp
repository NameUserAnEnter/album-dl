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


    //wxFontInfo testFont = wxFontInfo(wxSize(7, 15)).FaceName("Courier New").Bold();

    //MainDialog* testDialog1 = new MainDialog(L"About",      toWide(GetAbout()),     ID_dialogTest1, testFont, true);
    //MainDialog* testDialog2 = new MainDialog(L"License",    toWide(GetLicense()),   ID_dialogTest2, testFont, true,
    //                                         wxPoint(testDialog1->GetPosition().x + testDialog1->GetSize().x, 0));
    //MainDialog* testDialog3 = new MainDialog(L"License",    toWide(GetReadMe()),    ID_dialogTest3, testFont, true,
    //                                         wxPoint(testDialog2->GetPosition().x + testDialog2->GetSize().x, 0));







    return true;
}


















