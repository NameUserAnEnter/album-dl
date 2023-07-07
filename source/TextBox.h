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

    wxStaticBox labelBox;
    wxTextCtrl textField;

    TEXT_ENCODING fieldEncoding;

    std::mutex* bufMutex;
    std::wstring outputBuf;
public:
    TextBox();

    void Init(std::string, wxWindowID, wxPoint, wxSize, wxPanel*, long, RECT, wxSize&, wxSize);

    std::wstring* GetBuf();
    void SetMutex(std::mutex*);
    void FlushBuf();

    void SetText(std::wstring);
    void SetTextNarrow(std::string);

    void AddText(std::wstring);
    void AddTextNarrow(std::string);

    void SetFocus();
    void SetEditable(bool);

    void SetForeground(wxColour);
    void SetBackground(wxColour);

    void SetFont(wxFont);
    wxFont GetFont();
    void SetEncoding(TEXT_ENCODING);


    bool IsEmpty();

    void PopFirstLine();

    std::wstring GetText();

    int GetNumberOfLines();
};



#endif


