#pragma once
#include "DXApp.h"
#include "MathHelper.h"

class DXAppView : public DXApp
{
public:
	DXAppView(HINSTANCE hInstance, std::string title);
	virtual ~DXAppView();

	virtual void Update(float dt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

protected:

	XMFLOAT4X4 m_World;
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;

	float m_Theta;
	float m_Phi;
	float m_Radius;

	float m_LowRadius;
	float m_HighRadius;
	float m_RadiusScaleVelocity;

	POINT m_LastMousePos;
};

