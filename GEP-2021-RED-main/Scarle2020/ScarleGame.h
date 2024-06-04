//
// ScarleGame.h
//

#pragma once

#include "pch.h"
#include "StepTimer.h"
#include "Effects.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Audio.h"
#include <Data/GameData.h>
#include "ECS.h"

class PhysicsSystem;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.

class ScarleGame
{
public:

	ScarleGame() noexcept;
	virtual ~ScarleGame() = default;

	// Initialization and management
	void Initialize(HWND _window, int _width, int _height);

	// Basic game loop
	void Tick();

	// Pure virtual
	virtual void start() = 0;
	virtual void updateObjects(GameData* gd) = 0;
	virtual void renderObjects() = 0;

	// Messages
	virtual void onActivated();
	virtual void onDeactivated();
	virtual void onSuspending();
	virtual void onResuming();
	virtual void onWindowSizeChanged(int _width, int _height);

	// Optional hook to render imgui ui
	virtual void RenderImGui();

	// Properties
	void GetDefaultSize(int& _width, int& _height) const;

	ID3D11Device1* getDevice() const { return m_d3dDevice.Get(); }
	ID3D11DeviceContext1* getDeviceContext() const { return m_d3dContext.Get(); }
	ECS::ECS* ecs() const { return m_ecs.get(); }
	
	DirectX::SimpleMath::Vector2 getDimensions() const
	{
		// this is a temporary way to get the screen bounds until window resizing events are implemented
		return { static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight) };
	}

protected:
	HWND getWindow() { return m_window; };
	DirectX::AudioEngine* GetAudioEngine() const { return m_audioEngine.get(); }

private:
	void Update(DX::StepTimer const& _timer);
	void Render();

	void Clear();
	void Present();

	void CreateDevice();
	void CreateResources();

	void OnDeviceLost();

private:
	// Device resources.
	HWND m_window;
	int m_outputWidth;
	int m_outputHeight;

	D3D_FEATURE_LEVEL m_featureLevel;
	Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	// Rendering loop timer
	DX::StepTimer m_timer;

	//sound
	std::unique_ptr<DirectX::AudioEngine> m_audioEngine;

	//game data
	std::unique_ptr<GameData> m_gd;

	// Entity component system.
	std::unique_ptr<ECS::ECS> m_ecs;
};
