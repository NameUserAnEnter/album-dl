#include "Control.h"

Control::Control(wxControl* ctrl)
{
    bInit = false;

    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;



    control = ctrl;



    minSize.x = 0;
    minSize.y = 0;

    maxSize.x = -1;
    maxSize.y = -1;
}

void Control::Init(wxWindowID id, wxSize size, wxWindow* parent, long style)
{
    std::wstring label = L"";

    labelBox.Create(
        parent, wxID_ANY, label, wxDefaultPosition, ComputeLabelBoxSize(size));


    labelBox.Bind(wxEVT_MOVE, &Control::OnLabelBoxMove, this, labelBox.GetId());
}



wxSize Control::ComputeLabelBoxSize(wxSize textBoxSize)
{
    return wxSize(labelOffset.left + textBoxSize.x + labelOffset.right, labelOffset.top + textBoxSize.y + labelOffset.bottom);
}

wxPoint Control::ComputeLabelBoxPos(wxPoint textBoxPos)
{
    return wxPoint(textBoxPos.x - labelOffset.left, textBoxPos.y - labelOffset.top);
}

wxSize Control::ComputeTextBoxSize(wxSize labelBoxSize)
{
    return wxSize(labelBoxSize.x - (labelOffset.left + labelOffset.right), labelBoxSize.y - (labelOffset.top + labelOffset.bottom));
}

wxPoint Control::ComputeTextBoxPos(wxPoint labelBoxPos)
{
    return wxPoint(labelBoxPos.x + labelOffset.left, labelBoxPos.y + labelOffset.top);
}

void Control::OnLabelBoxMove(wxMoveEvent& event)
{
    control->SetPosition(wxPoint(labelOffset.left, labelOffset.top));
}

void Control::SetLabel(std::wstring label)
{
    if (!bInit) return;
    labelBox.SetLabel(label);
}

void Control::AppendLabel(std::wstring suffix, bool popLast)
{
    if (!bInit) return;

    std::wstring label = GetLabel();
    if (popLast && label.size() > 0) label.pop_back();

    labelBox.SetLabel(label + suffix);
}

std::wstring Control::GetLabel()
{
    if (!bInit) return L"";
    return labelBox.GetLabel().ToStdWstring();
}



void Control::UpdateRectang()
{
    wxPoint pos = GetPosition();
    wxSize size = GetSize();
    rectang = Rectang(pos.x, pos.y, size.x, size.y);
}

void Control::SetPosition(int x, int y)
{
    SetPosition(wxPoint(x, y));
}

void Control::SetPosition(wxPoint newPos)
{
    if (!bInit) return;

    labelBox.SetPosition(ComputeLabelBoxPos(newPos));
    UpdateRectang();
}

wxPoint Control::GetPosition()
{
    if (!bInit) return wxPoint(-1, -1);

    return ComputeTextBoxPos(labelBox.GetPosition());
}

void Control::SetSize(int width, int height)
{
    SetSize(wxSize(width, height));
}

void Control::SetMinSize(int width, int height)
{
    minSize.x = width;
    minSize.y = height;
}

void Control::SetMaxSize(int width, int height)
{
    maxSize.x = width;
    maxSize.y = height;
}

void Control::SetSize(wxSize requestedSize)
{
    if (!bInit) return;
    wxSize newSize = control->GetSize();

    if (requestedSize.x >= minSize.x && (requestedSize.x <= maxSize.x || maxSize.x == -1))
    {
        newSize.x = requestedSize.x;
    }
    else if (requestedSize.x < minSize.x) newSize.x = minSize.x;
    else if (requestedSize.x > maxSize.x && maxSize.x != -1) newSize.x = maxSize.x;


    if (requestedSize.y >= minSize.y && (requestedSize.y <= maxSize.y || maxSize.y == -1))
    {
        newSize.y = requestedSize.y;
    }
    else if (requestedSize.y < minSize.y) newSize.y = minSize.y;
    else if (requestedSize.y > maxSize.y && maxSize.y != -1) newSize.y = maxSize.y;



    labelBox.SetSize(ComputeLabelBoxSize(newSize));
    control->SetSize(newSize);

    UpdateRectang();
}

void Control::SetMinSize(wxSize newMinSize)
{
    minSize = newMinSize;
}

void Control::SetMaxSize(wxSize newMaxSize)
{
    maxSize = newMaxSize;
}

wxSize Control::GetSize()
{
    if (!bInit) return wxSize(0, 0);
    return control->GetSize();
}

wxSize Control::GetMinSize()
{
    return minSize;
}

wxSize Control::GetMaxSize()
{
    return maxSize;
}

wxSize Control::GetDistance()
{
    if (!bInit) return wxSize(0, 0);
    return wxSize(GetPosition().x + GetSize().x, GetPosition().y + GetSize().y);
}



void Control::Show()
{
    if (!bInit) return;
    labelBox.Show();
    control->Show();
}

void Control::Hide()
{
    if (!bInit) return;
    labelBox.Hide();
    control->Hide();
}

void Control::Disable()
{
    control->Disable();
}

void Control::Enable()
{
    control->Enable();
}

