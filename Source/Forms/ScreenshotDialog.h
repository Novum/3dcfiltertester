///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ScreenshotDialog__
#define __ScreenshotDialog__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ScreenshotDialog
///////////////////////////////////////////////////////////////////////////////
class ScreenshotDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* sequence_length_static;
		wxStaticText* sequence_step_static;
		wxStaticText* file_static;
		
		wxStdDialogButtonSizer* dialog_button_sizer;
		wxButton* dialog_button_sizerOK;
		wxButton* dialog_button_sizerCancel;
	
	public:
		wxSpinCtrl* sequence_length;
		wxTextCtrl* sequence_step;
		wxFilePickerCtrl* file_picker;
		ScreenshotDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Save Screenshots"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 373,191 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ScreenshotDialog();
	
};

#endif //__ScreenshotDialog__
