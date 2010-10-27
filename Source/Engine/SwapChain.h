#pragma once

#include "RenderTarget.h"
#include "DepthStencilBuffer.h"

#include <D3D10_1.h>

namespace engine
{
	/*
	* Provides a swap chain
	* Recreate should be called everytime the viewport is resized.
	*/
	class SwapChain : public RenderTarget
	{
	public:
		SwapChain(IDXGIFactory *dxgi_factory,HWND hwnd,unsigned int sample_count);
		virtual ~SwapChain();		
		void Recreate();		
		void Present() { if(swap_chain) swap_chain->Present(0,0); }
	private:				
		IDXGISwapChain *swap_chain;		
		IDXGIFactory *dxgi_factory;	
		HWND hwnd;		
	};
}