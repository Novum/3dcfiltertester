#pragma once

#include <string>
#include <D3D10_1.h>

/*  Some D3D10 helper functions */

namespace engine
{
	/* Release a COM object if the pointer is not 0 */
	template <class T> inline void SafeRelease(T& IUnk)
	{
		if (IUnk) { IUnk->Release(); IUnk = 0; }
	}

	/* Translate D3D10 error code to string */
	std::wstring GetD3D10Error(HRESULT hresult);

	/* Translate DXGI format code to string */
	std::string GetDXGIFormatString(DXGI_FORMAT format);

	/* Get DXGI format element size */
	unsigned int GetDXGIFormatSize(DXGI_FORMAT format);

	/* Create shaders from file */
	ID3D10VertexShader *CreateVertexShaderFromFile(std::string name, char *&buffer, unsigned int &size);
	ID3D10GeometryShader *CreateGeometryShaderFromFile(std::string name, char *&buffer, unsigned int &size,
		D3D10_SO_DECLARATION_ENTRY *declaration_entries = 0, unsigned int num_entries = 0, unsigned int stream_output_stride = 0);
	ID3D10PixelShader *CreatePixelShaderFromFile(std::string name, char *&buffer, unsigned int &size);
}