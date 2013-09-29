#pragma once

#include <D3D10_1.h>
#include <string>

namespace engine
{
	/*
	* Manages a 2D texture including a shader resource view on it
	*/
	class Texture
	{
	public:
		Texture(std::wstring filename, bool srgb);
		~Texture();
		
		ID3D10ShaderResourceView *GetResourceView() const { return resource_view; }
		std::string GetFormatString();
		unsigned int GetWidth() const { return width; }
		unsigned int GetHeight() const { return height; }
	private:
		DXGI_FORMAT MakeSRGB(DXGI_FORMAT format) const;
		bool ConvertibleToSRGB(DXGI_FORMAT format) const;

		ID3D10Texture2D *texture;
		ID3D10ShaderResourceView *resource_view;
		unsigned int width, height;
	};
}
