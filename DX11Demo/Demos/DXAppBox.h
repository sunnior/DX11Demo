#pragma once
#include "DXApp.h"
#include "d3dx11effect.h"
#include <DirectXMath.h>
using namespace DirectX;

class DXAppBox : public DXApp
{
public:
	DXAppBox(HINSTANCE);
	~DXAppBox();

	bool Init() override;

	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

	void Update(float) override;
	void Render(float) override;

private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTech;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11EffectMatrixVariable* m_pMatrixVariable;

	ID3D11Buffer* m_iBuffer;
	ID3D11Buffer* m_vBuffer;

	XMFLOAT4X4 m_World;
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;

	float m_Theta;
	float m_Phi;
	float m_Radius;
};

