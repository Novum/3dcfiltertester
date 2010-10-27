#include "DepthStencilBuffer.h"
#include "D3DHelper.h"
#include "Exception.h"
#include "Engine.h"

namespace engine
{
	DepthStencilBuffer::~DepthStencilBuffer()
	{ 
		SafeRelease(depth_stencil_view);
		SafeRelease(depth_stencil_texture);
	}

	void DepthStencilBuffer::Recreate(unsigned int width,unsigned int height)
	{
		this->width = width;
		this->height = height;

		ID3D10Device *d3d10_device = engine.GetDevice();
		HRESULT hresult;

		SafeRelease(depth_stencil_view);
		SafeRelease(depth_stencil_texture);

		D3D10_TEXTURE2D_DESC depth_texture_desc;
		D3D10_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;		

		ZeroMemory(&depth_texture_desc,sizeof(depth_texture_desc));
		depth_texture_desc.Width = width;
		depth_texture_desc.Height = height;
		depth_texture_desc.MipLevels = 1;
		depth_texture_desc.ArraySize = 1;
		depth_texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_texture_desc.SampleDesc.Count = sample_count;
		depth_texture_desc.SampleDesc.Quality = 0;
		depth_texture_desc.Usage = D3D10_USAGE_DEFAULT;
		depth_texture_desc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		depth_texture_desc.CPUAccessFlags = 0;
		depth_texture_desc.MiscFlags = 0;

		if((hresult = d3d10_device->CreateTexture2D(&depth_texture_desc,0,&depth_stencil_texture)) != S_OK)
			throw Exception(L"SwapChain: CreateTexture2D failed: " + GetD3D10Error(hresult));

		ZeroMemory(&depth_stencil_view_desc,sizeof(depth_stencil_view_desc));
		depth_stencil_view_desc.Format = depth_texture_desc.Format;
		if(sample_count > 1) depth_stencil_view_desc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMS;
		else depth_stencil_view_desc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;		

		if((hresult = d3d10_device->CreateDepthStencilView(depth_stencil_texture,&depth_stencil_view_desc,&depth_stencil_view)) != S_OK)
			throw Exception(L"SwapChain: CreateDepthStencilView failed: " + GetD3D10Error(hresult));
	}

	void DepthStencilBuffer::ClearDepth(float depth,unsigned char stencil)
	{		
		engine.GetDevice()->ClearDepthStencilView(depth_stencil_view, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL,depth,stencil);
	}
}