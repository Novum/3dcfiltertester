#include "StaticBuffers.h"
#include "D3DHelper.h"
#include "Exception.h"
#include "Engine.h"

namespace engine
{
	/*
	 * StaticVertexBuffer
	 */
	StaticVertexBuffer::StaticVertexBuffer(void *data, unsigned int size) 
	{
		ID3D10Device *d3d10_device = engine.GetDevice();

		HRESULT hresult;

		D3D10_BUFFER_DESC vb_desc;
		vb_desc.ByteWidth = size;
		vb_desc.Usage = D3D10_USAGE_IMMUTABLE;
		vb_desc.CPUAccessFlags = 0;
		vb_desc.MiscFlags = 0;
		vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;

		D3D10_SUBRESOURCE_DATA subresource_data;
		subresource_data.pSysMem = data;
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		if((hresult = d3d10_device->CreateBuffer(&vb_desc,&subresource_data,&buffer)) != S_OK)
			throw Exception(L"StaticVertexBuffer: CreateBuffer failed: " + GetD3D10Error(hresult));	
	}

	StaticVertexBuffer::~StaticVertexBuffer()
	{
		SafeRelease(buffer);
	}

	/*
	* StaticIndexBuffer
	*/
	StaticIndexBuffer::StaticIndexBuffer(void *data, unsigned int size)
	{
		ID3D10Device *d3d10_device = engine.GetDevice();

		HRESULT hresult;

		D3D10_BUFFER_DESC ib_desc;
		ib_desc.ByteWidth = size;
		ib_desc.Usage = D3D10_USAGE_IMMUTABLE;
		ib_desc.CPUAccessFlags = 0;
		ib_desc.MiscFlags = 0;
		ib_desc.BindFlags = D3D10_BIND_INDEX_BUFFER;

		D3D10_SUBRESOURCE_DATA subresource_data;
		subresource_data.pSysMem = data;
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		if((hresult = d3d10_device->CreateBuffer(&ib_desc,&subresource_data,&buffer)) != S_OK)
			throw Exception(L"StaticIndexBuffer: CreateBuffer failed: " + GetD3D10Error(hresult));	
	}

	StaticIndexBuffer::~StaticIndexBuffer()
	{
		SafeRelease(buffer);
	}

	/*
	* StaticConstantBuffer
	*/
	StaticConstantBuffer::StaticConstantBuffer(void *data, unsigned int size)
	{
		ID3D10Device *d3d10_device = engine.GetDevice();

		HRESULT hresult;

		D3D10_BUFFER_DESC cb_desc;
		cb_desc.ByteWidth = size;
		cb_desc.Usage = D3D10_USAGE_DEFAULT;
		cb_desc.CPUAccessFlags = 0;
		cb_desc.MiscFlags = 0;
		cb_desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;

		D3D10_SUBRESOURCE_DATA subresource_data;
		subresource_data.pSysMem = data;
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		if((hresult = d3d10_device->CreateBuffer(&cb_desc,&subresource_data,&buffer)) != S_OK)
			throw Exception(L"StaticConstantBuffer: CreateBuffer failed: " + GetD3D10Error(hresult));	
	}

	StaticConstantBuffer::~StaticConstantBuffer()
	{
		SafeRelease(buffer);
	}

	void StaticConstantBuffer::UpdateBuffer(void *data,unsigned int size)
	{
		ID3D10Device *d3d10_device;
		buffer->GetDevice(&d3d10_device);

		d3d10_device->UpdateSubresource(buffer,0,0,data,size,size);
	}
}