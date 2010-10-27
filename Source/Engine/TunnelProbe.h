#pragma once

#include "ProbeObject.h"
#include "StaticBuffers.h"

namespace engine
{
	class TunnelProbe : public ProbeObject
	{
	public:
		TunnelProbe();
		virtual ~TunnelProbe();
		virtual VertexBufferInterface &GetVertexBuffer() { return *vertex_buffer; }
		virtual int GetVertexCount() const;
		virtual Vector3 GetDefaultCameraRotation() const { 
			return Vector3(0.0f, 0.0f, 0.0f); 
		}
	private:
		StaticVertexBuffer *vertex_buffer;
	};
}