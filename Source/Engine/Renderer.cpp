#include "Renderer.h"
#include "Engine.h"
#include "D3DHelper.h"
#include "Exception.h"
#include "StaticBuffers.h"
#include "SwapChain.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "DynamicConstantBuffer.h"
#include "FontRenderer.h"

namespace engine
{
	Renderer::Renderer() : probe_object(0), render_mode(render_tmu), do_lerp(true)
	{
		RenderStateManager &render_state_manager = engine.GetRenderStateManager();
		ID3D10Device *d3d10_device = engine.GetDevice();

		// Create shaders
		CreateShaders();

		// vertex buffers
		tmu_filtering_render_state.vertex_buffer_offsets.resize(1);
		tmu_filtering_render_state.vertex_buffers.resize(1);
		tmu_filtering_render_state.vertex_buffer_strides.push_back(sizeof(ProbeVertex));	

		alu_filtering_render_state.vertex_buffer_offsets.resize(1);
		alu_filtering_render_state.vertex_buffers.resize(1);
		alu_filtering_render_state.vertex_buffer_strides.push_back(sizeof(ProbeVertex));	

		// Topologies		
		tmu_filtering_render_state.topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		alu_filtering_render_state.topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		separator_render_state.topology = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;

		// Depth stencil states
		D3D10_DEPTH_STENCIL_DESC depth_stencil_desc = render_state_manager.GetDefaultDepthStencilDesc();
		depth_stencil_desc.DepthEnable = false;
		
		d3d10_device->CreateDepthStencilState(&depth_stencil_desc, &tmu_filtering_render_state.depth_stencil_state);
		d3d10_device->CreateDepthStencilState(&depth_stencil_desc, &alu_filtering_render_state.depth_stencil_state);
		d3d10_device->CreateDepthStencilState(&depth_stencil_desc, &separator_render_state.depth_stencil_state);		
		
		// Blend states		
		d3d10_device->CreateBlendState(&render_state_manager.GetDefaultBlendDesc(), &tmu_filtering_render_state.blend_state);
		d3d10_device->CreateBlendState(&render_state_manager.GetDefaultBlendDesc(), &alu_filtering_render_state.blend_state);
		d3d10_device->CreateBlendState(&render_state_manager.GetDefaultBlendDesc(), &separator_render_state.blend_state);

		// Shader resources		
		tmu_filtering_render_state.ps_resources.resize(1);
		alu_filtering_render_state.ps_resources.resize(1);

		// Sampler states
		ID3D10SamplerState *sampler_state;
		D3D10_SAMPLER_DESC sampler_desc = render_state_manager.GetDefaultSamplerDesc();
		
		for(unsigned int i=0; i<5; ++i) {
			sampler_desc.Filter = D3D10_FILTER_ANISOTROPIC;
			sampler_desc.MaxAnisotropy = 1 << i;
			d3d10_device->CreateSamplerState(&sampler_desc, &aniso_sampler_states[i]);
		}
		tmu_filtering_render_state.ps_sampler_states.push_back(aniso_sampler_states[0]);		
		
		sampler_desc.Filter = D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT;		
		sampler_desc.MaxAnisotropy = 1;
		d3d10_device->CreateSamplerState(&sampler_desc, &sampler_state);
		alu_filtering_render_state.ps_sampler_states.push_back(sampler_state);
		
		// Initialize global render states		
		d3d10_device->CreateRasterizerState(&render_state_manager.GetDefaultRasterizerDesc(), &global_render_state.rasterizer_state);		
		global_render_state.viewports.resize(1);
		global_render_state.render_targets.push_back(0);		

		D3D10_RASTERIZER_DESC rasterizer_desc = render_state_manager.GetDefaultRasterizerDesc();
		rasterizer_desc.ScissorEnable = true;
		d3d10_device->CreateRasterizerState(&rasterizer_desc, &scissor_global_render_state.rasterizer_state);
		scissor_global_render_state.scissor_rects.resize(1);

		// Initialize filter parameter dynamic constant buffer
		unsigned int cbuffer_size = sizeof(FilterParameterConstants);
		filter_parameter_constant_buffer = new DynamicConstantBuffer(d3d10_device, cbuffer_size);
		tmu_filtering_render_state.vs_constant_buffers.push_back(filter_parameter_constant_buffer->GetConstantBuffer());
		tmu_filtering_render_state.ps_constant_buffers.push_back(filter_parameter_constant_buffer->GetConstantBuffer());
		alu_filtering_render_state.vs_constant_buffers.push_back(filter_parameter_constant_buffer->GetConstantBuffer());
		alu_filtering_render_state.ps_constant_buffers.push_back(filter_parameter_constant_buffer->GetConstantBuffer());		

		// Initialize filter parameter constants
		filter_parameter_constants.texture_size = Vector2(0.0f, 0.0f);
		filter_parameter_constants.max_anisotropy = 1.0f;
		filter_parameter_constants.lod_bias = 0.0f;
		filter_parameter_constants.texcoord_translation = Vector2(0.0f, 0.0f);
		filter_parameter_constants.tex_scale = 1.0f;
		filter_parameter_constants.filter_mode = 0.0f;
		filter_parameter_constants.do_lerp = do_lerp;
		filter_parameter_constants.gamma_correction = true;
		filter_parameter_constant_buffer->Fill(&filter_parameter_constants);	
		texture_movement = Vector2(0.0f, 0.0f);

		// Load default texture
		LoadTexture(L"Resources/default.dds");

		// Create font renderer
		font_renderer = new FontRenderer(L"Resources/font");
	}

	Renderer::~Renderer()
	{
		delete font_renderer;

		delete filter_parameter_constant_buffer;

		SafeRelease(separator_render_state.blend_state);
		SafeRelease(separator_render_state.depth_stencil_state);
		SafeRelease(separator_render_state.vertex_shader);
		SafeRelease(separator_render_state.pixel_shader);

		SafeRelease(scissor_global_render_state.rasterizer_state);
		SafeRelease(global_render_state.rasterizer_state);

		SafeRelease(alu_filtering_render_state.ps_sampler_states[0]);
		SafeRelease(alu_filtering_render_state.blend_state);
		SafeRelease(alu_filtering_render_state.depth_stencil_state);		
		SafeRelease(alu_filtering_render_state.input_layout);
		SafeRelease(alu_filtering_render_state.vertex_shader);
		SafeRelease(alu_filtering_render_state.pixel_shader);
		
		for(int i=0; i<5; ++i) SafeRelease(aniso_sampler_states[i]);		
		SafeRelease(tmu_filtering_render_state.blend_state);
		SafeRelease(tmu_filtering_render_state.blend_state);
		SafeRelease(tmu_filtering_render_state.depth_stencil_state);		
		SafeRelease(tmu_filtering_render_state.pixel_shader);
	}

	void Renderer::Render(float timestep)
	{
		if(!probe_object) return;

		filter_parameter_constants.texcoord_translation[0] = 
			fmod(filter_parameter_constants.texcoord_translation[0] 
			+ timestep * (texture_movement[0] / filter_parameter_constants.tex_scale), 1.0f);
		filter_parameter_constants.texcoord_translation[1] = 
			fmod(filter_parameter_constants.texcoord_translation[1] 
			+ timestep * (texture_movement[1] / filter_parameter_constants.tex_scale), 1.0f);

		filter_parameter_constant_buffer->Fill(&filter_parameter_constants);

		ID3D10Device *d3d10_device = engine.GetDevice();
		RenderStateManager &render_state_manager = engine.GetRenderStateManager();

		// Set probe object vertex buffer
		tmu_filtering_render_state.vertex_buffers[0] = 
			probe_object->GetVertexBuffer().GetVertexBuffer(tmu_filtering_render_state.vertex_buffer_offsets[0]);
		alu_filtering_render_state.vertex_buffers[0] = 
			probe_object->GetVertexBuffer().GetVertexBuffer(alu_filtering_render_state.vertex_buffer_offsets[0]);

		render_state_manager.SetGlobalRenderState(global_render_state);	

		if(render_mode == render_tmu) {
			// Render with TMU filtering
			render_state_manager.SetRenderState(tmu_filtering_render_state);			
			d3d10_device->Draw(probe_object->GetVertexCount(), 0);

			font_renderer->Render(10, 10, "TMU Rendering");
		}
		else if(render_mode == render_alu) {
			// Render with ALU filtering
			render_state_manager.SetRenderState(alu_filtering_render_state);			
			d3d10_device->Draw(probe_object->GetVertexCount(), 0);	

			font_renderer->Render(10, 10, "ALU Rendering");
		}
		else {			
			// Render both TMU and ALU filtering splitted
			D3D10_RECT scissor_rect;

			scissor_rect.left = 0; 
			scissor_rect.top = 0;
			scissor_rect.right = viewport_width / 2;	
			scissor_rect.bottom = viewport_height; 			
			scissor_global_render_state.scissor_rects[0] = scissor_rect;

			// Render with TMU filtering
			render_state_manager.SetGlobalRenderState(scissor_global_render_state);				
			render_state_manager.SetRenderState(tmu_filtering_render_state);
			d3d10_device->Draw(probe_object->GetVertexCount(), 0);

			scissor_rect.left = viewport_width / 2;
			scissor_rect.top = 0;
			scissor_rect.right = viewport_width;	
			scissor_rect.bottom = viewport_height; 			
			scissor_global_render_state.scissor_rects[0] = scissor_rect;

			// Render with ALU filtering
			render_state_manager.SetGlobalRenderState(scissor_global_render_state);	
			render_state_manager.SetRenderState(alu_filtering_render_state);
			d3d10_device->Draw(probe_object->GetVertexCount(), 0);

			font_renderer->Render(10, 10, "TMU Rendering");
			font_renderer->Render(viewport_width / 2 + 10, 10, "ALU Rendering");

			// Draw separator line
			render_state_manager.SetRenderState(separator_render_state);
			d3d10_device->Draw(2, 0);
		}
	}

	void Renderer::SetSwapChain(SwapChain *swap_chain)
	{
		global_render_state.render_targets[0] = swap_chain;
		
		D3D10_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = viewport_width = swap_chain->GetWidth();
		viewport.Height = viewport_height = swap_chain->GetHeight();
		viewport.MinDepth = 0.0f;		
		viewport.MaxDepth = 1.0f;
		
		global_render_state.viewports[0] = viewport;
	}

	void Renderer::CreateShaders()
	{
		HRESULT hresult;
		ID3D10Device *d3d10_device = engine.GetDevice();
		
		char *vs_buffer, *ps_buffer;
		unsigned int vs_size, ps_size;
		
		// Load vertex shader
		ID3D10VertexShader *vertex_shader = CreateVertexShaderFromFile("VertexShader", vs_buffer, vs_size);
		
		// Create input layout
		D3D10_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};
		
		ID3D10InputLayout *input_layout;
		if((hresult = d3d10_device->CreateInputLayout(input_element_desc, 2, vs_buffer, vs_size, &input_layout)) != S_OK)
			throw Exception(L"Renderer: CreateInputLayout failed: " + GetD3D10Error(hresult));

		delete[] vs_buffer;

		// Load TMU filtering pixel shader and assign vertex shader and input layout
		tmu_filtering_render_state.pixel_shader = CreatePixelShaderFromFile("TMUFilteringPS", ps_buffer, ps_size);
		delete[] ps_buffer;
		tmu_filtering_render_state.vertex_shader = vertex_shader;
		tmu_filtering_render_state.input_layout = input_layout;

		// Load TMU filtering pixel shader and assign vertex shader and input layout
		alu_filtering_render_state.pixel_shader = CreatePixelShaderFromFile("ALUFilteringPS", ps_buffer, ps_size);
		delete[] ps_buffer;
		alu_filtering_render_state.vertex_shader = vertex_shader;
		alu_filtering_render_state.input_layout = input_layout;

		// Load separator line shaders
		separator_render_state.vertex_shader = CreateVertexShaderFromFile("SeparatorVS", vs_buffer, vs_size);
		delete[] vs_buffer;
		separator_render_state.pixel_shader = CreatePixelShaderFromFile("SeparatorPS", ps_buffer, ps_size);
		delete[] ps_buffer;
	}

	void Renderer::LoadTexture(std::wstring file)
	{
		if(texture) {
			// Unbind texture from shader
			tmu_filtering_render_state.ps_resources[0] = 0;
			engine.GetRenderStateManager().SetRenderState(tmu_filtering_render_state);
		}

		// Load texture
		texture.reset(new Texture(file, filter_parameter_constants.gamma_correction != 0));

		// Set resources
		tmu_filtering_render_state.ps_resources[0] = texture->GetResourceView();
		alu_filtering_render_state.ps_resources[0] = texture->GetResourceView();

		filter_parameter_constants.texture_size = Vector2((float)texture->GetWidth(), (float)texture->GetHeight());
		filter_parameter_constant_buffer->Fill(&filter_parameter_constants);

		texture_file = file;
	}

	void Renderer::SetMaxAnisotropy(unsigned int value)
	{
		filter_parameter_constants.max_anisotropy = (float)value;
		filter_parameter_constant_buffer->Fill(&filter_parameter_constants);

		switch(value) {
			case 1: 
				tmu_filtering_render_state.ps_sampler_states[0] = aniso_sampler_states[0];
				break;
			case 2: 
				tmu_filtering_render_state.ps_sampler_states[0] = aniso_sampler_states[1];
				break;
			case 4: 
				tmu_filtering_render_state.ps_sampler_states[0] = aniso_sampler_states[2];
				break;
			case 8: 
				tmu_filtering_render_state.ps_sampler_states[0] = aniso_sampler_states[3];
				break;
			case 16: 
				tmu_filtering_render_state.ps_sampler_states[0] = aniso_sampler_states[4];
				break;
			default:
				tmu_filtering_render_state.ps_sampler_states[0] = aniso_sampler_states[4];
		}		
	}

	void Renderer::SetLODBias(float bias)
	{
		filter_parameter_constants.lod_bias = bias;
		filter_parameter_constant_buffer->Fill(&filter_parameter_constants);
	}

	void Renderer::SetFilterMode(float mode)
	{
		filter_parameter_constants.filter_mode = mode;
		filter_parameter_constant_buffer->Fill(&filter_parameter_constants);
	}

	void Renderer::ToggleLinearInterpolation()
	{
		do_lerp = !do_lerp;
		filter_parameter_constants.do_lerp = do_lerp;
		filter_parameter_constant_buffer->Fill(&filter_parameter_constants);
	}

	void Renderer::SetTextureScale(float scale)
	{
		filter_parameter_constants.tex_scale = scale;
		filter_parameter_constant_buffer->Fill(&filter_parameter_constants);
	}

	void Renderer::SetGammaCorrection(const bool state)
	{
		filter_parameter_constants.gamma_correction = state;
		LoadTexture(texture_file);
	}
}