#pragma once

#include <D3D10_1.h>

namespace engine
{
	/*
	* Provides a depth stencil buffer to render to with D3D10. 
	* Recreate should be called everytime the viewport is resized.
	*/
	class DepthStencilBuffer
	{
	public:
		DepthStencilBuffer(unsigned int sample_count) : sample_count(sample_count), depth_stencil_texture(0), depth_stencil_view(0) {}		
		virtual ~DepthStencilBuffer();		
		unsigned int GetWidth() const { return width; }
		unsigned int GetHeight() const { return height; }
		ID3D10DepthStencilView *GetDepthStencilView() { return depth_stencil_view; }
		void ClearDepth(float depth=1.0f,unsigned char stencil=0);
		void Recreate(unsigned int width,unsigned int height);
	protected:
		unsigned int width, height;		
		ID3D10DepthStencilView *depth_stencil_view;
		ID3D10Texture2D *depth_stencil_texture;	
		unsigned int sample_count;
	};
}
