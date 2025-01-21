#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include "encoding.h"
#include "utils.h"
#include "Rectang.h"

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
public:
    wxStaticBox labelBox;
    wxTextCtrl textField;
    TextBox();

    Rectang rectang;

    void Init(wxWindowID, wxSize, wxWindow*, long = NULL);

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
    wxSize ComputeLabelBoxSize(wxSize);
    wxPoint ComputeLabelBoxPos(wxPoint);

    wxSize ComputeTextBoxSize(wxSize);
    wxPoint ComputeTextBoxPos(wxPoint);

    void UpdateRectang();

    void OnLabelBoxMove(wxMoveEvent&);
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

    wxSize GetDistance();


    void Show();
    void Hide();



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

    void SetLabel(std::wstring);
    void AppendLabel(std::wstring, bool = true);
    std::wstring  GetLabel();

    void SetHint(std::wstring);
    std::wstring GetHint();
};



#endif


