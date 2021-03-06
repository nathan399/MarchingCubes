
#include "Terrain.h"

Terrain::Terrain(ChunkSize size)
{
	Chunk = size;
}

void Terrain::setUp(ID3D11DeviceContext* context) 
{
	mpContext = context;

	ID3D11Device* device;
	mpContext->GetDevice(&device);
	
	States = std::make_unique<DirectX::CommonStates>(device);

	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = device->CreateBuffer(&constantBufferDesc, nullptr, &mpConstantBuffer);

	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/Rock.jpg", &mpDiffuseMap, &mpDiffuseMapSRV)))
	{
		std::string t = "Texture failed";
	}

	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/Grass.jpg", &mpDiffuseMap2, &mpDiffuseMapSRV2)))
	{
		std::string t = "Texture failed";
	}

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // Filtering method - see lecture
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;   // Addressing mode for texture coordinates outside 0->1 - see lecture
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--

	samplerDesc.MaxAnisotropy = 1;                        // Number of samples used for anisotropic filtering, more is better, but max value depends on GPU

	samplerDesc.MaxLOD = 0; // Controls how much mip-mapping can be used
	samplerDesc.MinLOD = 0; // --"--

	// Then create a DirectX object for your description that can be used by a shader
	if (FAILED(device->CreateSamplerState(&samplerDesc, &mpTextureSampler)))
	{
		std::string t = "wtf";
	}

	//ID3DBlob* VertexCode;

	//LoadVertexShader(device, L"simple_vs.hlsl", &mpVertexShader, &VertexCode);
	//LoadPixelShader(device, L"simple_ps.hlsl", &mpPixelShader);

	//D3D11_INPUT_ELEMENT_DESC VertexDesc[] =
	//{
	//	// Data Type,  Type Index,  Data format                      Slot  Offset    Other values can be ignored for now 
	//	{ "Position",  0,           DXGI_FORMAT_R32G32B32_FLOAT,     0,    0,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "Normal",  0,           DXGI_FORMAT_R32G32B32_FLOAT,     0,    12,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};
	//int VertexDescCount = sizeof(VertexDesc) / sizeof(VertexDesc[0]); // This gives a count of rows in the array above

	//device->CreateInputLayout(VertexDesc, VertexDescCount, VertexCode->GetBufferPointer(), VertexCode->GetBufferSize(), &mpVertexLayout);

	//SetBuffers();

	//create cubes
	for (int x = 0; x < Chunk.x; x++)
	{
		for (int y = 0; y < Chunk.y; y++)
		{
			for (int z = 0; z < Chunk.z; z++)
			{
				sEdges edge
				{
					x == Chunk.x - 1 ? true : false,
					x == 0 ? true : false,
					y == Chunk.y - 1 ? true : false,
					y == 0 ? true : false,
					z == Chunk.z - 1 ? true : false,
					z == 0 ? true : false
				};

				Cubes.push_back(MarchingCubes(context,{ (float)x,(float)y, (float)z }, edge));
			}
		}
	}

	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].neighbours = GetNeightbours(i);
	}
	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].CreateMesh();
	}

}

void Terrain::generateTerrain(float pointDistance,float frequency, int GridSize, bool interpolate, float surfaceLevel)
{
	/*generate vertices */
	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].generate(pointDistance, frequency, GridSize, interpolate,surfaceLevel);
	}
	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].CreateMesh();
	}
}

void Terrain::AffectMesh(Vector3 pos, bool direction, float radius)
{
	for (int i = 0; i < Cubes.size(); i++)
	{
		if(Cubes[i].CubeToSphere(pos,radius))
			Cubes[i].AffectPoints(pos, direction ? 1 : -1,radius);
	}
}

void Terrain::Smooth(Vector3 pos, float radius)
{
	for (int i = 0; i < Cubes.size(); i++)
	{
		if (Cubes[i].CubeToSphere(pos, radius))
			Cubes[i].Smooth(pos, radius);
	}
}

void Terrain::Flatten(Vector3 pos, float radius)
{
	for (int i = 0; i < Cubes.size(); i++)
	{
		if (Cubes[i].CubeToSphere(pos, radius))
			Cubes[i].Flatten(pos, radius);
	}
}

void Terrain::SetBuffers()
{
	Vertices.clear();
	if(mVertexBuffer)
		mVertexBuffer->Release();

	for (int i = 0; i < Cubes.size(); i++)
	{
		std::vector<SVertices> MarchingVertices = Cubes[i].getVertices();

		for (auto vertex : MarchingVertices)
		{
			Vertices.push_back({ vertex.pos.x,vertex.pos.y,vertex.pos.z,vertex.normal.x,vertex.normal.y,vertex.normal.z });
		}
	}

	HRESULT hr = S_OK;
	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CUSTOMVERTEX) * Vertices.size();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &Vertices[0];
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	ID3D11Device* device;
	mpContext->GetDevice(&device);
	hr = device->CreateBuffer(&bufferDesc, &InitData, &mVertexBuffer);
}

void Terrain::sendData(Matrix viewProj)
{
	D3D11_MAPPED_SUBRESOURCE data;

	ConstantBuffer dater{ viewProj,world };

	mpContext->Map(mpConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	*static_cast<ConstantBuffer*>(data.pData) = dater;

	mpContext->Unmap(mpConstantBuffer, 0);
}

void Terrain::render(Matrix viewProj, bool Wireframe)
{

	sendData(viewProj);
	mpContext->VSSetConstantBuffers(0, 1, &mpConstantBuffer);
	mpContext->PSSetShaderResources(0, 1, &mpDiffuseMapSRV);
	mpContext->PSSetShaderResources(1, 1, &mpDiffuseMapSRV2);
	mpContext->PSSetSamplers(0, 1, &mpTextureSampler);

	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].Render(Wireframe ? States->Wireframe() : States->CullCounterClockwise());
	}
}

bool Terrain::RayCast(Vector3& Pos, Vector3 Direction, float RayRadius, int RayCastLoops)
{
	for (int increment = 0; increment < RayCastLoops; increment++)
	{
		Vector3 RayPos = Pos + (Direction * increment * RayRadius);
		for (int i = 0; i < Cubes.size(); i++)
		{
			if (Cubes[i].CubeToSphere(RayPos, RayRadius))
			{
				if (Cubes[i].GetSurfacePoint(RayPos, RayRadius))
				{
					Pos = RayPos;
					return true;
				}
			}
		}
	}
	return false;
}

Neighbours Terrain::GetNeightbours(int cubeNum)
{
	Neighbours n;
	n.Left = cubeNum <= Cubes.size() - Chunk.y * Chunk.z ? &Cubes[cubeNum + Chunk.y * Chunk.z] : nullptr;
	n.Right = cubeNum >= Chunk.y * Chunk.z ? &Cubes[cubeNum - Chunk.y * Chunk.z] : nullptr;

	n.Up = cubeNum <= Cubes.size() - Chunk.z ? &Cubes[cubeNum + Chunk.z] : nullptr;
	n.Down = cubeNum >= Chunk.z ? &Cubes[cubeNum - Chunk.z] : nullptr;

	n.Forward = cubeNum > 0 ? &Cubes[cubeNum - 1] : nullptr;
	n.Back = cubeNum < Cubes.size() - 1 ? &Cubes[cubeNum + 1] : nullptr;

	return n;
}