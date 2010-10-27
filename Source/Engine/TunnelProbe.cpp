#include "TunnelProbe.h"

namespace engine
{
	static const unsigned int tunnel_segments = 20;
	static const unsigned int num_tunnel_sides = 256;
	static const unsigned int num_vertices = num_tunnel_sides*tunnel_segments*2*3;

	TunnelProbe::TunnelProbe()
	{
		const float pi = 3.141592f;		

		// Generate tunnel vertex buffer
		unsigned int vertex = 0;
		ProbeVertex vertices[num_vertices];

		const float angle_step = (1.0f/(float)num_tunnel_sides) * 2 * pi;
		const float tunnel_scale = 1.0f;
		const float tunnel_length = 200.0f ;
		const float segment_length = tunnel_length / (float)tunnel_segments;
		const float segment_min = -segment_length / 2.0f;
		const float segment_max = segment_length / 2.0f;

		const float tunnel_texcoord_scale = (tunnel_length/(float)tunnel_segments) * 3.0f;
		const float tunnel_texcoord_step = 2.0f/(float)num_tunnel_sides;
		const float segment_texcoord_step = tunnel_texcoord_scale / (float)tunnel_segments;

		for(unsigned int segment=0; segment<tunnel_segments; ++segment) {
			Vector3 segment_translation = Vector3(0.0f, 0.0f, -tunnel_length/2.0f + (float)segment*segment_length);
			Vector2 segment_texcoord_translation(0.0f, segment_texcoord_step * (float)segment);

			for(unsigned int i=0; i<num_tunnel_sides; ++i) {
				float angle = (float)i * angle_step;

				vertices[vertex].position[0] = cos(angle)*tunnel_scale; 
				vertices[vertex].position[1] = sin(angle)*tunnel_scale; 
				vertices[vertex].position[2] = segment_min;
				vertices[vertex].position += segment_translation;
				vertices[vertex].texcoord[0] = (float)i*tunnel_texcoord_step; 
				vertices[vertex].texcoord[1] = 0.0f;	
				vertices[vertex++].texcoord += segment_texcoord_translation;

				vertices[vertex].position[0] = cos(angle+angle_step)*tunnel_scale; 
				vertices[vertex].position[1] = sin(angle+angle_step)*tunnel_scale; 
				vertices[vertex].position[2] = segment_max;
				vertices[vertex].position += segment_translation;
				vertices[vertex].texcoord[0] = (float)(i+1)*tunnel_texcoord_step; 
				vertices[vertex].texcoord[1] = segment_texcoord_step;
				vertices[vertex++].texcoord += segment_texcoord_translation;

				vertices[vertex].position[0] = cos(angle)*tunnel_scale; 
				vertices[vertex].position[1] = sin(angle)*tunnel_scale; 
				vertices[vertex].position[2] = segment_max;
				vertices[vertex].position += segment_translation;
				vertices[vertex].texcoord[0] = (float)i*tunnel_texcoord_step; 
				vertices[vertex].texcoord[1] = segment_texcoord_step;
				vertices[vertex++].texcoord += segment_texcoord_translation;

				vertices[vertex].position[0] = cos(angle)*tunnel_scale; 
				vertices[vertex].position[1] = sin(angle)*tunnel_scale; 
				vertices[vertex].position[2] = segment_min;
				vertices[vertex].position += segment_translation;
				vertices[vertex].texcoord[0] = (float)i*tunnel_texcoord_step; 
				vertices[vertex].texcoord[1] = 0.0f;
				vertices[vertex++].texcoord += segment_texcoord_translation;

				vertices[vertex].position[0] = cos(angle+angle_step)*tunnel_scale; 
				vertices[vertex].position[1] = sin(angle+angle_step)*tunnel_scale; 
				vertices[vertex].position[2] = segment_min;
				vertices[vertex].position += segment_translation;
				vertices[vertex].texcoord[0] = (float)(i+1)*tunnel_texcoord_step; 
				vertices[vertex].texcoord[1] = 0.0f;
				vertices[vertex++].texcoord += segment_texcoord_translation;

				vertices[vertex].position[0] = cos(angle+angle_step)*tunnel_scale; 
				vertices[vertex].position[1] = sin(angle+angle_step)*tunnel_scale; 
				vertices[vertex].position[2] = segment_max;
				vertices[vertex].position += segment_translation;
				vertices[vertex].texcoord[0] = (float)(i+1)*tunnel_texcoord_step; 
				vertices[vertex].texcoord[1] = segment_texcoord_step;
				vertices[vertex++].texcoord += segment_texcoord_translation;
			}

			// Avoid rounding gap
			vertices[vertex-1].position[1] = 0.0f;
			vertices[vertex-2].position[1] = 0.0f;
			vertices[vertex-5].position[1] = 0.0f;
		}

		vertex_buffer = new StaticVertexBuffer(&vertices, sizeof(ProbeVertex) * num_vertices);
	}

	TunnelProbe::~TunnelProbe()
	{
		delete vertex_buffer;
	}

	int TunnelProbe::GetVertexCount() const
	{
		return num_vertices;
	}
}