#include "TextBox.h"

TextBox::TextBox()
{
    bInit = false;
    fieldEncoding = UNICODE_NONE;
    long style = 0;
}

void TextBox::Init(std::string label, wxWindowID textFieldID, wxPoint position, wxSize size, wxPanel* panel,
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






void TextBox::SetText(std::wstring text)
{
    if (!bInit) return;

    if (fieldEncoding == TEXT_ENCODING::CP852) textField.SetValue(EncodeToCodePage(text, codepage::table_CP852));
    else textField.SetValue(text);
}

void TextBox::AddText(std::wstring text)
{
    if (!bInit) return;

    if (fieldEncoding == TEXT_ENCODING::CP852) textField.AppendText(EncodeToCodePage(text, codepage::table_CP852));
    else textField.AppendText(text);
}

void TextBox::SetText(std::string text)
{
    SetText(toWide(text));
}

void TextBox::AddText(std::string text)
{
    AddText(toWide(text));
}






void TextBox::SetFocus()
{
    if (!bInit) return;
    textField.SetFocus();
}

void TextBox::SetEditable(bool bEditable)
{
    if (!bInit) return;
    textField.SetEditable(bEditable);
}



void TextBox::SetForeground(wxColour foreground)
{
    if (!bInit) return;
    textField.SetForegroundColour(wxColour(foreground));
}

void TextBox::SetBackground(wxColour background)
{
    if (!bInit) return;
    textField.SetBackgroundColour(wxColour(background));
}

void TextBox::SetFont(wxFont font)
{
    if (!bInit) return;
    textField.SetFont(font);
}

void TextBox::SetEncoding(TEXT_ENCODING _fieldEncoding)
{
    fieldEncoding = _fieldEncoding;
}





void TextBox::PopFirstLine()
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




bool TextBox::IsEmpty()
{
    if (!bInit) return true;
    return textField.IsEmpty();
}





std::wstring TextBox::GetText()
{
    if (!bInit) return L"";

    return textField.GetValue().ToStdWstring();
}

int TextBox::GetNumberOfLines()
{
    if (!bInit) return -1;

    return textField.GetNumberOfLines();
}





TextBoxLocked::TextBoxLocked()
{
    pTextMutex = nullptr;
}

void TextBoxLocked::Init(std::mutex* _pTextMutex, std::string label, wxWindowID textFieldID, wxPoint position, wxSize size, wxPanel* panel,
                         bool multiline, RECT labelOffset, wxSize& mainOffset, wxSize fieldBetweenSpace)
{
    pTextMutex = _pTextMutex;
    internal.Init(label, textFieldID, position, size, panel, multiline, labelOffset, mainOffset, fieldBetweenSpace);
}



void TextBoxLocked::SetText(std::wstring text)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.SetText(text);
    }
}

void TextBoxLocked::SetText(std::string text)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.SetText(text);
    }
}

void TextBoxLocked::AddText(std::wstring text)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.AddText(text);
    }
}

void TextBoxLocked::AddText(std::string text)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.AddText(text);
    }
}



void TextBoxLocked::AddTextClear(std::wstring& text)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.AddText(text);

        text.clear();
    }
}

void TextBoxLocked::AddTextClear(std::string& text)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.AddText(text);

        text.clear();
    }
}






void TextBoxLocked::SetFocus()
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.SetFocus();
    }
}

void TextBoxLocked::SetEditable(bool bEditable)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.SetEditable(bEditable);
    }
}



void TextBoxLocked::SetForeground(wxColour fg)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.SetForeground(fg);
    }
}

void TextBoxLocked::SetBackground(wxColour bg)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.SetBackground(bg);
    }
}

void TextBoxLocked::SetFont(wxFont font)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.SetFont(font);
    }
}

void TextBoxLocked::SetEncoding(TEXT_ENCODING encoding)
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.SetEncoding(encoding);
    }
}





void TextBoxLocked::PopFirstLine()
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        internal.PopFirstLine();
    }
}




bool TextBoxLocked::IsEmpty()
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        return internal.IsEmpty();
    }
    else return true;
}

std::wstring TextBoxLocked::GetText()
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        return internal.GetText();
    }
    else return L"";
}

int TextBoxLocked::GetNumberOfLines()
{
    if (pTextMutex != nullptr)
    {
        std::lock_guard<std::mutex> textLock(*pTextMutex);
        return internal.GetNumberOfLines();
    }
    else return -1;
}




