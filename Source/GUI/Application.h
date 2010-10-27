#pragma once

#include <wx/wxprec.h>
#include <wx/image.h>
#include <boost/thread.hpp>

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
	private:
		boost::thread *render_thread;
	};

	DECLARE_APP(Application)
}