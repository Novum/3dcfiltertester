#pragma once

#include "BufferInterfaces.h"

#include <D3D10_1.h>

namespace engine
{
	class DynamicConstantBuffer : public ConstantBufferInterface
	{
	public:
		DynamicConstantBuffer(ID3D10Device *d3d10_device,unsigned int size);
		void Fill(void *data);
		virtual ID3D10Buffer *GetConstantBuffer() const { return buffer; }
	private:
		ID3D10Buffer *buffer;
		unsigned int size;
	};
}