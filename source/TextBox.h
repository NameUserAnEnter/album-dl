#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include "encoding.h"
#include "utils.h"
#include "Control.h"

enum TEXT_ENCODING
{
    UNICODE_NONE,
    CP852
};

class TextBox : public Control
{
private:
    bool bAcceptReturn;

    std::mutex* bufMutex;
    std::wstring outputBuf;
    
    TEXT_ENCODING fieldEncoding;

public:
    wxTextCtrl textField;
    TextBox();

    void Init(wxWindowID, wxSize, wxWindow*, long = NULL);

public:
    void SetFont(wxFont);
    wxFont GetFont();
    std::wstring GetFontFaceName();

    std::wstring FormatText(std::wstring);
    void SetEncoding(TEXT_ENCODING);

    bool IsEmpty();
    int GetNumberOfLines();
    void RemoveExceeding(unsigned int uMaxLines);
    std::wstring GetText();

    void SetHint(std::wstring);
    std::wstring GetHint();

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
};



#endif


