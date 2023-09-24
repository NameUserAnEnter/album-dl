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
	bool bInit;

	RECT labelOffset;

	std::vector<std::string> items;
public:
	wxStaticBox labelBox;
	wxComboBox listBox;
	DropDown();

	void Init(std::string, std::wstring, wxWindowID, wxPoint, wxSize, wxWindow*, long = NULL);

	void AppendItem(std::string);
	void SetItems(std::vector<std::string>);

	std::string GetItem(int);
	std::vector<std::string> GetItems();

	void SetSelected(unsigned int);
	std::string GetSelected();

	void Disable();
	void Enable();
};

#endif

