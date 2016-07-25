#pragma once
#include "DXAppView.h"
#include "d3dx11effect.h"

class DXAppShapes : public DXAppView
{
public:
	DXAppShapes(HINSTANCE hInstance);
	~DXAppShapes();

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

	UINT m_IndexCount;

	ID3D11RasterizerState* m_pWireframeRS;
};

