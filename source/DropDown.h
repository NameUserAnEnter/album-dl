#ifndef DROPDOWN_H
#define DROPDOWN_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


#include "encoding.h"
#include "utils.h"
#include "Control.h"







class DropDown : public Control
{
private:
	RECT labelOffset;

	std::vector<std::string> items;




public:
	wxComboBox listBox;
	DropDown();

	void Init(wxWindowID, wxSize, wxWindow*, long = NULL);

public:
	void AppendItem(std::string);
	void SetItems(std::vector<std::string>);

	std::string GetItem(int);
	std::vector<std::string> GetItems();

	void SetSelected(unsigned int);
	void SetValue(std::wstring value);
	std::string GetSelected();
};

#endif

