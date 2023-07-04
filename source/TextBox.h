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
private:
    bool bInit;
    long style;

    wxStaticBox labelBox;
    wxTextCtrl textField;

    TEXT_ENCODING fieldEncoding;
public:
    TextBox();

    void Init(std::string, wxWindowID, wxPoint, wxSize, wxPanel*, bool, RECT, wxSize&, wxSize);

    void SetText(std::wstring);
    void SetText(std::string);

    void AddText(std::wstring);
    void AddText(std::string);

    void SetFocus();
    void SetEditable(bool);

    void SetForeground(wxColour);
    void SetBackground(wxColour);

    void SetFont(wxFont);
    void SetEncoding(TEXT_ENCODING);


    bool IsEmpty();

    void PopFirstLine();

    std::wstring GetText();

    int GetNumberOfLines();
};

#endif


