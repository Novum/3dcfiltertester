#include "FontRenderer.h"
#include "Exception.h"
#include "Engine.h"
#include "Texture.h"
#include "D3DHelper.h"
#include "DynamicBatchManager.h"
#include "StaticBuffers.h"

#include <fstream>

namespace engine
{
	FontRenderer::FontRenderer(std::wstring font,float red,float green,float blue)
	{
		LoadCharInfos(font);
		LoadTexture(font);		

		if(char_infos['.'])
			ppp_length = (int)(char_infos['.']->xadvance*3);
		else ppp_length = 0;

		HRESULT hresult;
		ID3D10Device *d3d10_device = engine.GetDevice();

		char *vs_buffer, *ps_buffer;
		unsigned int vs_size, ps_size;
		render_state.vertex_shader = CreateVertexShaderFromFile("FontVS", vs_buffer, vs_size);
		render_state.pixel_shader = CreatePixelShaderFromFile("FontPS", ps_buffer, ps_size);

		D3D10_INPUT_ELEMENT_DESC font_input_element_desc[] =
		{
			{ "SV_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		if((hresult = d3d10_device->CreateInputLayout(font_input_element_desc,2,vs_buffer,vs_size,&render_state.input_layout)) != S_OK)
			throw Exception(L"FontRenderer: CreateInputLayout failed: " + GetD3D10Error(hresult));

		delete[] vs_buffer;
		delete[] ps_buffer;

		float constant_data[4] = { red, green, blue, 0.0f };
		constant_buffer = new StaticConstantBuffer(&constant_data,4*sizeof(float));
		render_state.ps_constant_buffers.push_back(constant_buffer->GetConstantBuffer());
		
		// Initialize render states
		RenderStateManager &render_state_manager = engine.GetRenderStateManager();

		render_state.vertex_buffers.resize(1);
		render_state.vertex_buffer_offsets.resize(1);
		render_state.vertex_buffer_strides.push_back(sizeof(CharacterVertex));
		
		render_state.index_buffer_format = DXGI_FORMAT_R16_UINT;

		render_state.topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		d3d10_device->CreateRasterizerState(&render_state_manager.GetDefaultRasterizerDesc(),&global_render_state.rasterizer_state);

		D3D10_DEPTH_STENCIL_DESC depth_stencil_desc = render_state_manager.GetDefaultDepthStencilDesc();
		depth_stencil_desc.DepthEnable = false;		
		d3d10_device->CreateDepthStencilState(&depth_stencil_desc,&render_state.depth_stencil_state);

		d3d10_device->CreateBlendState(&render_state_manager.GetDefaultBlendDesc(),&render_state.blend_state);
		
		ID3D10SamplerState *sampler_state;
		D3D10_SAMPLER_DESC sampler_desc = render_state_manager.GetDefaultSamplerDesc();
		sampler_desc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
		d3d10_device->CreateSamplerState(&sampler_desc,&sampler_state);
		render_state.ps_sampler_states.push_back(sampler_state);

		render_state.ps_resources.push_back(texture->GetResourceView());
	}

	FontRenderer::~FontRenderer()
	{
		SafeRelease(render_state.vertex_shader);
		SafeRelease(render_state.pixel_shader);
		SafeRelease(render_state.ps_sampler_states[0]);
		SafeRelease(render_state.blend_state);
		SafeRelease(render_state.depth_stencil_state);
		SafeRelease(global_render_state.rasterizer_state);		

		delete constant_buffer;
		delete texture;

		for(unsigned int i=0;i<256;++i)
			delete char_infos[i];		
	}

	void FontRenderer::Render(int x,int y,std::string text,int max_length)
	{
		float current_x = (float)x, current_y = (float)y;
		int length = 0;
		int current_advance = 0;
		int ppp_left = 3;

		ID3D10Device *d3d10_device = engine.GetDevice();

		DynamicBatchManager &dynamic_batch_manager = engine.GetDynamicBatchManager();
		DynamicVertexBatch &dyn_vertex_batch = dynamic_batch_manager.NextDynamicVertexBatch((int)text.length()*4*sizeof(CharacterVertex));
		DynamicIndexBatch &dyn_index_batch = dynamic_batch_manager.NextDynamicIndexBatch((int)text.length()*6*sizeof(unsigned __int16));		

		CharacterVertex *vertices = (CharacterVertex*)dyn_vertex_batch.Map();
		unsigned __int16* indices = (unsigned __int16*)dyn_index_batch.Map();

		for(unsigned int i=0;i<text.length();++i) {
			CharacterInfo *char_info = char_infos[text[i]];
			
			if(max_length) {
				if(!ppp_left) break;			
				if(current_advance > max_length-ppp_length) {
					char_info = char_infos['.'];
					--ppp_left;
				}
			}

			if(!char_info) continue;

			unsigned int index_offset = i*6, vertex_offset = i*4;

			float left = current_x + char_info->xoffset;
			float top = current_y + char_info->yoffset;
			float right = left + char_info->width;
			float bottom = top + char_info->height;
			float tex_left = char_info->x;
			float tex_top = char_info->y;
			float tex_right = tex_left+char_info->tex_width;
			float tex_bottom = tex_top+char_info->tex_height;

			CharacterVertex new_vertices[4];
			new_vertices[0].position[0] = new_vertices[2].position[0] = left;
			new_vertices[0].position[1] = new_vertices[1].position[1] = top;
			new_vertices[1].position[0] = new_vertices[3].position[0] = right;
			new_vertices[2].position[1] = new_vertices[3].position[1] = bottom;	
			new_vertices[0].texcoords[0] = new_vertices[2].texcoords[0] = tex_left;
			new_vertices[0].texcoords[1] = new_vertices[1].texcoords[1] = tex_top;
			new_vertices[1].texcoords[0] = new_vertices[3].texcoords[0] = tex_right;
			new_vertices[2].texcoords[1] = new_vertices[3].texcoords[1] = tex_bottom;		

			vertices[vertex_offset] = new_vertices[0]; vertices[vertex_offset+1] = new_vertices[1];
			vertices[vertex_offset+2] = new_vertices[2]; vertices[vertex_offset+3] = new_vertices[3];
			indices[index_offset] = vertex_offset; indices[index_offset+1] = vertex_offset+1; indices[index_offset+2] = vertex_offset+2;
			indices[index_offset+3] = vertex_offset+1; indices[index_offset+4] = vertex_offset+3; indices[index_offset+5] = vertex_offset+2;

			current_x += char_info->xadvance;
			current_advance += (int)char_info->xadvance; 

			++length;
		}

		dyn_index_batch.Unmap();
		dyn_vertex_batch.Unmap();

		render_state.vertex_buffers[0] = dyn_vertex_batch.GetVertexBuffer(render_state.vertex_buffer_offsets[0]);		 
		render_state.index_buffer = dyn_index_batch.GetIndexBuffer(render_state.index_buffer_offset);		

		engine.GetRenderStateManager().SetGlobalRenderState(global_render_state);
		engine.GetRenderStateManager().SetRenderState(render_state);

		d3d10_device->DrawIndexed((unsigned int)text.length()*6,0,0);
	}

	void FontRenderer::LoadCharInfos(std::wstring &font)
	{
		std::ifstream filestream(font + L".fnt");

		char buffer[128];
		filestream.getline(buffer,128);

		unsigned int scale_w,scale_h,line_height,base;
		sscanf_s(buffer,"common lineHeight=%d base=%d scaleW=%d scaleH=%d",&line_height,&base,&scale_w,&scale_h);

		memset(char_infos,0,256*sizeof(CharacterInfo*));

		while(!filestream.eof()) {
			CharacterInfo char_info;

			unsigned int id;
			int x,y;
			int width,height;
			int xoffset,yoffset;
			int xadvance;

			filestream.getline(buffer,128);
			int test = sscanf_s(buffer,"char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d",
				&id,&x,&y,&width,&height,&xoffset,&yoffset,&xadvance);

			if(id>255 || test!=8) continue;

			char_info.x = (float)x/(float)scale_w;
			char_info.y = (float)y/(float)scale_h;
			char_info.width = (float)width;
			char_info.height = (float)height;
			char_info.tex_width = (float)width/(float)scale_w;
			char_info.tex_height = (float)height/(float)scale_h;
			char_info.xoffset = (float)xoffset;
			char_info.yoffset = (float)yoffset;
			char_info.xadvance = (float)xadvance;			

			if(!char_infos[id]) {
				char_infos[id] = new CharacterInfo;
				*char_infos[id] = char_info;
			}
		}

		filestream.close();
	}

	void FontRenderer::LoadTexture(std::wstring &font)
	{
		texture = new Texture(font + L".dds", false);	

		if(!texture) throw Exception(L"Could not load font resources");	
	}
}