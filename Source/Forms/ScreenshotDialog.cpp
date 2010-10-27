///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ScreenshotDialog.h"

///////////////////////////////////////////////////////////////////////////

ScreenshotDialog::ScreenshotDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* settings_static_box;
	settings_static_box = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Settings") ), wxVERTICAL );
	
	wxFlexGridSizer* settings_sizer;
	settings_sizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	settings_sizer->AddGrowableCol( 1 );
	settings_sizer->AddGrowableRow( 3 );
	settings_sizer->SetFlexibleDirection( wxBOTH );
	settings_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	sequence_length_static = new wxStaticText( this, wxID_ANY, wxT("Sequence Length:"), wxDefaultPosition, wxDefaultSize, 0 );
	sequence_length_static->Wrap( -1 );
	settings_sizer->Add( sequence_length_static, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	sequence_length = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxSP_ARROW_KEYS, 1, 1000, 50 );
	settings_sizer->Add( sequence_length, 0, wxALL|wxEXPAND, 5 );
	
	sequence_step_static = new wxStaticText( this, wxID_ANY, wxT("Sequence Step Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	sequence_step_static->Wrap( -1 );
	settings_sizer->Add( sequence_step_static, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	sequence_step = new wxTextCtrl( this, wxID_ANY, wxT("0.002"), wxDefaultPosition, wxDefaultSize, 0 );
	settings_sizer->Add( sequence_step, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	file_static = new wxStaticText( this, wxID_ANY, wxT("File Name Prefix:"), wxDefaultPosition, wxDefaultSize, 0 );
	file_static->Wrap( -1 );
	settings_sizer->Add( file_static, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	file_picker = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxFLP_SAVE|wxFLP_USE_TEXTCTRL );
	settings_sizer->Add( file_picker, 0, wxALL|wxEXPAND, 5 );
	
	settings_static_box->Add( settings_sizer, 0, wxEXPAND, 5 );
	
	sizer->Add( settings_static_box, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	
	sizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	dialog_button_sizer = new wxStdDialogButtonSizer();
	dialog_button_sizerOK = new wxButton( this, wxID_OK );
	dialog_button_sizer->AddButton( dialog_button_sizerOK );
	dialog_button_sizerCancel = new wxButton( this, wxID_CANCEL );
	dialog_button_sizer->AddButton( dialog_button_sizerCancel );
	dialog_button_sizer->Realize();
	sizer->Add( dialog_button_sizer, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( sizer );
	this->Layout();
}

ScreenshotDialog::~ScreenshotDialog()
{
}
