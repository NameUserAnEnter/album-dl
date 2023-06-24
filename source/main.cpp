#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif








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
    MainFrame* frame = new MainFrame();

    frame->labelOffset.left = 3;
    frame->labelOffset.right = 3;
    frame->labelOffset.top = 15;
    frame->labelOffset.bottom = 3;


    frame->SetClientSize(frame->ClientWidth, frame->ClientHeight);
    frame->SetPosition(wxPoint(1020, 20));
    frame->OpenSettings();
    frame->Show(true);
    return true;
}


















