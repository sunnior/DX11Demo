#include "DXAppBox.h"
#include "DXAppUtil.h"
#include <d3dcompiler.h>
#include <DirectXColors.h>

struct Vertex {
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

DXAppBox::DXAppBox(HINSTANCE hInstance) 
	: DXApp(hInstance, "Demo Box")
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_World, I);
	XMStoreFloat4x4(&m_View, I);
	XMStoreFloat4x4(&m_Proj, I);
}


DXAppBox::~DXAppBox()
{
}

bool DXAppBox::Init()
{
	if (!DXApp::Init()) {
		return false;
	}

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	return true;
}

void DXAppBox::BuildGeometryBuffers()
{
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4((const float*)&Colors::White) },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4((const float*)&Colors::Black) },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4((const float*)&Colors::Red) },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4((const float*)&Colors::Green) },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4((const float*)&Colors::Blue) },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4((const float*)&Colors::Yellow) },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4((const float*)&Colors::Cyan) },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4((const float*)&Colors::Magenta) }
	};

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbDesc.ByteWidth = sizeof(Vertex) * 8;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vinitData;
	ZeroMemory(&vinitData, sizeof(D3D11_SUBRESOURCE_DATA));
	vinitData.pSysMem = static_cast<void*>(vertices);

	HR(m_pDevice->CreateBuffer(&vbDesc, &vinitData, &m_vBuffer));

	// Create the index buffer

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibDesc;
	ZeroMemory(&ibDesc, sizeof(D3D11_BUFFER_DESC));
	ibDesc.ByteWidth = sizeof(UINT) * 36;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA iinitData;
	ZeroMemory(&iinitData, sizeof(D3D11_SUBRESOURCE_DATA));
	iinitData.pSysMem = static_cast<void*>(indices);

	HR(m_pDevice->CreateBuffer(&ibDesc, &iinitData, &m_iBuffer));
}

void DXAppBox::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined(_DEBUG) || defined(DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* compiledShader = nullptr;
	ID3DBlob* compilationMsgs = nullptr;
	HRESULT hr = D3DCompileFromFile(L"Shaders/Box_vs.hlsl", nullptr, nullptr, nullptr, "fx_5_0", shaderFlags, 0, &compiledShader, &compilationMsgs);
	
	if (compilationMsgs != nullptr) {
		MessageBoxA(nullptr, (char*)compilationMsgs->GetBufferPointer(), nullptr, 0);
	}

	HR(hr);

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, m_pDevice, &m_pEffect));

	SafeRelease(compiledShader);

	m_pTech = m_pEffect->GetTechniqueByName("ColorTech");
	m_pMatrixVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void DXAppBox::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC passDesc;
	m_pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	HR(m_pDevice->CreateInputLayout(desc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout));

}

void DXAppBox::Update(float)
{
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(0, 3.5, -3.5, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, V);
}

void DXAppBox::Render(float)
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, Colors::CornflowerBlue);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_vBuffer, &stride, &offset);
	m_pImmediateContext->IASetIndexBuffer(m_iBuffer, DXGI_FORMAT_R32_UINT, 0);


	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX worldViewProj = world*view*proj;

	m_pMatrixVariable->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p) {
		m_pTech->GetPassByIndex(p)->Apply(0, m_pImmediateContext);
		m_pImmediateContext->DrawIndexed(36, 0, 0);
	}

	HR(m_pSwapChain->Present(0, 0));
}
