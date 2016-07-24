#include "DXAppSingleColor.h"
#include <DirectXColors.h>

DXAppSingleColor::DXAppSingleColor(HINSTANCE hInstance) : DXApp(hInstance, "Single Color Demo")
{
}

void DXAppSingleColor::Update(float dt)
{

}

void DXAppSingleColor::Render(float dt)
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::CornflowerBlue);
	m_pSwapChain->Present(0, 0);
}

void DXAppSingleColor::OnMouseDown(WPARAM btnState, int x, int y)
{
	OutputDebugString("Detect Mouse Down Event\n");
}

DXAppSingleColor::~DXAppSingleColor()
{
}
