#include "DropDown.h"

DropDown::DropDown()
{
	bInit = false;

    labelOffset.left = 3;
    labelOffset.right = 3;
    labelOffset.top = 15;
    labelOffset.bottom = 3;

    items.clear();
}

void DropDown::Init(std::string label, std::wstring initial_value, wxWindowID dropDownID, wxPoint position, wxSize size, wxPanel* panel, long style)
{
    labelBox.Create(
        panel, wxID_ANY, label,
        wxPoint(position.x - labelOffset.left, position.y - labelOffset.top),
        wxSize(labelOffset.left + size.x + labelOffset.right, labelOffset.top + size.y + labelOffset.bottom),
        0, label + L" label");


    listBox.Create(
        &labelBox, dropDownID, initial_value,
        wxPoint(labelOffset.left, labelOffset.top),
        size, 0, NULL,
        style, wxDefaultValidator, label + L" text field");

    listBox.SetEditable(false);

	bInit = true;
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


std::string DropDown::GetItem(unsigned int index)
{
    if (!bInit) return "";

    if (index >= items.size()) return "";

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
