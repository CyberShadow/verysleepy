/*=====================================================================
launchdlg.cpp
----------------

Copyright (C) Johan Kohler

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

http://www.gnu.org/copyleft/gpl.html.
=====================================================================*/

#include "CallstackView.h"
#include "contextmenu.h"
#include "mainwin.h"

enum {
	ID_COLLAPSE_FUNC=2001,
	ID_COLLAPSE_MOD,
	ID_SET_ROOT,
	ID_FILTER_FUNC,
	ID_FILTER_MODULE,
	ID_FILTER_SOURCE,
	ID_HIGHLIGHT,
	ID_UNHIGHLIGHT,
};

class FunctionMenuWindow: public wxWindow
{
public:
	int option;
	FunctionMenuWindow(wxWindow *parent) : wxWindow(parent,-1), option(0) {}
	DECLARE_EVENT_TABLE()
	void OnMenu(wxCommandEvent& event)
	{
		option = event.GetId();
	}
};


BEGIN_EVENT_TABLE(FunctionMenuWindow, wxWindow)
EVT_MENU(ID_COLLAPSE_FUNC, FunctionMenuWindow::OnMenu)
EVT_MENU(ID_COLLAPSE_MOD, FunctionMenuWindow::OnMenu)
EVT_MENU(ID_SET_ROOT, FunctionMenuWindow::OnMenu)
EVT_MENU(ID_FILTER_FUNC, FunctionMenuWindow::OnMenu)
EVT_MENU(ID_FILTER_MODULE, FunctionMenuWindow::OnMenu)
EVT_MENU(ID_FILTER_SOURCE, FunctionMenuWindow::OnMenu)
EVT_MENU(ID_HIGHLIGHT, FunctionMenuWindow::OnMenu)
EVT_MENU(ID_UNHIGHLIGHT, FunctionMenuWindow::OnMenu)
END_EVENT_TABLE()

void FunctionMenu(wxWindow *window, const Database::Symbol *sym, Database *database, wxPropertyGrid *filters, std::set<std::wstring>& highlights)
{
	FunctionMenuWindow funcWindow(window);
	wxMenu *menu = new wxMenu;
	
	std::wstring id = sym->id;
	wxString mod = sym->module.c_str();
	wxString function = sym->procname.c_str();
	wxString source = sym->sourcefile.c_str();

	wxString modUpper = mod;
	modUpper.UpperCase();

	menu->AppendCheckItem(ID_COLLAPSE_FUNC,"Collapse child calls")->Check(IsOsFunction(function));
	menu->AppendCheckItem(ID_COLLAPSE_MOD,wxString::Format("Collapse all %s calls", modUpper))->Check(IsOsModule(modUpper));
	menu->AppendCheckItem(ID_SET_ROOT,wxString::Format("Set root to %s", function));

	if( filters )
	{
		menu->AppendSeparator();
		menu->AppendCheckItem(ID_FILTER_FUNC,wxString::Format("Filter functions to %s", function));
		menu->AppendCheckItem(ID_FILTER_MODULE,wxString::Format("Filter Module to %s", mod));
		menu->AppendCheckItem(ID_FILTER_SOURCE,wxString::Format("Filter Source to %s", source));
	}

	menu->AppendSeparator();
	if( highlights.find( id ) == highlights.end() )
	{
		menu->AppendCheckItem(ID_HIGHLIGHT,wxString::Format("Highlight %s", function));
	}
	else
	{
		menu->AppendCheckItem(ID_UNHIGHLIGHT,wxString::Format("Unhighlight %s", function));
	}


	funcWindow.PopupMenu(menu);
	switch(funcWindow.option) {
	case ID_COLLAPSE_FUNC:
		if (IsOsFunction(function))
			RemoveOsFunction(function);
		else
			AddOsFunction(function);
		break;
	case ID_COLLAPSE_MOD:
		if (IsOsModule(mod))
			RemoveOsModule(mod);
		else
			AddOsModule(mod);
		break;
	case ID_SET_ROOT:
		database->setRoot(sym);
		theMainWin->Reset();
		break;

	case ID_FILTER_FUNC:
		filters->GetProperty("procname")->SetValueFromString(function);
		break;

	case ID_FILTER_MODULE:
		filters->GetProperty("module")->SetValueFromString(mod);
		break;

	case ID_FILTER_SOURCE:
		filters->GetProperty("sourcefile")->SetValueFromString(source);
		break;

	case ID_HIGHLIGHT:
		highlights.insert(id);
		break;
	case ID_UNHIGHLIGHT:
		highlights.erase(id);
		break;
	}
}

