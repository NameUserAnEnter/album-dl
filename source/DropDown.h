#ifndef DROPDOWN_H
#define DROPDOWN_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "encoding.h"
#include "utils.h"

class DropDown
{
private:
	wxStaticBox labelBox;
	wxComboBox listBox;

	bool bInit;

	RECT labelOffset;

	std::vector<std::string> items;
public:
	DropDown();

	void Init(std::string, std::wstring, wxWindowID, wxPoint, wxSize, wxPanel*, long = NULL);

	void AppendItem(std::string);
	void SetItems(std::vector<std::string>);

	std::string GetItem(unsigned int);
	std::vector<std::string> GetItems();
};

#endif

