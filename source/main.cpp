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


    std::string testText = "";
    testText += GetLicense() + "\n" "-----------------------------------------------------------------------";
    testText += "\n\n" + GetLicense();// + "\n" "-----------------------------------------------------------------------";
    //testText += "\n\n" + GetLicense() + "\n" "-----------------------------------------------------------------------";
    std::vector<std::string> lines = splitByNewlines(testText, true);
    testText = "";
    for (int i = 0; i < lines.size(); i++)
    {
        testText += NumToStr(i + 1, 10, 3, ' ') + L": " + lines[i];
    }


    MainDialog* testDialog3 = new MainDialog(L"testDialog", toWide(testText), true);







    return true;
}

// Macro containing the main() function, invoking the entry-point of the program
wxIMPLEMENT_APP(MainApp);





