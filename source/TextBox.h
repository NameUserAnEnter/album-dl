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

    std::mutex* bufMutex;
    std::wstring outputBuf;
    
    TEXT_ENCODING fieldEncoding;
    std::wstring FormatText(std::wstring);

    wxSize minSize;
    wxSize maxSize;

    wxStaticBox labelBox;
    wxTextCtrl textField;
public:
    TextBox();

    void Init(std::string, wxWindowID, wxPoint, wxSize, wxWindow*, long = NULL);

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

private:
    wxSize ComputeLabelSize(wxSize);
    wxPoint ComputeLabelPos(wxPoint);
public:
    void SetPosition(int, int);
    void SetPosition(wxPoint);

    wxPoint GetPosition();

    void SetSize(int, int);
    void SetMinSize(int, int);
    void SetMaxSize(int, int);

    void SetSize(wxSize);
    void SetMinSize(wxSize);
    void SetMaxSize(wxSize);

    wxSize GetSize();
    wxSize GetMinSize();
    wxSize GetMaxSize();



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


