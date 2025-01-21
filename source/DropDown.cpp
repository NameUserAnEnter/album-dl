#include "DropDown.h"

DropDown::DropDown() : Control(&listBox)
{
    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;

    items.clear();
}

void DropDown::Init(wxWindowID id, wxSize size, wxWindow* parent, long style)
{
    Control::Init(id, size, parent, style);

    listBox.Create(
        &labelBox, id, L"",
        wxPoint(labelOffset.left, labelOffset.top),
        size, 0, NULL,
        style);

    listBox.SetEditable(false);

	bInit = true;
    UpdateRectang();
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

void DropDown::SetValue(std::wstring value)
{
    listBox.SetValue(value);
}

std::string DropDown::GetSelected()
{
    if (!bInit) return "";

    return GetItem(listBox.GetCurrentSelection());
}

