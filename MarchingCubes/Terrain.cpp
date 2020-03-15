
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

	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/mossy-ground1-albedo.dds", &SurfaceTextures.mpAlbedoMap, &SurfaceTextures.mpAlbedoMapSRV))) {std::string t = "Texture failed";}
	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/mossy-ground1-roughness.dds", &SurfaceTextures.mpRoughnessMap, &SurfaceTextures.mpRoughnessMapSRV))) { std::string t = "Texture failed"; }
	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/mossy-ground1-metalness.dds", &SurfaceTextures.mpMetalnessMap, &SurfaceTextures.mpMetalnessMapSRV))) { std::string t = "Texture failed"; }
	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/mossy-ground1-normal.dds", &SurfaceTextures.mpNormalMap, &SurfaceTextures.mpNormalMapSRV))) { std::string t = "Texture failed"; }
	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/mossy-ground1-height.dds", &SurfaceTextures.mpHeightMap, &SurfaceTextures.mpHeightMapSRV))) { std::string t = "Texture failed"; }


	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/Cliff_Albedo.jpg", &EdgeTextures.mpAlbedoMap, &EdgeTextures.mpAlbedoMapSRV))) {std::string t = "Texture failed";}
	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/Cliff_roughness.dds", &EdgeTextures.mpRoughnessMap, &EdgeTextures.mpRoughnessMapSRV))) { std::string t = "Texture failed"; }
	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/mossy-ground1-metalness.dds", &EdgeTextures.mpMetalnessMap, &EdgeTextures.mpMetalnessMapSRV))) { std::string t = "Texture failed"; }
	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/Cliff_normal.dds", &EdgeTextures.mpNormalMap, &EdgeTextures.mpNormalMapSRV))) { std::string t = "Texture failed"; }
	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"MediaCliff_height.dds", &EdgeTextures.mpHeightMap, &EdgeTextures.mpHeightMapSRV))) { std::string t = "Texture failed"; }

	if (FAILED(DirectX::CreateWICTextureFromFile(device, context, L"Media/WaterNormalHeight.png", &mpWaterHeightMap, &mpWaterHeightMapSRV))) {std::string t = "Texture failed";}

	

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

	//set textures
	mpContext->PSSetShaderResources(0, 1, &EdgeTextures.mpAlbedoMapSRV);
	mpContext->PSSetShaderResources(1, 1, &EdgeTextures.mpRoughnessMapSRV);
	mpContext->PSSetShaderResources(2, 1, &EdgeTextures.mpMetalnessMapSRV);
	mpContext->PSSetShaderResources(3, 1, &EdgeTextures.mpNormalMapSRV);
	mpContext->PSSetShaderResources(4, 1, &EdgeTextures.mpHeightMapSRV);
	mpContext->PSSetShaderResources(5, 1, &SurfaceTextures.mpAlbedoMapSRV);
	mpContext->PSSetShaderResources(6, 1, &SurfaceTextures.mpRoughnessMapSRV);
	mpContext->PSSetShaderResources(7, 1, &SurfaceTextures.mpMetalnessMapSRV);
	mpContext->PSSetShaderResources(8, 1, &SurfaceTextures.mpNormalMapSRV);
	mpContext->PSSetShaderResources(9, 1, &SurfaceTextures.mpHeightMapSRV);
	mpContext->PSSetShaderResources(10, 1, &waterSrv);
	mpContext->PSSetSamplers(0, 1, &mpTextureSampler);

	for (int i = 0; i < Cubes.size(); i++)
	{
		Cubes[i].RenderEarth(Wireframe ? States->Wireframe() : States->CullCounterClockwise()
			, States->Opaque()
			, States->DepthDefault());
	}

	mpContext->PSSetShaderResources(10, 1, &SurfaceTextures.mpAlbedoMapSRV);

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
			, true);
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
			, States->DepthDefault());
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