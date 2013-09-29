#include "SettingsPane.h"
#include "Viewport.h"
#include "../Engine/Engine.h"
#include "../Engine/Renderer.h"

#include <string>
#include <sstream>

namespace gui
{
	enum {
		display_slider_id,		
		mipmap_lerp_checkbox_id,
		filter_mode_slider_id,
		max_anisotropy_slider_id,
		lod_bias_slider_id,
		tex_scale_slider_id,
		tex_movement_x_slider_id,		
		tex_movement_y_slider_id
	};

	IMPLEMENT_CLASS(SettingsPane, wxWindow)
	BEGIN_EVENT_TABLE(SettingsPane, wxWindow)	
		EVT_SIZE(OnSize)
		EVT_COMMAND_SCROLL(display_slider_id, OnDisplaySliderChanged)		
		EVT_CHECKBOX(mipmap_lerp_checkbox_id, OnLERP)
		EVT_COMMAND_SCROLL(filter_mode_slider_id, OnFilterModeSliderChanged)
		EVT_COMMAND_SCROLL(max_anisotropy_slider_id, OnAnisotropySliderChanged)
		EVT_COMMAND_SCROLL(lod_bias_slider_id, OnLODBiasSliderChanged)
		EVT_COMMAND_SCROLL(tex_scale_slider_id, OnTexScaleSliderChanged)
		EVT_COMMAND_SCROLL(tex_movement_x_slider_id, OnTexMovementSliderChanged)
		EVT_COMMAND_SCROLL(tex_movement_y_slider_id, OnTexMovementSliderChanged)
	END_EVENT_TABLE()

	SettingsPane::SettingsPane(wxWindow *parent, Viewport *viewport) 
		: wxWindow(parent, -1, wxDefaultPosition, wxSize(250,300)), viewport(viewport)
	{
		display_slider_caption = new wxStaticText(this, -1, L"Display Mode:");
		display_slider = new wxSlider(this, display_slider_id, 0, 0, 2);
		display_value = new wxStaticText(this, -1, L"TMU");
		
		trilinear_filtering_checkbox = new wxCheckBox(this, mipmap_lerp_checkbox_id, L"ALU Mipmap Interpolation");
		trilinear_filtering_checkbox->SetValue(true);

		filter_mode_slider_caption = new wxStaticText(this, -1, L"ALU Filtering Mode:");
		filter_mode_slider = new wxSlider(this, filter_mode_slider_id, 0, 0, 2);
		filter_mode_value = new wxStaticText(this, -1, L"Simple");

		max_anisotropy_slider_caption = new wxStaticText(this, -1, L"Max Anisotropy:");
		max_anisotropy_slider = new wxSlider(this, max_anisotropy_slider_id, 1, 1, 10);
		max_anisotropy_value = new wxStaticText(this, -1, L"1x");

		lod_bias_slider_caption = new wxStaticText(this, -1, L"LOD Bias:");
		lod_bias_slider = new wxSlider(this, lod_bias_slider_id, 0, -100, 100);
		lod_bias_value = new wxStaticText(this, -1, L"0.0");

		tex_scale_slider_caption = new wxStaticText(this, -1, L"Texture Scale:");
		tex_scale_slider = new wxSlider(this, tex_scale_slider_id, 0, -20, 20);
		tex_scale_value = new wxStaticText(this, -1, L"1.0x");

		tex_movement_x_slider_caption = new wxStaticText(this, -1, L"Texture Movement X:");
		tex_movement_x_slider = new wxSlider(this, tex_movement_x_slider_id, 0, -30, 30);
		tex_movement_x_value = new wxStaticText(this, -1, L"0.0");

		tex_movement_y_slider_caption = new wxStaticText(this, -1, L"Texture Movement Y:");
		tex_movement_y_slider = new wxSlider(this, tex_movement_y_slider_id, 0, -30, 30);
		tex_movement_y_value = new wxStaticText(this, -1, L"0.0");
	}

	void SettingsPane::ApplySliders()
	{
		wxScrollEvent event;

		OnFilterModeSliderChanged(event);
		OnAnisotropySliderChanged(event);
		OnLODBiasSliderChanged(event);
		OnTexScaleSliderChanged(event);
		OnTexMovementSliderChanged(event);
	}

	void SettingsPane::OnSize(wxSizeEvent &event)
	{
		unsigned top;
		unsigned int slide_width = GetSize().GetWidth() - 60;
		unsigned int value_pos_x = GetSize().GetWidth() - 55;

		top = 8;
		display_slider_caption->SetSize(12, top, 150, 23);
		display_slider->SetSize(5, top+20, slide_width , 25);
		display_value->SetSize(value_pos_x, top+23, 40, 23);

		top += 50;
		trilinear_filtering_checkbox->SetSize(12, top, 150, 23);

		top += 32;
		filter_mode_slider_caption->SetSize(12, top, 150, 23);
		filter_mode_slider->SetSize(5, top+20, slide_width , 25);
		filter_mode_value->SetSize(value_pos_x, top+23, 40, 23);

		top += 55;
		max_anisotropy_slider_caption->SetSize(12, top, 150, 23);
		max_anisotropy_slider->SetSize(5, top+20, slide_width , 25);
		max_anisotropy_value->SetSize(value_pos_x, top+23, 40, 23);

		top += 55;
		lod_bias_slider_caption->SetSize(12, top, 150, 23);
		lod_bias_slider->SetSize(5, top+20, slide_width, 25);
		lod_bias_value->SetSize(value_pos_x, top+23, 40, 23);

		top += 55;
		tex_scale_slider_caption->SetSize(12, top, 150, 23);
		tex_scale_slider->SetSize(5, top+20, slide_width, 25);
		tex_scale_value->SetSize(value_pos_x, top+23, 40, 23);

		top += 55;
		tex_movement_x_slider_caption->SetSize(12, top, 150, 23);
		tex_movement_x_slider->SetSize(5, top+20, slide_width, 25);
		tex_movement_x_value->SetSize(value_pos_x, top+23, 40, 23);

		top += 55;
		tex_movement_y_slider_caption->SetSize(12, top, 150, 23);
		tex_movement_y_slider->SetSize(5, top+20, slide_width, 25);
		tex_movement_y_value->SetSize(value_pos_x, top+23, 40, 23);
	}

	void SettingsPane::OnDisplaySliderChanged(wxScrollEvent &event)
	{
		engine::Renderer &renderer = engine::engine.GetRenderer();

		unsigned int value = display_slider->GetValue();
		if(value == 0) {
			display_value->SetLabel(L"TMU");
			renderer.SetRenderMode(engine::Renderer::render_tmu);
		}
		else if(value == 1) {
			display_value->SetLabel(L"ALU");
			renderer.SetRenderMode(engine::Renderer::render_alu);
		}
		else {
			display_value->SetLabel(L"Split");
			renderer.SetRenderMode(engine::Renderer::render_split);
		}
	}

	void SettingsPane::OnLERP(wxCommandEvent &event)
	{
		engine::engine.GetRenderer().ToggleLinearInterpolation();
	}

	void SettingsPane::OnFilterModeSliderChanged(wxScrollEvent &event)
	{
		unsigned int value = filter_mode_slider->GetValue();
		if(value == 0) filter_mode_value->SetLabel(L"Simple");
		else if(value == 1) filter_mode_value->SetLabel(L"Improved");		
		else filter_mode_value->SetLabel(L"Perfect");

		engine::engine.GetRenderer().SetFilterMode(value);
	}

	void SettingsPane::OnAnisotropySliderChanged(wxScrollEvent &event)
	{
		unsigned int value = 1 << (max_anisotropy_slider->GetValue()-1);
		max_anisotropy_value->SetLabel(std::to_wstring(value) + L"x");
		engine::engine.GetRenderer().SetMaxAnisotropy(value);		
	}

	void SettingsPane::OnLODBiasSliderChanged(wxScrollEvent &event)
	{
		float value = (float)(lod_bias_slider->GetValue()) / 20.0f;		
		
		std::wstringstream ss;
		ss.precision(2);
		ss << value;
		std::wstring string_value = ss.str();		
		lod_bias_value->SetLabel(string_value);

		engine::engine.GetRenderer().SetLODBias(value);
	}

	void SettingsPane::OnTexScaleSliderChanged(wxScrollEvent &event)
	{
		float tex_scale = pow(10.0f, (float)(tex_scale_slider->GetValue()) / 20.0f);

		std::wstringstream ss;
		ss.precision(2);
		ss << tex_scale;
		std::wstring string_value = ss.str();		
		tex_scale_value->SetLabel(string_value + L"x");

		engine::engine.GetRenderer().SetTextureScale(tex_scale);
	}

	void SettingsPane::OnTexMovementSliderChanged(wxScrollEvent &event)
	{
		float x_movement = (float)(tex_movement_x_slider->GetValue()) / 100.0f;	
		float y_movement = (float)(tex_movement_y_slider->GetValue()) / 100.0f;	

		std::wstringstream x_stringstream;
		x_stringstream.precision(2);		
		x_stringstream << x_movement;
		std::wstring x_movement_string_value = x_stringstream.str();
		tex_movement_x_value->SetLabel(x_movement_string_value);
		
		std::wstringstream y_stringstream;
		y_stringstream.precision(2);		
		y_stringstream << y_movement;
		std::wstring y_movement_string_value = y_stringstream.str();		
		tex_movement_y_value->SetLabel(y_movement_string_value);

		engine::engine.GetRenderer().SetTextureMovement(x_movement, y_movement);
	}
}