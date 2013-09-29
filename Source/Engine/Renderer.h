#pragma once

#include "RenderStateManager.h"
#include "LinearMath.h"
#include "ProbeObject.h"

#include <D3D10_1.h>
#include <memory>

namespace engine
{
	class StaticVertexBuffer;
	class StaticConstantBuffer;
	class SwapChain;
	class RenderTarget;
	class Texture;
	class DynamicConstantBuffer;	
	class FontRenderer;

	__declspec(align(1)) struct FilterParameterConstants
	{
		Vector2 texture_size;
		float max_anisotropy;
		float lod_bias;
		Vector2 texcoord_translation;
		float tex_scale;
		float filter_mode;
		int do_lerp;
		int gamma_correction;
	};	

	class Renderer
	{
	public:
		enum RenderMode { render_tmu, render_alu, render_split };

		Renderer();
		~Renderer();

		void Render(float timestep);		
		void SetSwapChain(SwapChain *swap_chain);		
		void SetProbeObject(ProbeObject *probe_object) { 
			this->probe_object = probe_object; 
		}
		ProbeObject *GetProbeObject() const { return probe_object; }
		void CreateShaders();		
		void LoadTexture(std::wstring file);		
		void SetRenderMode(RenderMode mode) { render_mode = mode; }
		void ToggleLinearInterpolation();
		void SetMaxAnisotropy(unsigned int value);
		void SetLODBias(float bias);
		void SetFilterMode(float mode);
		void SetTexcoordTranslation(float x, float y) { 
			filter_parameter_constants.texcoord_translation = Vector2(x, y); 
		}
		void SetTextureMovement(float x, float y) { 
			texture_movement = Vector2(x, y); 
		}
		void SetTextureScale(float scale);		

		void SetGammaCorrection(const bool state);
		bool GetGammaCorrection() const { return filter_parameter_constants.gamma_correction != 0; }

	private:
		void UpdateFilterParameterConstantBuffer();		

		// Resources
		ProbeObject *probe_object;

		ID3D10SamplerState *aniso_sampler_states[5];

		GlobalRenderState global_render_state;
		GlobalRenderState scissor_global_render_state;
		RenderState tmu_filtering_render_state;
		RenderState alu_filtering_render_state;
		RenderState separator_render_state;
		
		DynamicConstantBuffer *filter_parameter_constant_buffer;
		FilterParameterConstants filter_parameter_constants;

		std::wstring texture_file;
		std::unique_ptr<Texture> texture;

		FontRenderer *font_renderer;
	
		// States
		RenderMode render_mode;
		bool do_lerp;
		Vector2 texture_movement;
		unsigned int viewport_width;
		unsigned int viewport_height;
	};
}
