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



    TextBox()
    {
        bInit = false;
        fieldEncoding = UNICODE_NONE;
        long style = 0;
    }

    void Init(std::string label, wxWindowID textFieldID, wxPoint position, wxSize size, wxPanel* panel,
            bool multiline, RECT labelOffset, wxSize& mainOffset, wxSize fieldBetweenSpace)
    {
        labelBox.Create(
            panel, wxID_ANY, toWide(label),
            wxPoint(position.x - labelOffset.left, position.y - labelOffset.top),
            wxSize(labelOffset.left + size.x + labelOffset.right, labelOffset.top + size.y + labelOffset.bottom),
            0, wxString(label + " label"));


        
        if (multiline) style = wxTE_MULTILINE;

        textField.Create(
            &labelBox, textFieldID, "",
            wxPoint(labelOffset.left, labelOffset.top),
            size,
            style, wxDefaultValidator, wxString(label + " text field"));

        mainOffset.y += size.y + fieldBetweenSpace.y;

        bInit = true;
    }



    void SetText(std::wstring text)
    {
        if (!bInit) return;

        if (fieldEncoding == TEXT_ENCODING::CP852) textField.SetValue(EncodeToCodePage(text, codepage::table_CP852));
        else textField.SetValue(text);
    }

    void PopFirstLine()
    {
        if (!bInit) return;

        std::wstring data = GetText();
        std::wstring newdata = L"";
        bool bAdd = false;
        for (int i = 0; i < data.size(); i++)
        {
            if (bAdd)
            {
                newdata += data[i];
                continue;
            }

            if (data[i] == '\n' && !bAdd) bAdd = true;
        }
        SetText(newdata);
    }

    void AddText(std::wstring text)
    {
        if (!bInit) return;

        if (fieldEncoding == TEXT_ENCODING::CP852) textField.AppendText(EncodeToCodePage(text, codepage::table_CP852));
        else textField.AppendText(text);
    }

    std::wstring GetText()
    {
        if (!bInit) return L"";

        return textField.GetValue().ToStdWstring();
    }

    int GetNumberOfLines()
    {
        if (!bInit) return -1;

        return textField.GetNumberOfLines();
    }
};

#endif


