#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <string>
#include "GameTimer.h"

class DXApp
{
public:
	DXApp(HINSTANCE hInstance, std::string appTitle);
	virtual ~DXApp();

	virtual bool Init();
	int Run();

	virtual void Update(float dt) = 0;
	virtual void Render(float dt) = 0;
	void CalculateFrameStats();

	float AspectRatio() const;

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	bool InitWindow();
	bool InitDX11();

protected:
	GameTimer		m_GameTimer;
	UINT			m_ClientWidth;
	UINT			m_ClientHeight;
	DWORD			m_WndStyle;
	std::string		m_AppTitle;
	HINSTANCE		m_hAppInstance;
	HWND			m_hAppWnd;

	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pImmediateContext;
	D3D_DRIVER_TYPE			m_DriverType;
	D3D_FEATURE_LEVEL		m_FeatureLevel;

	IDXGISwapChain*			m_pSwapChain;

	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;


};

