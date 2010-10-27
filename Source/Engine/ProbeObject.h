#pragma once

#include "LinearMath.h"

namespace engine
{
	class VertexBufferInterface;

	struct ProbeVertex
	{
		Vector3 position;
		Vector2 texcoord;
	};

	class ProbeObject
	{
	public:		
		virtual ~ProbeObject() {}
		virtual VertexBufferInterface &GetVertexBuffer() = 0;
		virtual int GetVertexCount() const = 0;
		virtual Vector3 GetDefaultCameraRotation() const = 0;
	};
}