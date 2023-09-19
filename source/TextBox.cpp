#include "TextBox.h"

TextBox::TextBox()
{
    bInit = false;
    bAcceptReturn = true;
    fieldEncoding = UNICODE_NONE;

    bufMutex = nullptr;
    outputBuf.clear();

    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;
}

void TextBox::Init(std::string label, wxWindowID textFieldID, wxPoint position, wxSize size, wxPanel* panel, long style)
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

    bInit = true;
}






void TextBox::SetText(std::wstring text)
{
    if (!bInit) return;

    textField.SetValue("");
    AddText(text);
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
            std::vector<std::wstring> lineFrags = splitByChar(lines[i], L'\r', false);
            for (int j = 0; j < lineFrags.size(); j++)
            {
                if (j == 0) textField.AppendText(lineFrags[j]);
                if (j > 0)
                {
                    textField.Replace(startOfLastLine, startOfLastLine + lineFrags[j].size() - 1, lineFrags[j]);
                }
            }
            //textField.AppendText("\n");
            startOfLastLine = textField.GetInsertionPoint();
        }
    }
    else textField.AppendText(output);
    //textField.AppendText(output);

    textField.SetInsertionPointEnd();
}


std::wstring TextBox::FormatText(std::wstring text)
{
    std::wstring output;

    if (fieldEncoding == TEXT_ENCODING::CP852) output = toWide(EncodeToCodePage(text, codepage::table_CP852));
    else output = text;

    return output;
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

void TextBox::Replace(long startIndex, long endIndex, std::wstring with)
{
    if (bufMutex == nullptr) return;

    std::lock_guard<std::mutex> bufLock(*bufMutex);
    textField.Replace(startIndex, endIndex + 1, with);
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

wxFont TextBox::GetFont()
{
    return textField.GetFont();
}

std::wstring TextBox::GetFontFaceName()
{
    return GetFont().GetFaceName().ToStdWstring();
}



void TextBox::SetEncoding(TEXT_ENCODING _fieldEncoding)
{
    fieldEncoding = _fieldEncoding;
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





void TextBox::Disable()
{
    textField.Disable();
}

void TextBox::Enable()
{
    textField.Enable();
}










