#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include "utils.h"



class TextBox
{
public:
    wxStaticBox* labelBox;
    wxTextCtrl* textField;

    TextBox(std::string label, wxWindowID textFieldID, wxPoint position, wxSize size, wxPanel* panel,
            bool multiline, RECT labelOffset, wxSize& mainOffset, wxSize fieldBetweenSpace)
    {
        labelBox = new wxStaticBox(
            panel, wxID_ANY, toWide(label),
            wxPoint(position.x - labelOffset.left, position.y - labelOffset.top),
            wxSize(labelOffset.left + size.x + labelOffset.right, labelOffset.top + size.y + labelOffset.bottom),
            0, wxString(label + " label"));


        long style = 0;
        if (multiline) style = wxTE_MULTILINE;
        textField = new wxTextCtrl(
            labelBox, textFieldID, "",
            wxPoint(labelOffset.left, labelOffset.top),
            size,
            style, wxDefaultValidator, wxString(label + " text field"));

        mainOffset.y += size.y + fieldBetweenSpace.y;
    }

    void SetText(std::wstring text)
    {
        textField->SetValue(text);
    }

    void AddFromStream(std::wstring& buf)
    {
        textField->AppendText(buf);
        buf.clear();
    }

    std::wstring GetText()
    {
        return textField->GetValue().ToStdWstring();
    }

    ~TextBox()
    {
        delete labelBox;
        delete textField;
    }
};

#endif


