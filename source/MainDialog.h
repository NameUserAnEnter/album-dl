#ifndef MAIN_DIALOG_H
#define MAIN_DIALOG_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "utils.h"

namespace MainDialogDefaults
{
    static const wxFontInfo defaultFont = wxFontInfo(10).FaceName("Consolas");
    static const unsigned int maxLines = 30;
}


class MainDialog : public wxDialog
{
private:
    wxPanel panel;
    wxButton buttonOk;
    wxStaticText textBox;
    wxScrollBar scroll;

    wxSize buttonSize;
    wxSize scrollSize;
    wxSize textSize;
    Margin buttonMargin;
    Margin scrollMargin;
    Margin textMargin;

    std::vector<std::wstring> lines;
    bool bScrollable;
private:
    std::vector<std::wstring> GetVisibleLines(int first, int last)
    {
        std::vector<std::wstring> visibleLines;
        for (int i = first; i <= last && i < lines.size(); i++)
        {
            visibleLines.push_back(lines[i]);
        }
        return visibleLines;
    }

    std::wstring GetCurrentText(std::vector<std::wstring> visibleLines)
    {
        std::wstring currentText = L"";
        for (int i = 0; i < visibleLines.size(); i++)
        {
            currentText += visibleLines[i];
            if (i != visibleLines.size() - 1) currentText += L'\n';
        }
        return currentText;
    }


    void InitButton()
    {
        buttonOk.Create(&panel, wxID_ANY, L"OK");

        buttonMargin = { 5, 0, 15, 0 };
        buttonSize = wxSize(100, 25);
        buttonOk.SetSize(buttonSize);
    }

    void InitPanel()
    {
        panel.Create(this, wxID_ANY);
    }

    void InitTextBox(wxFontInfo requestedFont)
    {
        using namespace MainDialogDefaults;
        textBox.Create(&panel, wxID_ANY, L"");

        // The main idea behind this class is to display dialogs with custom fonts
        // This enables to display dialogs with information that has to be aligned with a monospaced font

        textBox.SetFont(wxFont(requestedFont));                                  // Set requested font
        if (textBox.GetFont().GetFaceName() != requestedFont.GetFaceName())      // Set a default monospace font, in case of failure to set the requested font
        {
            textBox.SetFont(defaultFont);
        }

        textBox.SetPosition(wxPoint(textMargin.left, textMargin.top));
        textBox.SetMinSize(wxSize(1, 1));
        textBox.SetSize(1, 1);

        if (lines.size() <= maxLines)
        {
            // Set width according to the widest line in the text
            // Set height according to number of lines in the text
            textBox.SetLabel(GetCurrentText(lines));
            textSize = textBox.GetSize();
            bScrollable = false;
        }
        else
        {
            // Set width according to the widest line in the text
            textBox.SetLabel(GetCurrentText(lines));
            textSize.x = textBox.GetSize().x;
            
            // Set height according to max number of lines
            textBox.SetLabel(GetCurrentText(GetVisibleLines(0, maxLines - 1)));
            textSize.y = textBox.GetSize().y;
            bScrollable = true;
        }
    }
    
    void InitScrollBar()
    {
        if (!bScrollable) return;

        using namespace MainDialogDefaults;
        scroll.Create(&panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);

        scrollSize = wxSize(scroll.GetSize().x, textSize.y);
        scroll.SetSize(scrollSize);
        scrollMargin = { 0, 0, 0, 5 };
        scroll.SetPosition(wxPoint(textMargin.left + textSize.x + textMargin.right + scrollMargin.left, textMargin.top));

        // SetScrollbar(firstPos, thumb, range, ...)
        // lastPos = range - thumb
        
        // firstPos, thumb, range
        // lastPos = range - thumb
        //
        // args: 0, 30, 80
        // lastPos: 80 - 30 = 50
        // pos belongs to: 0 to 50
        // pos = 0: 0, 1, 2, 3, ..., 29
        // pos = 50: 50, 51, 52, 53, 53, ..., 79
        //
        // args: 0, 30, 79
        // lastPos: 79 - 30 = 49
        // pos belonds to: 0 to 49
        // pos = 0: 0, 1, 2, 3, ..., 29
        // pos = 49: 49, 50, 51, 52, ... 78
        
        // conclusion: contrary to wxWidgets docs, range parameter is not the last position, hence all the confusion

        scroll.SetScrollbar(0, maxLines, lines.size(), maxLines - 1, true);
    }

    void InitDialogSize()
    {
        wxSize clientSize;
        if (bScrollable) clientSize.x = textMargin.left + textSize.x + textMargin.right + scrollMargin.left + scrollSize.x + scrollMargin.right;
        else             clientSize.x = textMargin.left + textSize.x + textMargin.right;

        clientSize.y = textMargin.top + textSize.y + textMargin.bottom + buttonMargin.top + buttonSize.y + buttonMargin.bottom;
        SetClientSize(clientSize);
    }

    void InitBindings()
    {
        Bind(wxEVT_BUTTON, &MainDialog::OnButton, this, buttonOk.GetId());
        Bind(wxEVT_SIZE, &MainDialog::OnSize, this, GetId());

        if (bScrollable)
        {
            Bind(wxEVT_SCROLL_TOP, &MainDialog::OnScroll, this);
            Bind(wxEVT_SCROLL_BOTTOM, &MainDialog::OnScroll, this);
            Bind(wxEVT_SCROLL_LINEUP, &MainDialog::OnScroll, this);
            Bind(wxEVT_SCROLL_LINEDOWN, &MainDialog::OnScroll, this);
            Bind(wxEVT_SCROLL_PAGEUP, &MainDialog::OnScroll, this);
            Bind(wxEVT_SCROLL_PAGEDOWN, &MainDialog::OnScroll, this);
            Bind(wxEVT_SCROLL_THUMBTRACK, &MainDialog::OnScroll, this);
            Bind(wxEVT_SCROLL_THUMBRELEASE, &MainDialog::OnScroll, this);
            Bind(wxEVT_SCROLL_CHANGED, &MainDialog::OnScroll, this);
        }
    }
public:
    MainDialog(std::wstring title, std::wstring text,

               bool bShow = false,
               wxPoint position = wxPoint(0, 0),
               wxFontInfo requestedFont = MainDialogDefaults::defaultFont,
               Margin requestedTextMargin = { 30, 30, 5, 30 })
        : wxDialog(NULL, wxID_ANY, title, position, wxDefaultSize, wxCAPTION | wxCLOSE_BOX)
    {
        lines = splitByNewlines(trimTrailingNewlines(text), false);
        textMargin = requestedTextMargin;

        InitPanel();
        InitButton();
        InitTextBox(requestedFont);
        InitScrollBar();
        InitDialogSize();
        InitBindings();

        // Dialog is ready to be shown.
        Show(bShow);
    }
private:
    void OnButton(wxCommandEvent& event)
    {
        Close();
    }

    void OnSize(wxSizeEvent& event)
    {
        buttonOk.SetPosition(wxPoint(GetClientSize().x / 2 - buttonSize.x / 2, GetClientSize().y - buttonMargin.bottom - buttonSize.y));

        event.Skip();
    }


    void OnScroll(wxScrollEvent& event)
    {
        using namespace MainDialogDefaults;
        
        int newPosition = event.GetPosition();

        if (newPosition > lines.size() - 1) newPosition = lines.size() - 1;
        if (newPosition < 0) newPosition = 0;

        scroll.SetScrollbar(newPosition, maxLines, lines.size(), maxLines - 1, true);

        textBox.SetLabel(GetCurrentText(GetVisibleLines(newPosition, newPosition + maxLines - 1)));
    }
};
#endif

