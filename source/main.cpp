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



    wxFont outputFont;
    std::wstring currentFaceName;
    std::wstring testStr = L":\n";
    testStr += L"ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
    testStr += L"abcdefghijklmnopqrstuvwxyz\n\n";

    testStr += L"• ”åèØ∆—£\n";
    testStr += L"πÍÛúüøÊÒ≥\n\n";

    testStr += L"[0x0080 - 0x017F] CODE POINTS:\n";
    testStr += L"    ";
    testStr += L" |";
    for (int i = 0; i < 16; i++) testStr += ' ' + NumToWstr(i, 16, 2);
    testStr += '\n';

    for (wchar_t codePoint = 0x0080; codePoint <= 0x017F; codePoint++)
    {
        if (codePoint % 16 == 0) testStr += NumToWstr(codePoint, 16, 4, ' ') + L" | ";

        if ((codePoint >= 0x80 && codePoint <= 0xA0) || codePoint == 0xAD) testStr += ' ';
        else testStr += codePoint;

        if ((codePoint + 1) % 16 == 0) testStr += '\n';
        else testStr += L"  ";
    }
    if (testStr.back() != '\n') testStr += '\n';




    //--//
    outputFont = wxFont(wxFontInfo(wxSize(8, 16)).FaceName("Courier New").Bold());
    frame->fOutput.SetFont(outputFont);
    frame->fOutput.SetEncoding(TEXT_ENCODING::UNICODE_NONE);

    currentFaceName = frame->fOutput.GetFont().GetFaceName().ToStdWstring();

    frame->mainConsole.PrintLogAndConsole(currentFaceName + testStr);
    frame->mainConsole.PrintLogAndConsoleNarrow("-----------------------------------------------------------------------------\n");







    return true;
}


















