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
    bool bAcceptReturn;

    RECT labelOffset;

    wxStaticBox labelBox;

    wxTextCtrl textField;
    TEXT_ENCODING fieldEncoding;

    std::mutex* bufMutex;
    std::wstring outputBuf;

    std::wstring FormatText(std::wstring);
public:
    TextBox();

    void Init(std::string, wxWindowID, wxPoint, wxSize, wxPanel*, long = NULL);

    std::wstring* GetBuf();
    void SetMutex(std::mutex*);
    void FlushBuf();

    void SetText(std::wstring);
    void SetTextNarrow(std::string);

    void AddText(std::wstring);
    void AddTextNarrow(std::string);

    std::wstring GetContent();
    long GetCursorPos();

    void SetFocus();
    void SetEditable(bool);

    void SetForeground(wxColour);
    void SetBackground(wxColour);

    void SetFont(wxFont);
    wxFont GetFont();
    std::wstring GetFontFaceName();

    void SetEncoding(TEXT_ENCODING);


    bool IsEmpty();

    void RemoveExceeding(unsigned int uMaxLines);

    std::wstring GetText();

    int GetNumberOfLines();

    void Disable();
    void Enable();
};



#endif


