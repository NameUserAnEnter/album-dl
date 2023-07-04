#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include "encoding.h"
#include "utils.h"

enum TEXT_ENCODING
{
    UNICODE_NONE,
    CP852
};

class TextBox
{
public:
    wxStaticBox labelBox;
    wxTextCtrl textField;

    TEXT_ENCODING fieldEncoding;
    long style;
    bool bInit;


    TextBox();

    void Init(std::string label, wxWindowID textFieldID, wxPoint position, wxSize size, wxPanel* panel,
                       bool multiline, RECT labelOffset, wxSize& mainOffset, wxSize fieldBetweenSpace);

    void SetText(std::wstring text);

    void PopFirstLine();

    void AddText(std::wstring text);

    std::wstring GetText();

    int GetNumberOfLines();
};

#endif


