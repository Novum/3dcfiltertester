#pragma once

#include <wx/wxprec.h>
#include <wx/aui/aui.h>
#include <wx/config.h>

namespace gui
{
	class Viewport;
	class SettingsPane;

	/*
	* Main editor frame. Handles menu/toolbar interactions
	* and resizes it's client frames automatically.
	*/
	class Frame : public wxFrame
	{
	public:
		Frame();	
		~Frame();		
	private:
		void OnMenuOpen(wxMenuEvent& event);
		void OnLoadTexture(wxCommandEvent& event);
		void OnSaveScreenshot(wxCommandEvent& event);
		void OnSaveScreenshots(wxCommandEvent& event);
		void OnToggleSettings(wxCommandEvent& event);
		void OnToggleTunnel(wxCommandEvent& event);
		void OnToggleRefrast(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnIdle(wxIdleEvent &event);
		void OnQuit(wxCommandEvent& event);
		void OnErase(wxEraseEvent &event) {}
		void OnClose(wxCloseEvent &event);

		wxConfig *config;
		wxAuiManager aui_manager;
		Viewport *viewport;	
		SettingsPane *settings_pane;
		
		wxMenuBar *menu_bar;		

		DECLARE_CLASS(Frame)
		DECLARE_EVENT_TABLE()
	};
}