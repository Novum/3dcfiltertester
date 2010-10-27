#pragma once

#include "BufferInterfaces.h"

#include <D3D10_1.h>

namespace engine
{
	/*	
	* Obtain this through DynamicBatchManager::NextDynamicVertexBatch
	* This is used for all dynamic vertex input
	*/
	class DynamicVertexBatch : public VertexBufferInterface
	{
		friend class DynamicBatchManager;
	public:
		DynamicVertexBatch(ID3D10Device *d3d10_device,ID3D10Buffer *buffer)	: d3d10_device(d3d10_device), buffer(buffer) {}
		void Fill(void *data);
		virtual ID3D10Buffer *GetVertexBuffer(unsigned int &offset) { offset = this->offset; return buffer; }		
		
		// Wherever possible use Fill instead of Map/Unmap, because it's safer		
		void *Map();
		void Unmap();
	private:
		void SetNextStreamPoint(int offset,int size,bool discard);

		unsigned int offset, size;
		bool discard;

		ID3D10Device *d3d10_device;
		ID3D10Buffer *buffer;
	};

	/*	
	* Obtain this through DynamicBatchManager::NextDynamicIndexBatch
	* This is used for all dynamic index input
	*/
	class DynamicIndexBatch : public IndexBufferInterface
	{
		friend class DynamicBatchManager;
	public:
		DynamicIndexBatch(ID3D10Device *d3d10_device,ID3D10Buffer *buffer) : d3d10_device(d3d10_device), buffer(buffer) {}
		void Fill(void *data);	
		virtual ID3D10Buffer *GetIndexBuffer(unsigned int &offset) { offset = this->offset; return buffer; }				
		
		// Wherever possible use Fill instead of Map/Unmap, because it's safer		
		void *Map();
		void Unmap();
	private:		
		void SetNextStreamPoint(int offset,int size,bool discard);

		unsigned int offset, size;
		bool discard;

		ID3D10Device *d3d10_device;
		ID3D10Buffer *buffer;
	};

	/*
	* DynamicBatchManager manages the dynamic vertex and index buffers
	*/
	class DynamicBatchManager
	{
	public:
		DynamicBatchManager(ID3D10Device *d3d10_device);
		~DynamicBatchManager();

		/* 
		* These functions are used to render dynamic geometry.
		* We manage two different dynamic buffers for vertex and index inputs.
		* Note: Do we need multiple dynamic vertex sources?
		*/
		DynamicVertexBatch &NextDynamicVertexBatch(unsigned int size);
		DynamicIndexBatch &NextDynamicIndexBatch(unsigned int size);		
	private:
		ID3D10Device *d3d10_device;

		ID3D10Buffer *dynamic_vertex_buffer;
		DynamicVertexBatch *dynamic_vertex_batch;
		unsigned int dynamic_vertex_batch_offset;

		ID3D10Buffer *dynamic_index_buffer;
		DynamicIndexBatch *dynamic_index_batch;
		unsigned int dynamic_index_batch_offset;
	};
}
