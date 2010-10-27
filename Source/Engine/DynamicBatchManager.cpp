#include "DynamicBatchManager.h"
#include "D3DHelper.h"
#include "Exception.h"

#include "../Misc/mmgr.h"

namespace engine
{
	// These should roughly be big enough to buffer at least one frame of data
	static const unsigned int dynamic_vertex_buffer_size = 8192 * 1024; // 8 MiB dynamic vertex buffer
	static const unsigned int dynamic_index_buffer_size = 2048 * 1024; // 2 MiB dynamic index buffer	

	/*
	 * DynamicVertexBatch
	 */
	void DynamicVertexBatch::Fill(void *data)
	{
		char *buffer_ptr;			
		D3D10_MAP map_flags = discard ? D3D10_MAP_WRITE_DISCARD : D3D10_MAP_WRITE_NO_OVERWRITE;
		buffer->Map(map_flags,0,(void**)&buffer_ptr);
		memcpy(buffer_ptr+offset,data,size);
		buffer->Unmap();
	}

	void *DynamicVertexBatch::Map()
	{
		char *buffer_ptr;			
		D3D10_MAP map_flags = discard ? D3D10_MAP_WRITE_DISCARD : D3D10_MAP_WRITE_NO_OVERWRITE;
		buffer->Map(map_flags,0,(void**)&buffer_ptr);		
		return (void*)(buffer_ptr+offset);
	}

	void DynamicVertexBatch::Unmap()
	{
		buffer->Unmap();
	}

	void DynamicVertexBatch::SetNextStreamPoint(int offset,int size,bool discard)
	{
		this->offset = offset;
		this->size = size;		
		this->discard = discard;
	}

	/*
	 * DynamicIndexBatch
	 */
	void DynamicIndexBatch::Fill(void *data)
	{
		char *buffer_ptr;			
		D3D10_MAP map_flags = discard ? D3D10_MAP_WRITE_DISCARD : D3D10_MAP_WRITE_NO_OVERWRITE;
		buffer->Map(map_flags,0,(void**)&buffer_ptr);
		memcpy(buffer_ptr+offset,data,size);
		buffer->Unmap();
	}

	void *DynamicIndexBatch::Map()
	{
		char *buffer_ptr;			
		D3D10_MAP map_flags = discard ? D3D10_MAP_WRITE_DISCARD : D3D10_MAP_WRITE_NO_OVERWRITE;
		buffer->Map(map_flags,0,(void**)&buffer_ptr);		
		return (void*)(buffer_ptr+offset);
	}

	void DynamicIndexBatch::Unmap()
	{
		buffer->Unmap();
	}

	void DynamicIndexBatch::SetNextStreamPoint(int offset,int size,bool discard)
	{
		this->offset = offset;
		this->size = size;
		this->discard = discard;
	}

	/*
	 * DynamicBatchManager
	 */
	DynamicBatchManager::DynamicBatchManager(ID3D10Device *d3d10_device) 
		: d3d10_device(d3d10_device), dynamic_vertex_batch_offset(0), dynamic_index_batch_offset(0)
	{
		HRESULT hresult;

		D3D10_BUFFER_DESC dynamic_vb_desc;
		dynamic_vb_desc.ByteWidth = dynamic_vertex_buffer_size;
		dynamic_vb_desc.Usage = D3D10_USAGE_DYNAMIC;
		dynamic_vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		dynamic_vb_desc.MiscFlags = 0;
		dynamic_vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;

		if((hresult = d3d10_device->CreateBuffer(&dynamic_vb_desc,0,&dynamic_vertex_buffer)) != S_OK)
			throw Exception(L"Engine: CreateBuffer failed: " + GetD3D10Error(hresult));	

		dynamic_vertex_batch = new DynamicVertexBatch(d3d10_device,dynamic_vertex_buffer);

		D3D10_BUFFER_DESC dynamic_ib_desc;		
		dynamic_ib_desc.ByteWidth = dynamic_index_buffer_size;
		dynamic_ib_desc.Usage = D3D10_USAGE_DYNAMIC;
		dynamic_ib_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		dynamic_ib_desc.MiscFlags = 0;
		dynamic_ib_desc.BindFlags = D3D10_BIND_INDEX_BUFFER;

		if((hresult = d3d10_device->CreateBuffer(&dynamic_ib_desc,0,&dynamic_index_buffer)) != S_OK)
			throw Exception(L"Engine: CreateBuffer failed: " + GetD3D10Error(hresult));	

		dynamic_index_batch = new DynamicIndexBatch(d3d10_device,dynamic_index_buffer);
	}

	DynamicBatchManager::~DynamicBatchManager()
	{		
		delete dynamic_index_batch;
		delete dynamic_vertex_batch;
		
		SafeRelease(dynamic_index_buffer);
		SafeRelease(dynamic_vertex_buffer);
	}

	/* 
	* Prepare DynamicVertexBatch interface the next batch
	*/
	DynamicVertexBatch &DynamicBatchManager::NextDynamicVertexBatch(unsigned int size)
	{		
		if(size > dynamic_vertex_buffer_size) 
			throw Exception(L"The allocated dynamic vertex buffer is too small for the requested batch size");

		if(dynamic_vertex_batch_offset+size > dynamic_vertex_buffer_size) {
			// Data doesn't fit in old buffer, so discard it and start at the beginning of a new one
			dynamic_vertex_batch->SetNextStreamPoint(0,size,true);
			dynamic_vertex_batch_offset = size;
		}
		else {
			// Data fits in old buffer, so just update the buffer offset
			dynamic_vertex_batch->SetNextStreamPoint(dynamic_vertex_batch_offset,size,false);
			dynamic_vertex_batch_offset += size;
		}

		return *dynamic_vertex_batch;
	}

	/* 
	* Prepare DynamicIndexBatch interface for next batch
	*/
	DynamicIndexBatch &DynamicBatchManager::NextDynamicIndexBatch(unsigned int size)
	{
		if(size > dynamic_index_buffer_size) 
			throw Exception(L"The allocated dynamic index buffer is too small for the requested batch size");

		if(dynamic_index_batch_offset+size > dynamic_index_buffer_size) {
			// Data doesn't fit in old buffer, so discard it and start at the beginning of a new one
			dynamic_index_batch->SetNextStreamPoint(0,size,true);
			dynamic_index_batch_offset = size;			
		}
		else {
			// Data fits in old buffer, so just update the buffer offset
			dynamic_index_batch->SetNextStreamPoint(dynamic_index_batch_offset,size,false);			
			dynamic_index_batch_offset += size;			
		}	

		return *dynamic_index_batch;
	}
}