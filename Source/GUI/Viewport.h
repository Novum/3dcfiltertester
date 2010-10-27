#pragma once

#include "../Engine/Camera.h"
#include "../Engine/Timer.h"

#include <wx/wxprec.h>

namespace engine
{
	class SwapChain;	
}

namespace gui
{
	class Viewport : public wxWindow
	{
	public:
		Viewport(wxWindow *parent);
		~Viewport();

		void Render();
		void SetCameraRotation(engine::Vector3 rotation);
		void SaveScreenshot(std::wstring file);
	private:
		void UpdateCamera();		
		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent &event);
		void OnMouseEvent(wxMouseEvent& event);
		void OnCaptureLost(wxMouseCaptureLostEvent &event);

		engine::SwapChain *swap_chain;
		engine::Camera camera;

		float distance_to_center;		

		bool mouse_captured;
		int capturex, capturey;
		bool left;

		engine::Timer timer;

		DECLARE_CLASS(Viewport)
		DECLARE_EVENT_TABLE()
	};
}