#include "PlaneProbe.h"

namespace engine
{
	static const unsigned int plane_segments = 50;	
	static const unsigned int num_vertices = 6 * plane_segments * plane_segments;
	
	PlaneProbe::PlaneProbe()
	{
		// Initialize vertex buffer
		ProbeVertex vertices[num_vertices];

		unsigned int vertex = 0;
		float plane_texcoord_scale = 1000.0f / (float)plane_segments;
		float plane_min = -500.0f;
		float plane_max = 500.0f;
		float step_size = (plane_max-plane_min) / (float)plane_segments;

		float current_y_min = plane_min;
		float current_y_max = plane_min + step_size;

		for(unsigned int y=0; y<plane_segments; ++y) {
			float current_x_min = plane_min;
			float current_x_max = plane_min + step_size;

			for(unsigned int x=0; x<plane_segments; ++x) {
				// Generate plane
				vertices[vertex].position[0] = current_x_min; vertices[vertex].position[1] = 0.0f; vertices[vertex].position[2] = current_y_min;
				vertices[vertex].texcoord[0] = 0.0f; vertices[vertex++].texcoord[1] = 0.0f;
				vertices[vertex].position[0] = current_x_max; vertices[vertex].position[1] = 0.0f; vertices[vertex].position[2] = current_y_min;
				vertices[vertex].texcoord[0] = plane_texcoord_scale; vertices[vertex++].texcoord[1] = 0.0f;
				vertices[vertex].position[0] = current_x_min; vertices[vertex].position[1] = 0.0f; vertices[vertex].position[2] = current_y_max;
				vertices[vertex].texcoord[0] = 0.0f; vertices[vertex++].texcoord[1] = plane_texcoord_scale;

				vertices[vertex].position[0] = current_x_min; vertices[vertex].position[1] = 0.0f; vertices[vertex].position[2] = current_y_max;
				vertices[vertex].texcoord[0] = 0.0f; vertices[vertex++].texcoord[1] = plane_texcoord_scale;
				vertices[vertex].position[0] = current_x_max; vertices[vertex].position[1] = 0.0f; vertices[vertex].position[2] = current_y_min;
				vertices[vertex].texcoord[0] = plane_texcoord_scale; vertices[vertex++].texcoord[1] = 0.0f;
				vertices[vertex].position[0] = current_x_max; vertices[vertex].position[1] = 0.0f; vertices[vertex].position[2] = current_y_max;
				vertices[vertex].texcoord[0] = plane_texcoord_scale; vertices[vertex++].texcoord[1] = plane_texcoord_scale;	

				current_x_min += step_size;
				current_x_max += step_size;
			}

			current_y_min += step_size;
			current_y_max += step_size;
		}

		vertex_buffer = new StaticVertexBuffer(&vertices, sizeof(ProbeVertex) * num_vertices);
	}

	PlaneProbe::~PlaneProbe()
	{
		delete vertex_buffer;
	}

	int PlaneProbe::GetVertexCount() const
	{
		return num_vertices;
	}
}