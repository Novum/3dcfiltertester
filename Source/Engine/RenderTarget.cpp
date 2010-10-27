#include "RenderTarget.h"
#include "Engine.h"
#include "Exception.h"
#include "D3DHelper.h"

#include "../Misc/mmgr.h"

namespace engine
{
	RenderTarget::~RenderTarget()
	{ 
		SafeRelease(render_target_view);
		SafeRelease(color_buffer);
	}

	void RenderTarget::Recreate(unsigned int width, unsigned int height)
	{
		this->width = width;
		this->height = height;

		ID3D10Device *d3d10_device = engine.GetDevice();
		HRESULT hresult;

		SafeRelease(render_target_view);
		SafeRelease(color_buffer);

		D3D10_TEXTURE2D_DESC color_buffer_desc;
		D3D10_RENDER_TARGET_VIEW_DESC render_target_view_desc;
		D3D10_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;

		ZeroMemory(&color_buffer_desc,sizeof(color_buffer_desc));
		color_buffer_desc.Width = width;
		color_buffer_desc.Height = height;
		color_buffer_desc.MipLevels = 1;
		color_buffer_desc.ArraySize = 1;
		color_buffer_desc.Format = format;
		color_buffer_desc.SampleDesc.Count = sample_count;
		color_buffer_desc.SampleDesc.Quality = 0;
		color_buffer_desc.Usage = D3D10_USAGE_DEFAULT;
		color_buffer_desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		color_buffer_desc.CPUAccessFlags = 0;
		color_buffer_desc.MiscFlags = 0;		

		if((hresult = d3d10_device->CreateTexture2D(&color_buffer_desc,0,&color_buffer)) != S_OK) {
			throw Exception(L"RenderTarget: CreateTexture2D failed: " + GetD3D10Error(hresult));
		}

		ZeroMemory(&render_target_view_desc,sizeof(render_target_view_desc));
		render_target_view_desc.Format = color_buffer_desc.Format;
		if(sample_count > 1) render_target_view_desc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DMS;
		else render_target_view_desc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;				

		if((hresult = d3d10_device->CreateRenderTargetView(color_buffer,&render_target_view_desc,&render_target_view)) != S_OK) {
			throw Exception(L"RenderTarget: CreateRenderTargetView failed: " + GetD3D10Error(hresult));		
		}

		ZeroMemory(&shader_resource_view_desc,sizeof(shader_resource_view_desc));
		shader_resource_view_desc.Format = color_buffer_desc.Format;
		if(sample_count > 1) shader_resource_view_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DMS;
		else shader_resource_view_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MipLevels = 1;

		if((hresult = d3d10_device->CreateShaderResourceView(color_buffer,&shader_resource_view_desc,&shader_resource_view)) != S_OK) {
			throw Exception(L"RenderTarget: CreateShaderResourceView failed: " + GetD3D10Error(hresult));
		}
	}

	void RenderTarget::ClearColor(float red, float green, float blue, float alpha)
	{
		ID3D10Device *d3d10_device = engine.GetDevice();
		float clear_color[4] = { red, green, blue, alpha };
		d3d10_device->ClearRenderTargetView(render_target_view, clear_color);		
	}

	void RenderTarget::CopyToSystemRAM(unsigned char *dest)
	{
		if(sample_count != 1) {
			throw Exception(L"RenderTarget: Cannot copy multisampled render target to staging buffer");
		}		

		ID3D10Device *d3d10_device = engine.GetDevice();
		HRESULT hresult;

		// Create staging buffer
		D3D10_TEXTURE2D_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D10_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
		desc.MiscFlags = 0;

		ID3D10Texture2D *staging_buffer;

		if((hresult = d3d10_device->CreateTexture2D(&desc, 0, &staging_buffer)) != S_OK) {
			throw Exception(L"RenderTarget: CreateTexture2D failed: " + GetD3D10Error(hresult));
		}

		// Copy to staging buffer
		d3d10_device->CopyResource(staging_buffer, color_buffer);
		
		const unsigned int element_size = GetDXGIFormatSize(format);

		// Map staging buffer
		D3D10_MAPPED_TEXTURE2D staging_mapping;
		staging_buffer->Map(0, D3D10_MAP_READ, 0, &staging_mapping);

		// Copy each row to system memory
		for(int y=0; y<(int)height; ++y) {
			const unsigned int row_length = element_size * width;
			const unsigned int memory_row_offset = y * width * element_size;
			const unsigned int staging_row_offset = y * staging_mapping.RowPitch;

			memcpy(dest + memory_row_offset, (unsigned char*)staging_mapping.pData + staging_row_offset, row_length);			
		}

		// Unmap and release staging buffer
		staging_buffer->Unmap(0);
		SafeRelease(staging_buffer);
	}
}