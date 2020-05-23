
#include "Terrain.h"

Terrain::Terrain(ChunkSize size)
{
	Chunk = size;

}

void Terrain::setUp(ID3D11DeviceContext* context, DX::DeviceResources* deviceResources)
{
	mpContext = context;
	mpDeviceResources = deviceResources;

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

	constantBufferDesc.ByteWidth = sizeof(WaterConstantBuffer);
	hr = device->CreateBuffer(&constantBufferDesc, nullptr, &mpWaterConstantBuffer);
	constantBufferDesc.ByteWidth = sizeof(CameraConstantBuffer);
	hr = device->CreateBuffer(&constantBufferDesc, nullptr, &mpCameraConstantBuffer);

	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/Rock.jpg", &mpDiffuseMap, &mpDiffuseMapSRV)))
	{
		std::string t = "Texture failed";
	}

	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/Grass.jpg", &mpDiffuseMap2, &mpDiffuseMapSRV2)))
	{
		std::string t = "Texture failed";
	}

	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/WaterNormalHeight.png", &mpWaterHeightMap, &mpWaterHeightMapSRV)))
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
		std::string t = "Sampler failed";
	}

	//depth texture
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_R32_FLOAT,
		mpDeviceResources->GetOutputSize().right,
		mpDeviceResources->GetOutputSize().bottom,
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_SHADER_RESOURCE
	);

	DX::ThrowIfFailed(device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		DepthTex.ReleaseAndGetAddressOf()
	));


	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MipLevels = 1;
	srDesc.Texture2D.MostDetailedMip = 0;

	device->CreateShaderResourceView(DepthTex.Get(), &srDesc, DepthSrv.ReleaseAndGetAddressOf());

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
		Cubes[i].CreateMesh(earth);
		Cubes[i].CreateMesh(water);
	}

	ID3DBlob* VertexCode;

	LoadVertexShader(device, L"simple_vs.hlsl", &mpVertexShader, &VertexCode);
	LoadVertexShader(device, L"WaterVS.hlsl", &mpWaterVertexShader, &VertexCode);
	LoadPixelShader(device, L"simple_ps.hlsl", &mpPixelShader);
	LoadPixelShader(device, L"Water_ps.hlsl", &mpWaterPixelShader);
	LoadPixelShader(device, L"WaterHeight_ps.hlsl", &mpWaterHeightPixelShader);

	D3D11_INPUT_ELEMENT_DESC VertexDesc[] =
	{
		// Data Type,  Type Index,  Data format                      Slot  Offset    Other values can be ignored for now 
		{ "Position",  0,           DXGI_FORMAT_R32G32B32_FLOAT,     0,    0,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Normal",  0,           DXGI_FORMAT_R32G32B32_FLOAT,     0,    12,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	int VertexDescCount = sizeof(VertexDesc) / sizeof(VertexDesc[0]); // This gives a count of rows in the array above

	device->CreateInputLayout(VertexDesc, VertexDescCount, VertexCode->GetBufferPointer(), VertexCode->GetBufferSize(), &mpVertexLayout);

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
		Cubes[i].CreateMesh(earth);
		Cubes[i].CreateMesh(water);
	}
}

void Terrain::GenerateFullArea(ChunkSize chunksize ,float pointDistance, float frequency, int GridSize, bool interpolate, float surfaceLevel)
{
	Chunk = chunksize;
	Cubes.clear();

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

				Cubes.push_back(MarchingCubes(mpContext, { (float)x,(float)y, (float)z }, edge , pointDistance, frequency, GridSize, interpolate, surfaceLevel));
			}
		}
	}

	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].neighbours = GetNeightbours(i);
	}
	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].CreateMesh(earth);
		Cubes[i].CreateMesh(water);
	}
}

void Terrain::UpdateCubes()
{
	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].UpdateWater();
	}
}

void Terrain::AffectMesh(Vector3 pos, bool direction, float radius, int type)
{
	for (int i = 0; i < Cubes.size(); i++)
	{
		if(Cubes[i].CubeToSphere(pos,radius))
			Cubes[i].AffectPoints(pos, direction ? 1 : -1,radius, type);
	}
}

void Terrain::Smooth(Vector3 pos, float radius, int type)
{
	for (int i = 0; i < Cubes.size(); i++)
	{
		if (Cubes[i].CubeToSphere(pos, radius))
			Cubes[i].Smooth(pos, radius, type);
	}
}

void Terrain::Flatten(Vector3 pos, float radius, int type)
{
	for (int i = 0; i < Cubes.size(); i++)
	{
		if (Cubes[i].CubeToSphere(pos, radius))
			Cubes[i].Flatten(pos, radius, type);
	}
}


void Terrain::sendData(Matrix viewProj, Matrix viewProjInv, float frameTime, Vector3 CameraPos)
{
	D3D11_MAPPED_SUBRESOURCE data;

	ConstantBuffer ModelData{ viewProj,world,viewProj};

	mpContext->Map(mpConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	*static_cast<ConstantBuffer*>(data.pData) = ModelData;

	mpContext->Unmap(mpConstantBuffer, 0);

	WaterConstantBuffer WaterData{ frameTime,0.f,0.f,0.f };

	mpContext->Map(mpWaterConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	*static_cast<WaterConstantBuffer*>(data.pData) = WaterData;

	mpContext->Unmap(mpWaterConstantBuffer, 0);

	CameraConstantBuffer CameraData{ CameraPos.x,CameraPos.y,CameraPos.z,0.f };

	mpContext->Map(mpCameraConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	*static_cast<CameraConstantBuffer*>(data.pData) = CameraData;

	mpContext->Unmap(mpCameraConstantBuffer, 0);
}

void Terrain::render(Matrix viewProj, Matrix viewProjInv,bool Wireframe, float frameTime, Vector3 CameraPos)
{
	sendData(viewProj, viewProjInv, frameTime, CameraPos);

	//set shader info
	auto waterSrv = mpDeviceResources->GetWaterHeightSrv();
	mpContext->VSSetConstantBuffers(0, 1, &mpConstantBuffer);
	mpContext->VSSetConstantBuffers(1, 1, &mpWaterConstantBuffer);
	mpContext->PSSetConstantBuffers(0, 1, &mpCameraConstantBuffer);
	mpContext->PSSetConstantBuffers(1, 1, &mpWaterConstantBuffer);
	mpContext->PSSetShaderResources(0, 1, &mpDiffuseMapSRV);
	mpContext->PSSetShaderResources(1, 1, &mpDiffuseMapSRV2);
	mpContext->PSSetShaderResources(2, 1, &waterSrv);
	mpContext->PSSetSamplers(0, 1, &mpTextureSampler);

	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].RenderEarth(Wireframe ? States->Wireframe() : States->CullCounterClockwise()
			, States->Opaque()
			, States->DepthDefault()
			, mpVertexShader
			, mpPixelShader
			, mpVertexLayout);
	}
	
	mpContext->PSSetShaderResources(2, 1, &mpDiffuseMapSRV2);

	//water height rendering
	auto DepthView = mpDeviceResources->GetDepthStencilView();
	auto waterRenderTarget = mpDeviceResources->GetWaterHeightRenderTargetView();
	mpContext->OMSetRenderTargets(1, &waterRenderTarget, DepthView);
	mpContext->ClearRenderTargetView(waterRenderTarget, DirectX::Colors::Black);

	mpContext->VSSetShaderResources(0, 1, &mpWaterHeightMapSRV);
	mpContext->VSSetSamplers(0, 1, &mpTextureSampler);

	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].RenderWater(States->CullCounterClockwise()
			, States->AlphaBlend()
			, States->DepthDefault()
			, mpWaterVertexShader
		, mpWaterHeightPixelShader
		, mpVertexLayout);
	}

	//reset render target
	auto RenderTarget = mpDeviceResources->GetRenderTargetView();
	mpContext->OMSetRenderTargets(1, &RenderTarget, DepthView);

	auto depth = mpDeviceResources->GetDepthStencil();
	mpContext->CopyResource(DepthTex.Get(), depth);
	mpContext->PSSetShaderResources(2, 1, &mpWaterHeightMapSRV);

	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].RenderWater(Wireframe ? States->Wireframe() : States->CullCounterClockwise()
			, States->AlphaBlend()
			, States->DepthDefault()
			, mpWaterVertexShader
			, mpWaterPixelShader
			, mpVertexLayout);
	}




}

bool Terrain::RayCast(Vector3& Pos, Vector3 Direction, float RayRadius, int RayCastLoops, int type)
{
	for (int increment = 0; increment < RayCastLoops; increment++)
	{
		Vector3 RayPos = Pos + (Direction * increment * RayRadius);
		for (int i = 0; i < Cubes.size(); i++)
		{
			if (Cubes[i].CubeToSphere(RayPos, RayRadius))
			{
				if (Cubes[i].GetSurfacePoint(RayPos, RayRadius, type))
				{
					Pos = RayPos;
					return true;
				}
			}
		}
	}
	return false;
}

void Terrain::SaveTerrain(float pointDistance, int gridSize, std::string saveName)
{
	std::ofstream myfile;
	
	myfile.open("./Maps/" + saveName + ".txt");
	myfile << Chunk.x << std::endl;
	myfile << Chunk.y << std::endl;
	myfile << Chunk.z << std::endl;
	myfile << pointDistance << std::endl;
	myfile << gridSize << std::endl;

	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].WriteChunk(myfile);
	}

	myfile.close();
}
void Terrain::LoadTerrain(float& pointDistance, int& gridSize, std::string fileName)
{
	std::string line;
	std::ifstream infile("./Maps/" + fileName +".txt");

	if (infile.is_open())
	{
		getline(infile, line);
		Chunk.x = std::stoi(line);
		getline(infile, line);
		Chunk.y = std::stoi(line);
		getline(infile, line);
		Chunk.z = std::stoi(line);
		getline(infile, line);
		pointDistance = std::stof(line);
		getline(infile, line);
		gridSize = std::stoi(line);
		
		GenerateFullArea(Chunk, pointDistance, 1, gridSize,true,20);
			
		for (int i = 0; i < Cubes.size(); i++)
		{
			Cubes[i].ReadChunk(infile);
		}

		for (int i = 0; i < Cubes.size(); i++)
		{
			

			Cubes[i].CreateMesh(0);
			Cubes[i].CreateMesh(1);
		}

		infile.close();
	}
}

Neighbours Terrain::GetNeightbours(int cubeNum)
{
	Neighbours n;
	n.Left = cubeNum < Cubes.size() - (Chunk.y * Chunk.z) ? &Cubes[cubeNum + Chunk.y * Chunk.z] : nullptr;
	n.Right = cubeNum >= Chunk.y * Chunk.z ? &Cubes[cubeNum - Chunk.y * Chunk.z] : nullptr;

	n.Up = cubeNum < Cubes.size() - Chunk.z ? &Cubes[cubeNum + Chunk.z] : nullptr;
	n.Down = cubeNum >= Chunk.z ? &Cubes[cubeNum - Chunk.z] : nullptr;

	n.Forward = cubeNum > 0 ? &Cubes[cubeNum - 1] : nullptr;
	n.Back = cubeNum < Cubes.size() - 1 ? &Cubes[cubeNum + 1] : nullptr;

	return n;
}