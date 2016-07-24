#include "DXApp.h"
#include <assert.h>
#include <windowsx.h>
#include "DXAppUtil.h"

namespace
{
	DXApp* g_App = nullptr;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_App) {
		return g_App->MsgProc(hwnd, msg, wParam, lParam);
	}
	else {
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

DXApp::DXApp(HINSTANCE hInstance, std::string appTitle) 
	:m_ClientWidth(800)
	,m_ClientHeight(600)
	,m_hAppInstance(hInstance)
	,m_AppTitle(appTitle)
	,m_WndStyle(WS_OVERLAPPEDWINDOW)
	,m_hAppWnd(nullptr)
	,m_pDevice(nullptr)
	,m_DriverType(D3D_DRIVER_TYPE_HARDWARE)
{
	assert(g_App == nullptr);
	g_App = this;
}

bool DXApp::Init()
{
	if (!InitWindow()) {
		return false;
	}

	if (!InitDX11()) {
		return false;
	}

	return true;
}

bool DXApp::InitWindow()
{
	
	//WNDCLASSEX
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = m_hAppInstance;
	wcex.lpfnWndProc = MainWndProc;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = "DXAPPWNDCLASS";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		OutputDebugString("FAIL TO CREATE WINDOW CLASS\n");
		return false;
	}

	RECT r = { 0, 0, static_cast<long>(m_ClientWidth), static_cast<long>(m_ClientHeight) };
	AdjustWindowRect(&r, m_WndStyle, FALSE);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	UINT x = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	UINT y = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

	m_hAppWnd = CreateWindow("DXAPPWNDCLASS", m_AppTitle.c_str(), m_WndStyle, x, y, width, height, NULL, NULL, m_hAppInstance, NULL);
	if (!m_hAppWnd) {
		OutputDebugString("FAIL TO CREATE WINDOW\n");
		return false;
	}

	ShowWindow(m_hAppWnd, SW_SHOW);

	return true;
}

bool DXApp::InitDX11()
{
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};
	//CREATE DEVICE
	HRESULT hr = D3D11CreateDevice(nullptr, m_DriverType, NULL, createDeviceFlags,
		featureLevels, 4, D3D11_SDK_VERSION, &m_pDevice, &m_FeatureLevel, &m_pImmediateContext);
	if (FAILED(hr)) {
		OutputDebugString("FAIL TO CREATE D3D11 DEVICE\n");
		return false;
	}

	if (m_FeatureLevel != D3D_FEATURE_LEVEL_11_1) {
		OutputDebugString("FEATURE LEVEL IS NOT 11\n");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = m_hAppWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;

	//DISABLE 4XMSAA
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	IDXGIDevice* dxgiDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	IDXGIFactory* dxgiFactory = nullptr;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	HRESULT hrSwap = dxgiFactory->CreateSwapChain(m_pDevice, &swapDesc, &m_pSwapChain);
	if (FAILED(hrSwap)) {
		OutputDebugString("FAIL TO CREATE D3D11 SWAP CHAIN\n");
		return false;
	}
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	ID3D11Texture2D* backBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	m_pDevice->CreateRenderTargetView(backBuffer, nullptr, &m_pRenderTargetView);
	backBuffer->Release();

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilDesc.Width = m_ClientWidth;
	depthStencilDesc.Height = m_ClientHeight;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	//NO SMAA
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	ID3D11Texture2D* pDepthStencilBuffer;

	m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer);

	m_pDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &m_pDepthStencilView);

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	//pDepthStencilBuffer->Release();

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float> (m_ClientWidth);
	vp.Height = static_cast<float> (m_ClientHeight);
	vp.MinDepth = 0;
	vp.MaxDepth = 1;

	m_pImmediateContext->RSSetViewports(1, &vp);

	return true;
}

int DXApp::Run()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			m_GameTimer.Tick();
			float dt = m_GameTimer.DeltaTime();
			Update(dt);
			Render(dt);
		}
	}

	return static_cast<int>(msg.wParam);
}

LRESULT DXApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_LBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

DXApp::~DXApp()
{
	if (m_pImmediateContext) {
		m_pImmediateContext->ClearState();
	}
	SafeRelease(m_pDevice);
	SafeRelease(m_pImmediateContext);
}

