#pragma once

#include "BufferInterfaces.h"
#include "LinearMath.h"

#include <D3D10_1.h>
#include <vector>
#include <map>
#include <boost/tuple/tuple.hpp>
#include <functional>

namespace engine
{	
	class RenderTarget;
	class DepthStencilBuffer;

	// This contains states that are frequently changed and are sorted for while rendering
	struct RenderState
	{
		RenderState() : input_layout(0), depth_stencil_state(0), index_buffer(0),
			blend_state(0), pixel_shader(0), geometry_shader(0), vertex_shader(0), stencil_ref(0), 
			blend_factor(Vector4(0.0f,0.0f,0.0f,0.0f)), sample_mask(0xFFFFFFFF) {}
	
		void SetChanges(RenderState &old_state);
	
		// Input assembler states
		ID3D10InputLayout *input_layout;		
		std::vector<ID3D10Buffer*> vertex_buffers;
		std::vector<unsigned int> vertex_buffer_offsets;
		std::vector<unsigned int> vertex_buffer_strides;
		ID3D10Buffer *index_buffer;
		DXGI_FORMAT index_buffer_format;
		unsigned int index_buffer_offset;
		D3D10_PRIMITIVE_TOPOLOGY topology;

		// Depth stencil state
		ID3D10DepthStencilState *depth_stencil_state;
		unsigned int stencil_ref;

		// Blend state
		ID3D10BlendState *blend_state;
		Vector4 blend_factor;
		unsigned int sample_mask;

		// Sampler states		
		std::vector<ID3D10SamplerState*> vs_sampler_states;
		std::vector<ID3D10SamplerState*> gs_sampler_states;
		std::vector<ID3D10SamplerState*> ps_sampler_states;

		// Resources
		std::vector<ID3D10ShaderResourceView*> vs_resources;
		std::vector<ID3D10ShaderResourceView*> gs_resources;
		std::vector<ID3D10ShaderResourceView*> ps_resources;
		
		// Constants
		std::vector<ID3D10Buffer*> vs_constant_buffers;
		std::vector<ID3D10Buffer*> gs_constant_buffers;
		std::vector<ID3D10Buffer*> ps_constant_buffers;

		// Stream output states
		std::vector<ID3D10Buffer*> so_buffers;
		std::vector<unsigned int> so_buffer_offsets;

		// Shaders
		ID3D10VertexShader *vertex_shader;
		ID3D10GeometryShader *geometry_shader;
		ID3D10PixelShader *pixel_shader;
	};

	// This contains states that are rarely changed and are not part of the state sorting
	struct GlobalRenderState 
	{
		GlobalRenderState() : rasterizer_state(0), depth_stencil_buffer(0) {}

		void SetChanges(GlobalRenderState &old_state);

		// Rasterizer
		ID3D10RasterizerState *rasterizer_state;
		std::vector<D3D10_VIEWPORT> viewports;
		std::vector<D3D10_RECT> scissor_rects;

		// Output merger
		std::vector<RenderTarget*> render_targets;
		DepthStencilBuffer *depth_stencil_buffer;
	};

	class RenderStateManager
	{
	public:		
		void SetRenderState(RenderState &state);
		void SetGlobalRenderState(GlobalRenderState &state);

		D3D10_RASTERIZER_DESC GetDefaultRasterizerDesc();
		D3D10_DEPTH_STENCIL_DESC GetDefaultDepthStencilDesc();
		D3D10_BLEND_DESC GetDefaultBlendDesc();
		D3D10_SAMPLER_DESC GetDefaultSamplerDesc();
	private:
		RenderState current_state;
		GlobalRenderState current_global_state;
	};
}