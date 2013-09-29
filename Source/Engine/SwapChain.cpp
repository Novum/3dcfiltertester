#include "SwapChain.h"
#include "Engine.h"
#include "Exception.h"
#include "D3DHelper.h"

namespace engine
{
	SwapChain::SwapChain(IDXGIFactory *dxgi_factory,HWND hwnd,unsigned int sample_count) 
		: RenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM,sample_count), dxgi_factory(dxgi_factory), hwnd(hwnd), swap_chain(0)
	{
		Recreate();
	}

	SwapChain::~SwapChain()
	{ 		
		SafeRelease(swap_chain);
	}

	void SwapChain::Recreate()
	{
		ID3D10Device *d3d10_device = engine.GetDevice();
		HRESULT hresult;
		RECT rect;
		DXGI_SWAP_CHAIN_DESC swap_chain_desc;		

		SafeRelease(render_target_view);
		SafeRelease(color_buffer);
		SafeRelease(swap_chain);		

		GetClientRect(hwnd,&rect);
		width = rect.right-rect.left;
		height = rect.bottom-rect.top;
		if(width==0 || height==0) return;

		ZeroMemory(&swap_chain_desc,sizeof(swap_chain_desc));
		swap_chain_desc.BufferCount = 1;
		swap_chain_desc.BufferDesc.Width = width;
		swap_chain_desc.BufferDesc.Height = height;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.OutputWindow = hwnd;
		swap_chain_desc.SampleDesc.Count = sample_count;
		swap_chain_desc.SampleDesc.Quality = 0;
		swap_chain_desc.Windowed = true;

		if((hresult = dxgi_factory->CreateSwapChain(d3d10_device,&swap_chain_desc,&swap_chain)) != S_OK)
			throw Exception(L"SwapChain: CreateSwapChain failed: " + GetD3D10Error(hresult));

		if((hresult = swap_chain->GetBuffer(0,__uuidof(ID3D10Texture2D),(LPVOID*)&color_buffer)) != S_OK)
			throw Exception(L"SwapChain: GetBuffer failed: " + GetD3D10Error(hresult));

		D3D10_RENDER_TARGET_VIEW_DESC render_target_view_desc;
		ZeroMemory(&render_target_view_desc,sizeof(render_target_view_desc));
		render_target_view_desc.Format = swap_chain_desc.BufferDesc.Format;			
		if(sample_count > 1) render_target_view_desc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DMS;
		else render_target_view_desc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;

		if((hresult = d3d10_device->CreateRenderTargetView(color_buffer,&render_target_view_desc,&render_target_view)) != S_OK)
			throw Exception(L"SwapChain: CreateSwapChainView failed: " + GetD3D10Error(hresult));		
	}
}