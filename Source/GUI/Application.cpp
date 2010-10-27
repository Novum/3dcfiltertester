#include "Application.h"
#include "Frame.h"
#include "Exception.h"
#include "../Engine/Engine.h"
#include "../Engine/Exception.h"
#include "../Engine/ProbeObjectManager.h"

#include "../Misc/mmgr.h"

#include <wx/sysopt.h>
#include <exception>

namespace gui
{
	IMPLEMENT_APP(Application)

	bool Application::OnInit() 
	{	
		try {
			engine::engine.Initialize();
			engine::probe_object_manager.Initialize();

			// Load wxWidgets PNG functionality
			wxImage::AddHandler(new wxPNGHandler);

			// Make PNG icons pretty...
			wxSystemOptions::SetOption(wxT("msw.remap"),0);

			// Create and show main frame
			Frame *frame = new Frame();
			frame->Show(true);
			SetTopWindow(frame);

			return true;
		}
		catch(engine::Exception &exception) {
			const wchar_t *what = exception;
			wxMessageBox(what,L"Renderer Exception",wxOK|wxICON_ERROR);			
		}
		catch(gui::Exception &exception) {
			const wchar_t *what = exception;
			wxMessageBox(what,L"GUI Exception",wxOK|wxICON_ERROR);
		}
		catch(std::exception &exception) {
			wxString what(exception.what(),wxConvUTF8);
			wxMessageBox(what,L"STL Exception",wxOK|wxICON_ERROR);
		}
		catch(...) {
			wxMessageBox(L"Unknown Exception",L"Exception",wxOK|wxICON_ERROR);
		}

		return false;
	}

	int Application::OnRun()
	{
		try {
			return wxApp::OnRun();
		}
		catch(engine::Exception &exception) {
			const wchar_t *what = exception;
			wxMessageBox(what,L"Renderer Exception",wxOK|wxICON_ERROR);			
		}
		catch(gui::Exception &exception) {
			const wchar_t *what = exception;
			wxMessageBox(what,L"GUI Exception",wxOK|wxICON_ERROR);
		}
		catch(std::exception &exception) {
			wxString what(exception.what(),wxConvUTF8);
			wxMessageBox(what,L"STL Exception",wxOK|wxICON_ERROR);
		}
		catch(...) {
			wxMessageBox(L"Unknown Exception",L"Exception",wxOK|wxICON_ERROR);			
		}

		return -1;
	}

	int Application::OnExit()
	{		
		engine::probe_object_manager.Deinitalize();
		engine::engine.Deinitalize();

		return 0;	
	}
}