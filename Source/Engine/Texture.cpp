#include "Texture.h"
#include "D3DHelper.h"
#include "Exception.h"
#include "Engine.h"

#include <D3DX10.h>

// Must be included after D3DX10.h
#include "../Misc/mmgr.h"

namespace engine
{
	// Loads a texture from a file and creates the resource view
	Texture::Texture(std::wstring filename, bool srgb, bool color_mips)
	{
		HRESULT hresult;
		ID3D10Device *d3d10_device = engine.GetDevice();

		D3DX10_IMAGE_INFO image_info;
		if((hresult = D3DX10GetImageInfoFromFile(filename.c_str(), 0, &image_info, 0)) != S_OK)
			throw Exception(L"Texture: D3DX10GetImageInfoFromFile failed: " + GetD3D10Error(hresult));

		ID3D10Resource *resource;
		D3DX10_IMAGE_LOAD_INFO load_info;
		ZeroMemory(&load_info, sizeof(D3DX10_IMAGE_LOAD_INFO));
		load_info.Width = D3DX_FROM_FILE;
		load_info.Height = D3DX_FROM_FILE;
		load_info.Depth = D3DX_FROM_FILE;
		load_info.FirstMipLevel = D3DX_FROM_FILE;
		load_info.MipLevels = D3DX_FROM_FILE;
		load_info.Usage = D3D10_USAGE_IMMUTABLE;
		load_info.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		load_info.CpuAccessFlags = 0;
		load_info.MiscFlags = 0;
		load_info.Format = image_info.Format;
		load_info.Filter = D3DX10_FILTER_NONE;
		load_info.MipFilter = D3DX10_FILTER_NONE;

		if((hresult = D3DX10CreateTextureFromFile(d3d10_device,filename.c_str(), &load_info, 0, &resource, 0)) != S_OK) 
			throw Exception(L"Texture: D3DX10CreateTextureFromFile failed: " + GetD3D10Error(hresult));

		D3D10_RESOURCE_DIMENSION dim;
		resource->GetType(&dim);

		if(dim != D3D10_RESOURCE_DIMENSION_TEXTURE2D)
			throw Exception(L"Texture: Only 2D textures are supported currently");
		
		if((hresult = resource->QueryInterface(__uuidof(ID3D10Texture2D),(LPVOID*)&texture)) != S_OK)
			throw Exception(L"Texture: QueryInterface failed: " + GetD3D10Error(hresult));

		// If convertible to sRGB copy resource to SRGB format, because D3DX10CreateTextureFromFile doesn't handle it.
		if(srgb && ConvertibleToSRGB(image_info.Format)) {
			ID3D10Texture2D* copy_tex = NULL;
			D3D10_TEXTURE2D_DESC copy_desc;
			texture->GetDesc(&copy_desc);			
			copy_desc.Format = MakeSRGB(copy_desc.Format);
			if((hresult = d3d10_device->CreateTexture2D(&copy_desc, NULL, &copy_tex)) != S_OK)
				throw Exception(L"Texture: CreateTexture2D failed: " + GetD3D10Error(hresult));

			d3d10_device->CopyResource(copy_tex, texture);
			texture->Release();

			texture = copy_tex;			
		}

		D3D10_TEXTURE2D_DESC tex_desc;
		texture->GetDesc(&tex_desc);	

		width = tex_desc.Width;
		height = tex_desc.Height;

		D3D10_SHADER_RESOURCE_VIEW_DESC srv_desc;
		ZeroMemory(&srv_desc,sizeof(srv_desc));
		if(srgb) srv_desc.Format = MakeSRGB(tex_desc.Format);
		else srv_desc.Format = tex_desc.Format;
		srv_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = tex_desc.MipLevels;
		srv_desc.Texture2D.MostDetailedMip = 0;

		if((hresult = d3d10_device->CreateShaderResourceView(texture, &srv_desc, &resource_view)) != S_OK)
			throw Exception(L"Texture: CreateShaderResourceView failed: " + GetD3D10Error(hresult));

		SafeRelease(resource);
	}

	Texture::~Texture()
	{
		SafeRelease(resource_view);
		SafeRelease(texture);
	}

	std::string Texture::GetFormatString()
	{
		D3D10_TEXTURE2D_DESC tex_desc;
		texture->GetDesc(&tex_desc);
		return GetDXGIFormatString(tex_desc.Format);
	}

	DXGI_FORMAT Texture::MakeSRGB(DXGI_FORMAT format) const
	{
		switch(format) {
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM_SRGB;
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM_SRGB;
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM_SRGB;
		};

		return format;
	}

	bool Texture::ConvertibleToSRGB(DXGI_FORMAT format) const
	{
		switch(format) {
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:		
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:			
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
			return true;
		};

		return false;
	}
}