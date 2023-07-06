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

    static MainFrame* frame = new MainFrame();
    frame->StartOutputUpdate();  // starts a new thread that updates the output box
    //std::this_thread::sleep_for(std::chrono::seconds(5));



    std::wstring tableDiff = GetTableDiff(codepage::table_CP852, codepage::table_CP1250);
    std::wstring output = L"";
    for (int i = 0; i < tableDiff.size(); i++)
    {
        output += toWide(NumToStr(i + 1)) + L": " + tableDiff[i] + L"\n";
    }
    if (!output.empty()) output += L"\n\n";
    //fOutput.SetText(output);
    frame->mainConsole.PrintLogAndConsole(output);




    return true;
}


















