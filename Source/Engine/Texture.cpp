#include "Texture.h"
#include "D3DHelper.h"
#include "Exception.h"
#include "Engine.h"
#include "../Misc/DDSTextureLoader.h"

namespace engine
{
	// Loads a texture from a file and creates the resource view
	Texture::Texture(std::wstring filename, bool srgb, bool color_mips)
	{
		HRESULT hresult;
		ID3D10Device *d3d10_device = engine.GetDevice();

		ID3D10Resource *resource;
		if ((hresult = DirectX::CreateDDSTextureFromFile(d3d10_device, filename.c_str(), &resource, &resource_view)) != S_OK)
			throw Exception(L"DirectX::CreateDDSTextureFromFile failed: " + GetD3D10Error(hresult));

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