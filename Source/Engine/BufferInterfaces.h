#pragma once

#include <D3D10_1.h>

namespace engine
{
	class VertexBufferInterface
	{
	public:		
		virtual ID3D10Buffer *GetVertexBuffer(unsigned int &offset) = 0;		
	};

	class IndexBufferInterface
	{
	public:
		enum format { indices_16_bit, indices_32_bit };
		
		virtual ID3D10Buffer *GetIndexBuffer(unsigned int &offset) = 0;		
	};

	class ConstantBufferInterface
	{
	public:
		virtual ID3D10Buffer *GetConstantBuffer() const = 0;
	};
}