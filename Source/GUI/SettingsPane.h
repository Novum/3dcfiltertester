#pragma once

#include <wx/wxprec.h>

namespace gui
{
	class Viewport;

	class SettingsPane : public wxWindow
	{
	public:
		SettingsPane(wxWindow *parent, Viewport *viewport);
		void ApplySliders();
	private:
		void OnSize(wxSizeEvent &event);
		void OnDisplaySliderChanged(wxScrollEvent &event);		
		void OnLERP(wxCommandEvent &event);
		void OnFilterModeSliderChanged(wxScrollEvent &event);
		void OnAnisotropySliderChanged(wxScrollEvent &event);
		void OnLODBiasSliderChanged(wxScrollEvent &event);
		void OnTexScaleSliderChanged(wxScrollEvent &event);
		void OnTexMovementSliderChanged(wxScrollEvent &event);

		Viewport *viewport;

		wxStaticText *display_slider_caption;
		wxSlider *display_slider;
		wxStaticText *display_value;
			
		wxCheckBox *trilinear_filtering_checkbox;

		wxStaticText *filter_mode_slider_caption;
		wxSlider *filter_mode_slider;
		wxStaticText *filter_mode_value;

		wxStaticText *max_anisotropy_slider_caption;
		wxSlider *max_anisotropy_slider;
		wxStaticText *max_anisotropy_value;

		wxStaticText *lod_bias_slider_caption;
		wxSlider *lod_bias_slider;
		wxStaticText *lod_bias_value;

		wxStaticText *tex_scale_slider_caption;
		wxSlider *tex_scale_slider;
		wxStaticText *tex_scale_value;

		wxStaticText *tex_movement_x_slider_caption;
		wxSlider *tex_movement_x_slider;
		wxStaticText *tex_movement_x_value;

		wxStaticText *tex_movement_y_slider_caption;
		wxSlider *tex_movement_y_slider;
		wxStaticText *tex_movement_y_value;

		DECLARE_CLASS(SettingsPane)
		DECLARE_EVENT_TABLE()
	};
}