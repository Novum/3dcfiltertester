#include "Application.h"
#include "Frame.h"
#include "Viewport.h"
#include "SettingsPane.h"
#include "../Forms/ScreenshotDialog.h"
#include "../Engine/Engine.h"
#include "../Engine/Renderer.h"
#include "../Engine/ProbeObjectManager.h"

#include <wx/image.h>

#include "../Misc/mmgr.h"

namespace gui 
{
	enum {	
		Menu_LoadTexture,
		Menu_SaveScreenshot,
		Menu_SaveScreenshots,
		Menu_Quit,
		Menu_About,				
		Menu_Toggle_Settings,				
		Menu_Toggle_Tunnel,
		Menu_Toggle_Refrast,
		Menu_MemoryReport
	};

	IMPLEMENT_CLASS(Frame, wxFrame)
	BEGIN_EVENT_TABLE(Frame, wxFrame)
		EVT_MENU_OPEN(OnMenuOpen)
		EVT_MENU(Menu_LoadTexture, OnLoadTexture)
		EVT_MENU(Menu_SaveScreenshot, OnSaveScreenshot)
		EVT_MENU(Menu_SaveScreenshots, OnSaveScreenshots)
		EVT_MENU(Menu_About, OnAbout)
		EVT_MENU(Menu_Quit, OnQuit)				
		EVT_MENU(Menu_Toggle_Settings, OnToggleSettings)		
		EVT_MENU(Menu_Toggle_Tunnel, OnToggleTunnel)	
		EVT_IDLE(OnIdle)
#ifdef _DEBUG
		EVT_MENU(Menu_Toggle_Refrast, OnToggleRefrast)
		EVT_MENU(Menu_MemoryReport, OnMemoryReport)
#endif
		EVT_ERASE_BACKGROUND(OnErase)		
		EVT_CLOSE(OnClose)
	END_EVENT_TABLE()

	Frame::Frame() : viewport(0), wxFrame(0,-1,L"3DCenter Filter Tester", wxDefaultPosition, wxSize(1024,768), wxDEFAULT_FRAME_STYLE)
	{
		aui_manager.SetManagedWindow(this);

		// Menu 
		wxMenu *file_menu = new wxMenu;		
		wxMenu *view_menu = new wxMenu;
		wxMenu *help_menu = new wxMenu;
		
		file_menu->Append(Menu_LoadTexture, L"Load &Texture", L"Load new texture for viewing");
		file_menu->Append(Menu_SaveScreenshot, L"Save S&creenshot...", L"Save screenshot");
		file_menu->Append(Menu_SaveScreenshots, L"Save &Screenshots...", L"Save screenshot sequence");
		file_menu->AppendSeparator();
		file_menu->Append(Menu_Quit, L"E&xit", L"Quit this program");		
				
		view_menu->AppendCheckItem(Menu_Toggle_Settings, L"Show &Settings", L"Show settings pane");		
		view_menu->AppendCheckItem(Menu_Toggle_Tunnel, L"Show &Tunnel", L"Shows tunnel mesh instead of plane");			
#ifdef _DEBUG
		view_menu->AppendSeparator();
		view_menu->AppendCheckItem(Menu_Toggle_Refrast, L"Use &Refrast (debug)", L"Use the reference rasterizer");

		help_menu->Append(Menu_MemoryReport, L"&Dump memory report (debug)", L"Dump memory report");
		help_menu->AppendSeparator();
#endif
		help_menu->Append(Menu_About, L"&About...", L"Show about dialog");

		menu_bar = new wxMenuBar();
		menu_bar->Append(file_menu, L"&File");		
		menu_bar->Append(view_menu, L"&View");
		menu_bar->Append(help_menu, L"&Help");

		SetMenuBar(menu_bar);

		SetIcon(wxICON(IDI_ICON));

		aui_manager.AddPane(viewport = new Viewport(this),
			wxAuiPaneInfo().Name(L"viewport").CenterPane().PaneBorder(false));
		aui_manager.AddPane(settings_pane = new SettingsPane(this, viewport), 
			wxAuiPaneInfo().Caption(L"Settings").Name(L"settings").Right().PinButton(true).FloatingSize(250,500));

		config = new wxConfig(L"3DCenter Filter Tester");
		wxString perspective;
		if(config->Read(L"AUI",&perspective)) aui_manager.LoadPerspective(perspective);

		aui_manager.Update();

		engine::ProbeObject *plane_probe = engine::probe_object_manager.GetPlaneProbe();

		viewport->SetCameraRotation(plane_probe->GetDefaultCameraRotation());
		engine::engine.GetRenderer().SetProbeObject(plane_probe);
	}	

	Frame::~Frame()
	{
		config->Write(L"AUI",aui_manager.SavePerspective());		
		delete config;
		aui_manager.UnInit();
	}

	void Frame::OnMenuOpen(wxMenuEvent& event)
	{
		menu_bar->Check(Menu_Toggle_Settings, aui_manager.GetPane(L"settings").IsShown());		
	}

	void Frame::OnQuit(wxCommandEvent &event)
	{		
		Close();
	}

	void Frame::OnClose(wxCloseEvent &event)
	{
		delete viewport;		
		viewport = 0;		

		Destroy();
	}

	void Frame::OnLoadTexture(wxCommandEvent& event)
	{
		wxFileDialog dialog(this,L"Load Texture",L"Texture",wxEmptyString,L"DDS (*.dds)|*.dds",wxFD_OPEN);

		if(dialog.ShowModal() == wxID_OK) {
			engine::Renderer &renderer = engine::engine.GetRenderer();
			renderer.LoadTexture(dialog.GetPath().wc_str());			
		}
	}

	void Frame::OnSaveScreenshot(wxCommandEvent& event)
	{
		wxFileDialog dialog(this, L"Save Screenshot", L"Screenshot", wxEmptyString, L"PNG (*.png)|*.png", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

		if(dialog.ShowModal() == wxID_OK) {
			std::wstring path = dialog.GetPath();

			viewport->SaveScreenshot(path);
		}
	}

	void Frame::OnSaveScreenshots(wxCommandEvent& event)
	{
		ScreenshotDialog screenshot_dialog(this);
		
		if(screenshot_dialog.ShowModal() == wxID_OK) {				
			std::wstring path = screenshot_dialog.file_picker->GetPath().c_str();
			int sequence_length = screenshot_dialog.sequence_length->GetValue();
			
			float sequence_step;
			try {
				std::wstring sequence_step_string = screenshot_dialog.sequence_step->GetValue().c_str();
				sequence_step = boost::lexical_cast<float>(sequence_step_string);
			}
			catch(boost::bad_lexical_cast) {
				wxMessageBox(L"Invalid step size", L"Error", wxOK | wxICON_ERROR, this);
				return;
			}

			if(path.size() == 0) {
				wxMessageBox(L"Please select a path", L"Error", wxOK | wxICON_ERROR, this);
				return;
			}
			
			engine::Renderer &renderer = engine::engine.GetRenderer();

			engine::ProbeObject *old_probe = renderer.GetProbeObject();
			engine::ProbeObject *plane_probe = engine::probe_object_manager.GetPlaneProbe();

			renderer.SetTextureScale(1.0f);
			renderer.SetTextureMovement(0.0f, 0.0f);

			renderer.SetProbeObject(plane_probe);
			viewport->SetCameraRotation(plane_probe->GetDefaultCameraRotation());

			for(int i=0; i<sequence_length; ++i) {	
				renderer.SetTexcoordTranslation(0.0f, -i * sequence_step);
				viewport->Render();
				viewport->SaveScreenshot(path + L"_" + boost::lexical_cast<std::wstring>(i+1) + L".png");				
			}

			renderer.SetTextureMovement(0.0f, 0.0f);
			settings_pane->ApplySliders();
			renderer.SetProbeObject(old_probe);
			viewport->SetCameraRotation(old_probe->GetDefaultCameraRotation());
			viewport->Render();
		}		
	}

	void Frame::OnToggleSettings(wxCommandEvent& event)
	{
		wxAuiPaneInfo &pane_info = aui_manager.GetPane(L"settings");

		if(pane_info.IsShown())	pane_info.Hide();
		else pane_info.Show();

		aui_manager.Update();
	}

	void Frame::OnToggleTunnel(wxCommandEvent& event)
	{
		if(event.IsChecked()) {
			engine::ProbeObject *tunnel_probe = engine::probe_object_manager.GetTunnelProbe();
			viewport->SetCameraRotation(tunnel_probe->GetDefaultCameraRotation());
			engine::engine.GetRenderer().SetProbeObject(tunnel_probe);
		}
		else {
			engine::ProbeObject *plane_probe = engine::probe_object_manager.GetPlaneProbe();
			viewport->SetCameraRotation(plane_probe->GetDefaultCameraRotation());
			engine::engine.GetRenderer().SetProbeObject(plane_probe);
		}		
	}

	void Frame::OnIdle(wxIdleEvent &event)
	{
		if(viewport) viewport->Render();		
		event.RequestMore();
	}

#ifdef _DEBUG
	void Frame::OnToggleRefrast(wxCommandEvent& event)
	{
		engine::engine.ToggleRefrast();
	}

	void Frame::OnMemoryReport(wxCommandEvent& event)
	{
		m_dumpMemoryReport();
	}
#endif

	void Frame::OnAbout(wxCommandEvent& event)
	{
		wxMessageBox(L"3DCenter Filter Tester\nVersion 1.1\nAxel Gneiting 2009\n", 
			L"About 3DCenter Filter Tester", wxOK | wxICON_INFORMATION, this);
	}
}