#include "RenderStateManager.h"
#include "RenderTarget.h"
#include "DepthStencilBuffer.h"
#include "Engine.h"

namespace engine
{
	void RenderState::SetChanges(RenderState &old_state)
	{
		ID3D10Device *d3d10_device = engine.GetDevice();

		// Input Assembler
		if(old_state.input_layout != input_layout) d3d10_device->IASetInputLayout(input_layout);
		if(old_state.topology != topology) d3d10_device->IASetPrimitiveTopology(topology);
		if(index_buffer && (old_state.index_buffer != index_buffer || old_state.index_buffer_format != index_buffer_format || old_state.index_buffer_offset != index_buffer_offset)) 
			d3d10_device->IASetIndexBuffer(index_buffer, index_buffer_format, index_buffer_offset);		
		if(vertex_buffers.size() && (old_state.vertex_buffers != vertex_buffers || 	old_state.vertex_buffer_offsets != vertex_buffer_offsets || 
			old_state.vertex_buffer_strides != vertex_buffer_strides)) {
			d3d10_device->IASetVertexBuffers(0, (unsigned int)vertex_buffers.size(), &vertex_buffers[0], &vertex_buffer_strides[0], &vertex_buffer_offsets[0]);
		}

		// Depth stencil
		if(old_state.depth_stencil_state != depth_stencil_state || old_state.stencil_ref != stencil_ref) 
			d3d10_device->OMSetDepthStencilState(depth_stencil_state, stencil_ref);

		// Blend
		if(old_state.blend_state != blend_state || old_state.blend_factor != blend_factor || sample_mask != sample_mask) 
			d3d10_device->OMSetBlendState(blend_state, &blend_factor[0], sample_mask);

		// Samplers
		if(vs_sampler_states.size() && old_state.vs_sampler_states != vs_sampler_states) 
			d3d10_device->VSSetSamplers(0, (unsigned int)vs_sampler_states.size(), &vs_sampler_states[0]);
		if(gs_sampler_states.size() && old_state.gs_sampler_states != gs_sampler_states) 
			d3d10_device->GSSetSamplers(0, (unsigned int)gs_sampler_states.size(), &gs_sampler_states[0]);
		if(ps_sampler_states.size() && old_state.ps_sampler_states != ps_sampler_states) 
			d3d10_device->PSSetSamplers(0, (unsigned int)ps_sampler_states.size(), &ps_sampler_states[0]);

		// Resources
		if(vs_resources.size() && old_state.vs_resources != vs_resources) 
			d3d10_device->VSSetShaderResources(0, (unsigned int)vs_resources.size(), &vs_resources[0]);
		if(gs_resources.size() && old_state.gs_resources != gs_resources) 
			d3d10_device->GSSetShaderResources(0, (unsigned int)gs_resources.size(), &gs_resources[0]);
		if(ps_resources.size() && old_state.ps_resources != ps_resources) 
			d3d10_device->PSSetShaderResources(0, (unsigned int)ps_resources.size(), &ps_resources[0]);
	
		// Constants
		if(vs_constant_buffers.size() && old_state.vs_constant_buffers != vs_constant_buffers) 
			d3d10_device->VSSetConstantBuffers(1, (unsigned int)vs_constant_buffers.size(), &vs_constant_buffers[0]);
		if(gs_constant_buffers.size() && old_state.gs_constant_buffers != gs_constant_buffers) 
			d3d10_device->GSSetConstantBuffers(1, (unsigned int)gs_constant_buffers.size(), &gs_constant_buffers[0]);
		if(ps_constant_buffers.size() && old_state.ps_constant_buffers != ps_constant_buffers) 
			d3d10_device->PSSetConstantBuffers(1, (unsigned int)ps_constant_buffers.size(), &ps_constant_buffers[0]);

		// Stream out
		if(so_buffers.size() && old_state.so_buffers != so_buffers)
			d3d10_device->SOSetTargets((unsigned int)so_buffers.size(), &so_buffers[0], &so_buffer_offsets[0]);

		// Shaders
		if(old_state.vertex_shader != vertex_shader) d3d10_device->VSSetShader(vertex_shader);
		if(old_state.geometry_shader != geometry_shader) d3d10_device->GSSetShader(geometry_shader);
		if(old_state.pixel_shader != pixel_shader) d3d10_device->PSSetShader(pixel_shader);
	}

	void GlobalRenderState::SetChanges(GlobalRenderState &old_state)
	{
		ID3D10Device *d3d10_device = engine.GetDevice();

		// Rasterizer
		if(old_state.rasterizer_state != rasterizer_state) d3d10_device->RSSetState(rasterizer_state);
		if(viewports.size()) d3d10_device->RSSetViewports((unsigned int)viewports.size(), &viewports[0]);
		if(scissor_rects.size()) d3d10_device->RSSetScissorRects((unsigned int)scissor_rects.size(), &scissor_rects[0]);
		
		ID3D10RenderTargetView *render_target_views[D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT];
		for(unsigned int i=0;i<render_targets.size();++i) render_target_views[i] = render_targets[i] ? render_targets[i]->GetRenderTargetView() : 0;
		ID3D10DepthStencilView *depth_stencil_view = depth_stencil_buffer ? depth_stencil_buffer->GetDepthStencilView() : 0;
		if(render_targets.size()) d3d10_device->OMSetRenderTargets((unsigned int)render_targets.size(), &render_target_views[0], depth_stencil_view);
	}

	void RenderStateManager::SetRenderState(RenderState &state)
	{
		state.SetChanges(current_state);
		current_state = state;
	}

	void RenderStateManager::SetGlobalRenderState(GlobalRenderState &state)
	{
		state.SetChanges(current_global_state);
		current_global_state = state;
	}

	D3D10_RASTERIZER_DESC RenderStateManager::GetDefaultRasterizerDesc()
	{
		D3D10_RASTERIZER_DESC desc;
		desc.FillMode = D3D10_FILL_SOLID;
		desc.CullMode = D3D10_CULL_BACK;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;
		return desc;
	}

	D3D10_DEPTH_STENCIL_DESC RenderStateManager::GetDefaultDepthStencilDesc()
	{
		D3D10_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D10_COMPARISON_LESS;
		desc.StencilEnable = false;
		desc.StencilReadMask = D3D10_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
		desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		return desc;
	}

	D3D10_BLEND_DESC RenderStateManager::GetDefaultBlendDesc()
	{
		D3D10_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = false;
		desc.SrcBlend = D3D10_BLEND_ONE;
		desc.DestBlend = D3D10_BLEND_ZERO;
		desc.BlendOp = D3D10_BLEND_OP_ADD;
		desc.SrcBlendAlpha = D3D10_BLEND_ONE;
		desc.DestBlendAlpha = D3D10_BLEND_ZERO;
		desc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
		for(unsigned int i=0;i<8;++i) {
			desc.BlendEnable[i] = false;
			desc.RenderTargetWriteMask[i] = D3D10_COLOR_WRITE_ENABLE_ALL;
		}
		return desc;
	}

	D3D10_SAMPLER_DESC RenderStateManager::GetDefaultSamplerDesc()
	{
		D3D10_SAMPLER_DESC desc;
		desc.Filter = D3D10_FILTER_ANISOTROPIC;
		desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = FLT_MAX;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D10_COMPARISON_NEVER;		
		for(unsigned int i=0;i<4;++i) desc.BorderColor[i] = 0.0f;
		return desc;
	}
}