#include "Engine.h"
#include "Exception.h"
#include "D3DHelper.h"
#include "SwapChain.h"
#include "Renderer.h"
#include "RenderStateManager.h"
#include "DynamicConstantBuffer.h"
#include "DynamicBatchManager.h"

#include <fstream>

#include "../Misc/mmgr.h"

namespace engine
{
	Engine engine;

	void Engine::Initialize()
	{
		HRESULT hresult;

#ifdef _DEBUG
		D3D10_DRIVER_TYPE driver_type = D3D10_DRIVER_TYPE_HARDWARE;
		HMODULE software_handle = 0;

		if(driver_type == D3D10_DRIVER_TYPE_SOFTWARE) {
			software_handle = LoadLibrary(L"D3D10WARP_beta.dll");
		}

		if((hresult = D3D10CreateDevice(0, D3D10_DRIVER_TYPE_HARDWARE, software_handle, D3D10_CREATE_DEVICE_DEBUG|D3D10_CREATE_DEVICE_SWITCH_TO_REF, 
			D3D10_SDK_VERSION, &d3d10_device)) != S_OK)
			throw Exception(L"Engine: D3D10CreateDevice failed: " + GetD3D10Error(hresult));
#else
		if((hresult = D3D10CreateDevice(0,D3D10_DRIVER_TYPE_HARDWARE,0,0,D3D10_SDK_VERSION,&d3d10_device)) != S_OK)
			throw Exception(L"Engine: D3D10CreateDevice failed: " + GetD3D10Error(hresult));
#endif

		if((hresult = CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)(&dxgi_factory))) != S_OK)
			throw Exception(L"Engine: CreateDXGIFactory failed: " + GetD3D10Error(hresult));		

		renderer = new Renderer();
		render_state_manager = new RenderStateManager();
		may_change_every_frame = new DynamicConstantBuffer(d3d10_device, 1*sizeof(Matrix44) + 4*sizeof(Vector4) + 2*sizeof(int));		
		dynamic_batch_manager = new DynamicBatchManager(d3d10_device);

	}

	void Engine::Deinitalize()
	{
		delete dynamic_batch_manager;
		delete may_change_every_frame;
		delete render_state_manager;
		delete renderer;

		SafeRelease(dxgi_factory);
		SafeRelease(d3d10_device);
	}

	SwapChain *Engine::CreateSwapChain(HWND hwnd,unsigned int sample_count) const
	{
		return new SwapChain(dxgi_factory,hwnd,sample_count);
	}

	void Engine::DestroySwapChain(SwapChain *swap_chain) const
	{
		delete swap_chain;
	}

	void Engine::SetCamera(Camera *camera) 
	{ 
		this->camera = camera; 

		camera->Update();

		Matrix44 mvp = camera->GetViewProjectionMatrix();		
		
		__declspec(align(1)) struct {
			Matrix44 mvp_transpose;			
			Vector4 camera_position;
			Vector4 camera_left;
			Vector4 camera_up;
			Vector4 camera_forward;
			int viewport_x;
			int viewport_y;
		} constants;

		float scaley = tan(camera->GetFov()*0.5f) * camera->GetZNear();
		float scalex = scaley * camera->GetAspectRatio();

		constants.mvp_transpose = MatrixTranspose(mvp);		
		constants.camera_position = camera->GetPosition();
		constants.camera_left = camera->GetLeftVector() * scalex;
		constants.camera_up = camera->GetUpVector() * scaley;
		constants.camera_forward = camera->GetForwardVector() * -camera->GetZNear();
		constants.viewport_x = (int)camera->GetViewportX();
		constants.viewport_y = (int)camera->GetViewportY();

		may_change_every_frame->Fill(&constants);			

		ID3D10Buffer *buffers[] = { may_change_every_frame->GetConstantBuffer() };
		d3d10_device->VSSetConstantBuffers(0,1,buffers);
		d3d10_device->GSSetConstantBuffers(0,1,buffers);
		d3d10_device->PSSetConstantBuffers(0,1,buffers);
	}

#ifdef _DEBUG
	void Engine::ToggleRefrast()
	{
		ID3D10SwitchToRef *switch_to_ref = 0;
		d3d10_device->QueryInterface(__uuidof(switch_to_ref), (void**)&switch_to_ref);
		switch_to_ref->SetUseRef(!switch_to_ref->GetUseRef());
		SafeRelease(switch_to_ref);
	}
#endif
}