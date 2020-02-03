//
// Game.h
//

#pragma once

#include "Terrain.h"
#include "DeviceResources.h"
#include "StepTimer.h"
#include "Camera.h"
#include "Mouse.h"
#include "Keyboard.h"

enum class AffectTypes { Add, Remove, Smooth, Flatten };

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
	~Game();
	

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	// Input
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

	//UI Variables 
	int AffectType = 0;
	float PointDistance = 1.f;
	float Frequency = 1.f;
	float surfaceLevel = 20.f;
	int GridSize = 10;
	bool interpolate = true;
	bool wireframe = false;
	bool extrude = true;
	float ToolRadius = 10.f;

	// Game Objects
	Terrain terrain;
	Camera camera;
};
