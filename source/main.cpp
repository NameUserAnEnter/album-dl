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
    //DumpLicense();
    //DumpReadMe();

    std::string enBytes;
    std::string enConsole_bytes;
    GetFileData("bytes_buf", &enBytes);
    GetFileData("console", &enConsole_bytes);

    std::wstring bytes = DecodeFromUTF8(enBytes);
    std::wstring console_bytes = DecodeFromUTF8(enConsole_bytes);

    std::wstring output;
    if (bytes == console_bytes)
    {
        output = L"true";
    }
    else
    {
        output = L"false";
    }
    output += L"\n\n";

    for (int i = 0; i < 16 && i < bytes.size(); i++)
    {
        output += bytes[i];
    }
    output += L"\n";
    for (int i = 0; i < 16 && i < console_bytes.size(); i++)
    {
        output += console_bytes[i];
    }
    MessageDialog(output);


    MainFrame* frame = new MainFrame();
    frame->StartOutputUpdate();  // starts a new thread that updates the output box





    







    return true;
}


















