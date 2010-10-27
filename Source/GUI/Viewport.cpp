#include "Viewport.h"
#include "../Engine/Engine.h"
#include "../Engine/Renderer.h"
#include "../Engine/SwapChain.h"

namespace gui
{
	static const float rotation_factor = 0.0025f;

	IMPLEMENT_CLASS(Viewport,wxWindow)
	BEGIN_EVENT_TABLE(Viewport,wxWindow)		
		EVT_SIZE(OnSize)
		EVT_PAINT(OnPaint)
		EVT_MOUSE_EVENTS(OnMouseEvent)
		EVT_MOUSE_CAPTURE_LOST(OnCaptureLost)
	END_EVENT_TABLE()

	Viewport::Viewport(wxWindow *parent) : swap_chain(0),
		wxWindow(parent,-1), mouse_captured(false), left(false), distance_to_center(2.0f)
	{
		swap_chain = engine::engine.CreateSwapChain((HWND)GetHandle(),1);
		engine::engine.GetRenderer().SetSwapChain(swap_chain);
		timer.Start();
	}

	Viewport::~Viewport()
	{
		timer.Stop();
		engine::engine.DestroySwapChain(swap_chain);
	}

	void Viewport::Render()
	{
		if(!swap_chain) return;
		swap_chain->ClearColor();
		engine::engine.SetCamera(&camera);
		engine::engine.GetRenderer().Render(timer.Tick());
		swap_chain->Present();
	}

	void Viewport::SetCameraRotation(engine::Vector3 rotation)
	{
		camera.SetRotation(rotation);
		UpdateCamera();
	}

	void Viewport::SaveScreenshot(std::wstring file)
	{
		unsigned char *rgba_buffer = new unsigned char[swap_chain->GetWidth() * swap_chain->GetHeight() * 4];		
		swap_chain->CopyToSystemRAM(rgba_buffer);

		unsigned char *rgb_buffer = new unsigned char[swap_chain->GetWidth() * swap_chain->GetHeight() * 3];		
		for(unsigned int i=0; i<swap_chain->GetWidth() * swap_chain->GetHeight(); ++i) {
			rgb_buffer[i*3] = rgba_buffer[i*4];
			rgb_buffer[i*3 + 1] = rgba_buffer[i*4 + 1];
			rgb_buffer[i*3 + 2] = rgba_buffer[i*4 + 2];
		}

		wxImage image(swap_chain->GetWidth(), swap_chain->GetHeight(), rgb_buffer, true);
		image.SaveFile(file.c_str(), wxBITMAP_TYPE_PNG);

		delete[] rgb_buffer;
		delete[] rgba_buffer;
	}

	void Viewport::UpdateCamera()
	{		
		camera.Update();
		camera.SetPosition(camera.GetForwardVector() * distance_to_center);		
	}

	void Viewport::OnSize(wxSizeEvent& event)
	{
		wxRect client_rect = GetClientRect();
		
		swap_chain->Recreate();		
		camera.SetAspectRatio((float)client_rect.width/(float)client_rect.height);
		camera.SetViewportSize(client_rect.width, client_rect.height);
		engine::engine.GetRenderer().SetSwapChain(swap_chain);

		Render();		
	}

	void Viewport::OnPaint(wxPaintEvent &event)
	{
		wxPaintDC dc(this);		
		Render();
	}

	void Viewport::OnMouseEvent(wxMouseEvent& event)
	{
		if (event.LeftDown()) {	
			left = true;
			if(!mouse_captured) {
				capturex = event.GetX(); capturey = event.GetY();
				CaptureMouse();
				ShowCursor(false);
				mouse_captured = true;
			}
		}
		else if(event.LeftUp()) {
			left = false;
			if(mouse_captured) { 
				ReleaseMouse();
				ShowCursor(true);
				mouse_captured = false;			
			}	
		}
		else if(event.Dragging()) {	
			if(!mouse_captured) return;

			int deltax, deltay;
			deltax = event.GetX()-capturex;
			deltay = event.GetY()-capturey;
			
			engine::Vector3 rotate;
			rotate[0] = float(deltay) * -rotation_factor;
			rotate[1] = float(deltax) * -rotation_factor;
			rotate[2] = 0.0f;
			camera.Rotate(rotate);

			UpdateCamera();
			WarpPointer(capturex,capturey);	
			Render();
		}
	}

	void Viewport::OnCaptureLost(wxMouseCaptureLostEvent &event)
	{
		ShowCursor(true);
		mouse_captured = false;
	}
}