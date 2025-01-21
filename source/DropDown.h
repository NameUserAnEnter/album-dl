#ifndef DROPDOWN_H
#define DROPDOWN_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


#include "encoding.h"
#include "utils.h"
#include "Rectang.h"







class DropDown
{
private:
	bool bInit;


	RECT labelOffset;

	std::vector<std::string> items;



	wxSize minSize;
	wxSize maxSize;

public:
	wxStaticBox labelBox;
	wxComboBox listBox;
	DropDown();

	Rectang rectang;

	void Init(wxWindowID, wxSize, wxWindow*, long = NULL);

private:
	wxSize ComputeLabelBoxSize(wxSize);
	wxPoint ComputeLabelBoxPos(wxPoint);

	wxSize ComputeListBoxSize(wxSize);
	wxPoint ComputeListBoxPos(wxPoint);

	void UpdateRectang();

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

	void Disable();
	void Enable();

	void SetLabel(std::wstring);
	void AppendLabel(std::wstring, bool = true);
	std::wstring GetLabel();
	
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

