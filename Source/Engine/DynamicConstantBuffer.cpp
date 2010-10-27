#include "DynamicConstantBuffer.h"
#include "Exception.h"
#include "D3DHelper.h"

namespace engine
{
	DynamicConstantBuffer::DynamicConstantBuffer(ID3D10Device *d3d10_device,unsigned int size) : size(size)
	{
		HRESULT hresult;

		// Align size to next 16 byte boundary
		unsigned int byte_width;
		if(size % 16 == 0) byte_width = size;
		else byte_width = ((size / 16) + 1) * 16;

		D3D10_BUFFER_DESC dynamic_ib_desc;
		dynamic_ib_desc.ByteWidth = byte_width;
		dynamic_ib_desc.Usage = D3D10_USAGE_DYNAMIC;
		dynamic_ib_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		dynamic_ib_desc.MiscFlags = 0;
		dynamic_ib_desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;

		if((hresult = d3d10_device->CreateBuffer(&dynamic_ib_desc,0,&buffer)) != S_OK)
			throw Exception(L"Engine: CreateBuffer failed: " + GetD3D10Error(hresult));	
	}

	void DynamicConstantBuffer::Fill(void *data)
	{
		void *mapped_ptr;
		buffer->Map( D3D10_MAP_WRITE_DISCARD,0,&mapped_ptr);
		memcpy(mapped_ptr,data,size);
		buffer->Unmap();
	}
}