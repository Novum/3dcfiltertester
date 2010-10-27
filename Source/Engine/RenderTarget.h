#pragma once

#include <D3D10_1.h>

namespace engine
{
	/*
	* Provides a surface to render to with D3D10. 
	* Recreate should be called every time the viewport is resized.
	*/
	class RenderTarget
	{
	public:		
		RenderTarget(DXGI_FORMAT format, unsigned int sample_count) 
			: format(format), sample_count(sample_count), render_target_view(0), shader_resource_view(0), color_buffer(0) {}
		virtual ~RenderTarget();		

		void Recreate(unsigned int width, unsigned int height);
		unsigned int GetWidth() const { return width; }
		unsigned int GetHeight() const { return height; }
		DXGI_FORMAT GetFormat() const { return format; }
		void ClearColor(float red=0.0f, float green=0.0f, float blue=0.0f, float alpha=0.0f);
		void CopyToSystemRAM(unsigned char *dest);

		ID3D10RenderTargetView *GetRenderTargetView() const { return render_target_view; }
		ID3D10ShaderResourceView *GetShaderResourceView() const { return shader_resource_view; }
		ID3D10Texture2D *GetTexture() const { return color_buffer; }
	protected:
		unsigned int width, height;
		DXGI_FORMAT format;
		ID3D10RenderTargetView *render_target_view;	
		ID3D10ShaderResourceView *shader_resource_view;
		ID3D10Texture2D *color_buffer;		
		unsigned int sample_count;		
	};
}
