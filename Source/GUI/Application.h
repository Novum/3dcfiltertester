#pragma once

#include <wx/wxprec.h>
#include <wx/image.h>
#include <thread>

namespace gui
{
	/*
	* Main wxWidgets class. 
	* Initializes/Deinitializes everything else
	*/
	class Application : public wxApp
	{
	public:
		virtual bool OnInit();
		virtual int OnRun();
		virtual int OnExit();
		virtual bool OnExceptionInMainLoop() { throw; }
	};

	DECLARE_APP(Application)
}