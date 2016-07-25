#include "DXAppView.h"



DXAppView::DXAppView(HINSTANCE hInstance, std::string title)
	: DXApp(hInstance, title)
	, m_Theta(1.5f*MathHelper::Pi)
	, m_Phi(0.25f*MathHelper::Pi)
	, m_Radius(5.0f)
	, m_LowRadius(3.0f)
	, m_HighRadius(15.0f)
	, m_RadiusScaleVelocity(1.0f)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_World, I);
	XMStoreFloat4x4(&m_View, I);
}


DXAppView::~DXAppView()
{
}

void DXAppView::Update(float dt)
{
	float x = m_Radius*sinf(m_Phi)*cosf(m_Theta);
	float z = m_Radius*sinf(m_Phi)*sinf(m_Theta);
	float y = m_Radius*cosf(m_Phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, V);
}

void DXAppView::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void DXAppView::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Theta += dx;
		m_Phi += dy;

		// Restrict the angle mPhi.
		m_Phi = MathHelper::Clamp(m_Phi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_Radius += (dx - dy)*m_RadiusScaleVelocity;

		// Restrict the radius.
		m_Radius = MathHelper::Clamp(m_Radius, m_LowRadius, m_HighRadius);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void DXAppView::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_hAppWnd);
}