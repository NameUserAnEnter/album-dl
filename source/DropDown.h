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
	bool bInit;

	RECT labelOffset;

	std::vector<std::string> items;

	wxStaticBox labelBox;
	wxComboBox listBox;

	wxSize minSize;
	wxSize maxSize;
public:
	DropDown();

	void Init(std::wstring, std::wstring, wxWindowID, wxPoint, wxSize, wxWindow*, long = NULL);
	void Init(std::wstring, wxWindowID, wxPoint, wxSize, wxWindow*, long = NULL);

	void AppendItem(std::string);
	void SetItems(std::vector<std::string>);

	std::string GetItem(int);
	std::vector<std::string> GetItems();

	void SetSelected(unsigned int);
	std::string GetSelected();

	void Disable();
	void Enable();

private:
	wxSize ComputeLabelBoxSize(wxSize);
	wxPoint ComputeLabelBoxPos(wxPoint);

	wxSize ComputeListBoxSize(wxSize);
	wxPoint ComputeListBoxPos(wxPoint);

	void OnLabelBoxMove(wxMoveEvent&);
public:
	void SetPosition(int, int);
	void SetPosition(wxPoint);

	wxPoint GetPosition();

	void SetSize(int, int);
	void SetMinSize(int, int);
	void SetMaxSize(int, int);

	void SetSize(wxSize);
	void SetMinSize(wxSize);
	void SetMaxSize(wxSize);

	wxSize GetSize();
	wxSize GetMinSize();
	wxSize GetMaxSize();

	wxSize GetDistance();


	void Show();
	void Hide();

	void SetLabel(std::wstring);
	void AppendLabel(std::wstring, bool = true);
	std::wstring  GetLabel();
};

#endif

