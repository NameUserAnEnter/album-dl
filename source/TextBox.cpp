#include "TextBox.h"

TextBox::TextBox() : Control(&textField)
{
    bAcceptReturn = false;
    fieldEncoding = UNICODE_NONE;


    bufMutex = nullptr;
    outputBuf.clear();
}

void TextBox::Init(wxWindowID id, wxSize size, wxWindow* parent, long style)
{
    Control::Init(id, size, parent, style);

    textField.Create(
        &labelBox, id, L"",
        wxPoint(labelOffset.left, labelOffset.top),
        size,
        style);



    bInit = true;
    UpdateRectang();
}



void TextBox::SetFont(wxFont font)
{
    if (!bInit) return;
    textField.SetFont(font);
}

wxFont TextBox::GetFont()
{
    return textField.GetFont();
}

std::wstring TextBox::GetFontFaceName()
{
    return GetFont().GetFaceName().ToStdWstring();
}



std::wstring TextBox::FormatText(std::wstring text)
{
    std::wstring output;

    if (fieldEncoding == TEXT_ENCODING::CP852) output = toWide(EncodeToCodePage(text, codepage::table_CP852));
    else output = text;

    return output;
}

void TextBox::SetEncoding(TEXT_ENCODING _fieldEncoding)
{
    fieldEncoding = _fieldEncoding;
}



bool TextBox::IsEmpty()
{
    if (!bInit) return true;
    return textField.IsEmpty();
}

int TextBox::GetNumberOfLines()
{
    if (!bInit) return -1;

    return textField.GetNumberOfLines();
}

void TextBox::RemoveExceeding(unsigned int uMaxLines)
{
    if (!bInit) return;

    std::wstring data = GetText();
    std::wstring newdata = L"";
    
    bool bAdd = false;
    unsigned int uCurrentLine = 1;
    int uLastToRemove = GetNumberOfLines() - uMaxLines;

    //MessageDialog(std::to_string(uLastToRemove));
    if (uLastToRemove <= 0) return;

    for (int i = 0; i < data.size(); i++)
    {
        if (uCurrentLine > uLastToRemove) bAdd = true;

        if (bAdd)
        {
            newdata += data[i];
            continue;
        }

        if (data[i] == '\n') uCurrentLine++;
    }


    SetText(newdata);
}

std::wstring TextBox::GetText()
{
    if (!bInit) return L"";

    return textField.GetValue().ToStdWstring();
}



void TextBox::SetHint(std::wstring hint)
{
    if (!bInit) return;

    textField.SetHint(hint);
}

std::wstring TextBox::GetHint()
{
    if (!bInit) return L"";

    return textField.GetHint().ToStdWstring();
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



void TextBox::SetText(std::wstring text)
{
    if (!bInit) return;

    textField.SetValue("");
    AddText(text);
}

void TextBox::SetTextNarrow(std::string text)
{
    SetText(toWide(text));
}


void TextBox::AddText(std::wstring text)
{
    if (!bInit) return;

    std::wstring output = FormatText(text);


    if (bAcceptReturn)
    {
        // use only GetInsertionPoint() to assign startOfLastLine value
        static long startOfLastLine = 0;

        std::vector<std::wstring> lines = splitByNewlines(text);
        for (int i = 0; i < lines.size(); i++)
        {
            std::wstring currentLine = lines[i];
            if (lines[i].back() == L'\n') currentLine.pop_back();

            std::vector<std::wstring> lineFrags = splitByChar(currentLine, L'\r', false);
            for (int j = 0; j < lineFrags.size(); j++)
            {
                // not TextBox::Replace()
                textField.Replace(startOfLastLine, startOfLastLine + lineFrags[j].size(), lineFrags[j]);
            }

            if (lines[i].back() == L'\n')
            {
                textField.AppendText("\n");
                startOfLastLine = textField.GetInsertionPoint();
            }
        }
    }
    else textField.AppendText(output);
    //textField.AppendText(output);

    textField.SetInsertionPointEnd();
}

void TextBox::AddTextNarrow(std::string text)
{
    AddText(toWide(text));
}



std::wstring TextBox::GetContent()
{
    if (bufMutex == nullptr) return L"";
    return (std::wstring)textField.GetValue();
}

long TextBox::GetCursorPos()
{
    if (bufMutex == nullptr) return -1;
    return textField.GetInsertionPoint();
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

