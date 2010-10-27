#pragma once

#include "RenderStateManager.h"

#include <string>
#include <D3D10_1.h>
#include <D3DX10.h>

namespace engine
{
	class Texture;	
	class StaticConstantBuffer;

	/*
	* Class to render textr from a font resource generated
	* by Anglecode's bitmap font generator
	*/
	class FontRenderer
	{
		struct CharacterInfo {
			float x,y;
			float tex_width,tex_height;
			float width,height;
			float xoffset,yoffset;
			float xadvance;
		};

		struct CharacterVertex {
			float position[2];
			float texcoords[2];			
		};
	public:
		FontRenderer(std::wstring font,float red=1.0f,float green=1.0f,float blue=1.0f);
		~FontRenderer();		
		void Render(int x,int y,std::string text,int max_length=0);
	private:
		void LoadCharInfos(std::wstring &font);
		void LoadTexture(std::wstring &font);		

		GlobalRenderState global_render_state;
		RenderState render_state;
		StaticConstantBuffer *constant_buffer;

		Texture *texture;
		CharacterInfo *char_infos[256];		
		
		int ppp_length;
	};
}