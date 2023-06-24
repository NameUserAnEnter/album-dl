#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <thread>



// ID's for GUI-elements
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


// GLOBAL VARIABLES
unsigned int ClientWidth, ClientHeight;

wxSize mainOffset(20, 40);
wxSize fieldBetweenSpace(10, 20);

wxSize TextBoxSize(500, 20);
wxSize LargeBoxSize(500, 500);
wxSize ButtonSize(100, 25);

RECT labelOffset;



std::thread consoleThread;




#include "utils.h"
#include "TextBox.h"
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
    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;

    MainFrame* frame = new MainFrame();
    frame->SetClientSize(ClientWidth, ClientHeight);
    frame->SetPosition(wxPoint(1020, 20));
    frame->OpenSettings();
    frame->Show(true);
    return true;
}


















