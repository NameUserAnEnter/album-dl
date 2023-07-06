#include "TextBox.h"

TextBox::TextBox()
{
    bInit = false;
    fieldEncoding = UNICODE_NONE;
    long style = 0;

    bufMutex = nullptr;
    outputBuf.clear();
}

void TextBox::Init(std::string label, wxWindowID textFieldID, wxPoint position, wxSize size, wxPanel* panel,
          long style, RECT labelOffset, wxSize& mainOffset, wxSize fieldBetweenSpace)
{
    labelBox.Create(
        panel, wxID_ANY, toWide(label),
        wxPoint(position.x - labelOffset.left, position.y - labelOffset.top),
        wxSize(labelOffset.left + size.x + labelOffset.right, labelOffset.top + size.y + labelOffset.bottom),
        0, wxString(label + " label"));



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

void TextBox::SetTextNarrow(std::string text)
{
    SetText(toWide(text));
}

void TextBox::AddTextNarrow(std::string text)
{
    AddText(toWide(text));
}




std::wstring* TextBox::GetBuf()
{
    return &outputBuf;
}

void TextBox::SetMutex(std::mutex* existingMutex)
{
    bufMutex = existingMutex;
}

void TextBox::FlushBuf()
{
    if (bufMutex == nullptr) return;
    if (outputBuf.empty()) return;
    std::lock_guard<std::mutex> bufLock(*bufMutex);

    AddText(outputBuf);
    outputBuf.clear();
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










