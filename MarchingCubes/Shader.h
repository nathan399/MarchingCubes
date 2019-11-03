//--------------------------------------------------------------------------------------
// Functions for loading GPU shaders
//--------------------------------------------------------------------------------------
#ifndef _SHADER_H_INCLUDED_
#define _SHADER_H_INCLUDED_

#include <d3d11.h>
#include <string>
#include <vector>

bool LoadVertexShader(ID3D11Device* device, const std::wstring& fileName, ID3D11VertexShader** vertexShader, ID3DBlob** shaderCode);
bool LoadPixelShader(ID3D11Device* device, const std::wstring& fileName, ID3D11PixelShader** pixelShader);

#endif //_SHADER_H_INCLUDED_
