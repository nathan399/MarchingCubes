//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <iostream>

extern void ExitGame();

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false) : terrain({5,5,2})
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());

	/*initialise objects*/
	terrain.setUp(m_deviceResources->GetD3DDeviceContext(),m_deviceResources.get());

	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	
	

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
	
	auto kb = m_keyboard->GetState();
	if (kb.Escape) ExitGame();

	Vector3 move;
	float pitch = 0,yaw = 0;

	if (kb.W) move.z += 1.f;
	if (kb.S) move.z -= 1.f;
	if (kb.A) move.x += 1.f;
	if (kb.D) move.x -= 1.f;
	if (kb.Space) move.y += 1.f;
	if (kb.LeftControl) move.y -= 1.f;

	auto mouse = m_mouse->GetState();

	if (mouse.positionMode == Mouse::MODE_RELATIVE)
	{
		Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f) * 0.001;

		pitch -= delta.y;
		yaw -= delta.x;
	}
	m_mouse->SetMode(mouse.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	camera.Move(move * elapsedTime, pitch, yaw);

	if (mouse.leftButton && (mouse.x > 260 || mouse.positionMode == Mouse::MODE_RELATIVE))
	{
		Vector3 pos = camera.GetPos();
		Vector3 PointAheadPos = camera.GetPos() - (camera.getZAxis() * 30);

		auto r = m_deviceResources->GetOutputSize();
		auto mousePos = camera.WorldPointFromPixel(mouse.x, mouse.y, r.right, r.bottom);

		Vector3 ray = (mousePos - pos);
		ray.Normalize();

		auto raycastType = earth;
		if (RaycastWater)
			raycastType = water;

		if(terrain.RayCast(pos, ray, PointDistance, 100, raycastType))
		{
			auto state = earth;
			if (WaterMode)
				state = water;

			switch (AffectType)
			{
			case 0: 
			{
				terrain.AffectMesh(pos, true, ToolRadius, state);
				break;
			}
			case 1:
			{
				terrain.AffectMesh(pos, false, ToolRadius, state);
				break;
			}
			case 2:
			{
				terrain.Smooth(pos, ToolRadius, state);
				break;
			}
			case 3:
			{
				terrain.Flatten(pos, ToolRadius, state);
				break;
			}
			}
		}
		
	}
	if(UpdateWater)
		terrain.UpdateCubes();

    elapsedTime;
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

	
    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
	m_deviceResources->GetD3DDevice();
    // TODO: Add your rendering code here.
   
	camera.Render();

	terrain.render(camera.getViewProj(),wireframe);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0,-10));
	ImGui::SetNextWindowSize(ImVec2(260, 1080));

	ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	std::string fps = std::to_string(m_timer.GetFramesPerSecond());
	fps = "Fps: " + fps;

	ImGui::Text( &fps[0]);
	
	//slider and text
	bool changed = false;

	ImGui::Text("Point Distance");
	changed |= ImGui::SliderFloat("", &PointDistance, 0.1f, 10.f);

	ImGui::Text("Frequency");
	changed |= ImGui::SliderFloat(" ", &Frequency, 0.1f, 20.f);

	ImGui::Text("Surface Level");
	changed |= ImGui::SliderFloat("  ", &surfaceLevel, 5.f, 30.f);

	ImGui::Text("GridSize");
	changed |= ImGui::SliderInt("   ", &GridSize, 5, 30);

	ImGui::Text("Interpolate");
	changed |= ImGui::Checkbox("    ", &interpolate);
		
	if(changed)
		terrain.generateTerrain(PointDistance, Frequency, GridSize, interpolate, surfaceLevel);

	/*if (ImGui::Button("Regenerate"))
		terrain.generateTerrain(PointDistance, Frequency, GridSize, interpolate);*/

	ImGui::Separator();

	static const char* items[] = { "Add", "Remove", "Smooth", "Flatten"};
	static const char* current_item = items[0];

	if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			bool is_selected = current_item == items[n]; // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(items[n], is_selected))
			{
				AffectType = n;
				current_item = items[n];
			}
			if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	
	ImGui::Text("Affect water");
	ImGui::Checkbox("     ", &WaterMode);

	ImGui::Text("Update water");
	ImGui::Checkbox("      ", &UpdateWater);

	ImGui::Text("Raycast water");
	ImGui::Checkbox("       ", &RaycastWater);

	ImGui::Text("ToolRadius");
	ImGui::SliderFloat("        ", &ToolRadius, 1.f, 40.f);

	ImGui::Separator();

	ImGui::Text("Wireframe");
	ImGui::Checkbox("         ", &wireframe);
	

	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
	int width, height;
	GetDefaultSize(width, height);

	camera = Camera({ 0,0,-5 }, (float)width, (float)height);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
