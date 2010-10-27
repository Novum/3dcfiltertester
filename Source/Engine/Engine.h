#pragma once

#include "Camera.h"

#include <D3D10_1.h>

namespace engine
{
	class SwapChain;
	class Renderer;
	class RenderStateManager;
	class DynamicConstantBuffer;
	class DynamicBatchManager;

	class Engine
	{
	public:		
		void Initialize();
		void Deinitalize();

		ID3D10Device *GetDevice() const { return d3d10_device; }

		SwapChain *CreateSwapChain(HWND hwnd,unsigned int sample_count) const;
		void DestroySwapChain(SwapChain *swap_chain) const;

		Renderer &GetRenderer() const { return *renderer; }
		RenderStateManager &GetRenderStateManager() const { return *render_state_manager; }

		void SetCamera(Camera *camera);
		Camera &GetCamera() const { return *camera; }

		DynamicBatchManager &GetDynamicBatchManager() { return *dynamic_batch_manager; }

#ifdef _DEBUG
		void ToggleRefrast();
#endif
	private:
		ID3D10Device *d3d10_device;		
		IDXGIFactory *dxgi_factory;	

		Renderer *renderer;
		RenderStateManager *render_state_manager;
		Camera *camera;		

		DynamicConstantBuffer *may_change_every_frame;
		DynamicBatchManager *dynamic_batch_manager;
	};

	extern Engine engine;
}