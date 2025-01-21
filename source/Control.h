#ifndef CONTROL_H
#define CONTROL_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


#include "encoding.h"
#include "utils.h"
#include "Rectang.h"







class Control
{
protected:
    bool bInit;
    bool bAcceptReturn;

    RECT labelOffset;






    wxSize minSize;
    wxSize maxSize;

public:
    wxStaticBox labelBox;
    wxControl* control;
    Control(wxControl*);

    Rectang rectang;

    void Init(wxWindowID, wxSize, wxWindow*, long = NULL);

protected:
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

    void Disable();
    void Enable();

    void SetLabel(std::wstring);
    void AppendLabel(std::wstring, bool = true);
    std::wstring GetLabel();
};



#endif
