#include "DropDown.h"

DropDown::DropDown()
{
	bInit = false;

    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;

    items.clear();

    minSize.x = 0;
    minSize.y = 0;

    maxSize.x = -1;
    maxSize.y = -1;
}

void DropDown::Init(std::wstring label, std::wstring initial_value, wxWindowID dropDownID, wxPoint position, wxSize size, wxWindow* parent, long style)
{
    labelBox.Create(
        parent, wxID_ANY, label,
        wxPoint(position.x - labelOffset.left, position.y - labelOffset.top),
        wxSize(labelOffset.left + size.x + labelOffset.right, labelOffset.top + size.y + labelOffset.bottom),
        0, label + L" label");

    labelBox.Bind(wxEVT_MOVE, &DropDown::OnLabelBoxMove, this, labelBox.GetId());

    listBox.Create(
        &labelBox, dropDownID, initial_value,
        wxPoint(labelOffset.left, labelOffset.top),
        size, 0, NULL,
        style, wxDefaultValidator, label + L" text field");

    listBox.SetEditable(false);

	bInit = true;
}

void DropDown::Init(std::wstring initial_value, wxWindowID dropDownID, wxPoint position, wxSize size, wxWindow* parent, long style)
{
    Init(L"", initial_value, dropDownID, position, size, parent, style);
}




void DropDown::OnLabelBoxMove(wxMoveEvent& event)
{
    listBox.SetPosition(wxPoint(labelOffset.left, labelOffset.top));
}




void DropDown::SetLabel(std::wstring label)
{
    if (!bInit) return;
    labelBox.SetLabel(label);
}

void DropDown::AppendLabel(std::wstring suffix, bool popLast)
{
    if (!bInit) return;

    std::wstring label = GetLabel();
    if (popLast && label.size() > 0) label.pop_back();

    labelBox.SetLabel(label + suffix);
}

std::wstring DropDown::GetLabel()
{
    if (!bInit) return L"";
    return labelBox.GetLabel().ToStdWstring();
}




void DropDown::AppendItem(std::string item)
{
    if (!bInit) return;

    listBox.Append(item);
    items.push_back(item);
}

void DropDown::SetItems(std::vector<std::string> newItems)
{
    if (!bInit) return;

    for (int i = 0; i < items.size(); i++)
    {
        listBox.Delete(i);
    }
    items.clear();

    for (int i = 0; i < newItems.size(); i++)
    {
        AppendItem(newItems[i]);
    }
}


std::string DropDown::GetItem(int index)
{
    if (!bInit) return "";

    if (index >= items.size() || index < 0)
    {
        return "";
    }

    return items[index];
}

std::vector<std::string> DropDown::GetItems()
{
    std::vector<std::string> rv;
    rv.clear();

    if (!bInit) return rv;

    rv = items;
    return items;
}




void DropDown::SetSelected(unsigned int index)
{
    if (!bInit) return;

    if (!items.empty())
    {
        if (index < items.size())
        {
            listBox.SetSelection(index);
        }
    }
}

std::string DropDown::GetSelected()
{
    if (!bInit) return "";

    return GetItem(listBox.GetCurrentSelection());
}





wxSize DropDown::ComputeLabelBoxSize(wxSize listBoxSize)
{
    return wxSize(labelOffset.left + listBoxSize.x + labelOffset.right, labelOffset.top + listBoxSize.y + labelOffset.bottom);
}

wxPoint DropDown::ComputeLabelBoxPos(wxPoint listBoxPos)
{
    return wxPoint(listBoxPos.x - labelOffset.left, listBoxPos.y - labelOffset.top);
}

wxSize DropDown::ComputeListBoxSize(wxSize labelBoxSize)
{
    return wxSize(labelBoxSize.x - (labelOffset.left + labelOffset.right), labelBoxSize.y - (labelOffset.top + labelOffset.bottom));
}

wxPoint DropDown::ComputeListBoxPos(wxPoint labelBoxPos)
{
    return wxPoint(labelBoxPos.x + labelOffset.left, labelBoxPos.y + labelOffset.top);
}



void DropDown::SetPosition(int x, int y)
{
    SetPosition(wxPoint(x, y));
}

void DropDown::SetPosition(wxPoint newPos)
{
    if (!bInit) return;

    labelBox.SetPosition(ComputeLabelBoxPos(newPos));
}



wxPoint DropDown::GetPosition()
{
    if (!bInit) return wxPoint(-1, -1);

    return ComputeListBoxPos(labelBox.GetPosition());
}



void DropDown::SetSize(int width, int height)
{
    SetSize(wxSize(width, height));
}

void DropDown::SetMinSize(int width, int height)
{
    minSize.x = width;
    minSize.y = height;
}

void DropDown::SetMaxSize(int width, int height)
{
    maxSize.x = width;
    maxSize.y = height;
}



void DropDown::SetSize(wxSize attemptedSize)
{
    if (!bInit) return;
    wxSize newSize = listBox.GetSize();

    if (attemptedSize.x >= minSize.x && (attemptedSize.x <= maxSize.x || maxSize.x == -1))
    {
        newSize.x = attemptedSize.x;
    }
    else if (attemptedSize.x < minSize.x) newSize.x = minSize.x;
    else if (attemptedSize.x > maxSize.x && maxSize.x != -1) newSize.x = maxSize.x;


    if (attemptedSize.y >= minSize.y && (attemptedSize.y <= maxSize.y || maxSize.y == -1))
    {
        newSize.y = attemptedSize.y;
    }
    else if (attemptedSize.y < minSize.y) newSize.y = minSize.y;
    else if (attemptedSize.y > maxSize.y && maxSize.y != -1) newSize.y = maxSize.y;



    labelBox.SetSize(ComputeLabelBoxSize(newSize));
    listBox.SetSize(newSize);
}

void DropDown::SetMinSize(wxSize newMinSize)
{
    minSize = newMinSize;
}

void DropDown::SetMaxSize(wxSize newMaxSize)
{
    maxSize = newMaxSize;
}




wxSize DropDown::GetSize()
{
    if (!bInit) return wxSize(0, 0);
    return listBox.GetSize();
}

wxSize DropDown::GetMinSize()
{
    return minSize;
}

wxSize DropDown::GetMaxSize()
{
    return maxSize;
}



wxSize DropDown::GetDistance()
{
    if (!bInit) return wxSize(0, 0);
    return wxSize(GetPosition().x + GetSize().x, GetPosition().y + GetSize().y);
}






void DropDown::Show()
{
    if (!bInit) return;
    labelBox.Show();
    listBox.Show();
}

void DropDown::Hide()
{
    if (!bInit) return;
    labelBox.Hide();
    listBox.Hide();
}







void DropDown::Disable()
{
    listBox.Disable();
}

void DropDown::Enable()
{
    listBox.Enable();
}
