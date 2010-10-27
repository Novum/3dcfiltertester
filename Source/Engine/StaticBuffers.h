#pragma once

#include "BufferInterfaces.h"

#include <D3D10_1.h>

namespace engine
{
	class StaticVertexBuffer : public VertexBufferInterface
	{
	public:
		StaticVertexBuffer(void *data,unsigned int size);
		~StaticVertexBuffer();
		virtual ID3D10Buffer *GetVertexBuffer(unsigned int &offset) { offset = 0; return buffer; }		
	private:		
		ID3D10Buffer *buffer;
	};

	class StaticIndexBuffer : public IndexBufferInterface
	{
	public:
		StaticIndexBuffer(void *data,unsigned int size);
		~StaticIndexBuffer();
		virtual ID3D10Buffer *GetIndexBuffer(unsigned int &offset) { offset = 0; return buffer; }		
	private:		
		ID3D10Buffer *buffer;
	};

	class StaticConstantBuffer : public ConstantBufferInterface
	{
	public:
		StaticConstantBuffer(void *data,unsigned int size);
		~StaticConstantBuffer();
		void UpdateBuffer(void *data,unsigned int size);
		ID3D10Buffer *GetConstantBuffer() const { return buffer; }
	private:				
		ID3D10Buffer *buffer;
	};
}