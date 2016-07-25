#include "DXAppShapes.h"
#include "DXAppUtil.h"
#include <DirectXColors.h>
#include <fstream>
#include "GeometryGenerator.h"

DXAppShapes::DXAppShapes(HINSTANCE hInstance)
	: DXAppView(hInstance, "DXApp Demo Hills")
	, m_pEffect(nullptr)
	, m_pTech(nullptr)
	, m_pInputLayout(nullptr)
	, m_pMatrixVariable(nullptr)
	, m_iBuffer(nullptr)
	, m_vBuffer(nullptr)
	, m_pWireframeRS(nullptr)
{
	m_Radius = 200.0f;
	m_LowRadius = 100.0f;
	m_HighRadius = 300.0f;
	m_RadiusScaleVelocity = 5.0f;
}

DXAppShapes::~DXAppShapes()
{
	SafeRelease(m_pEffect);
	SafeRelease(m_pInputLayout);
	SafeRelease(m_iBuffer);
	SafeRelease(m_vBuffer);
	SafeRelease(m_pWireframeRS);
}

bool DXAppShapes::Init()
{
	if (!DXApp::Init()) {
		return false;
	}

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthClipEnable = true;

	HR(m_pDevice->CreateRasterizerState(&rasterDesc, &m_pWireframeRS));
	return true;
}

void DXAppShapes::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData meshData;
	GeometryGenerator::CreateHillsMeshData(160.0f, 160.0f, 50, 50, meshData);

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbDesc.ByteWidth = sizeof(GeometryGenerator::Vertex) * meshData.Vertices.size();
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vinitData;
	ZeroMemory(&vinitData, sizeof(D3D11_SUBRESOURCE_DATA));
	vinitData.pSysMem = static_cast<void*>(&meshData.Vertices[0]);

	HR(m_pDevice->CreateBuffer(&vbDesc, &vinitData, &m_vBuffer));

	m_IndexCount = meshData.Indices.size();
	D3D11_BUFFER_DESC ibDesc;
	ZeroMemory(&ibDesc, sizeof(D3D11_BUFFER_DESC));
	ibDesc.ByteWidth = sizeof(UINT) * m_IndexCount;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA iinitData;
	ZeroMemory(&iinitData, sizeof(D3D11_SUBRESOURCE_DATA));
	iinitData.pSysMem = static_cast<void*>(&(meshData.Indices[0]));

	HR(m_pDevice->CreateBuffer(&ibDesc, &iinitData, &m_iBuffer));
}

void DXAppShapes::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined(_DEBUG) || defined(DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	std::ifstream fin("Shaders/Box_vs.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size =  static_cast<int>(fin.tellg());
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);
	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, m_pDevice, &m_pEffect));

	/*
	ID3DBlob* compiledShader = nullptr;
	ID3DBlob* compilationMsgs = nullptr;
	HRESULT hr = D3DCompileFromFile(L"Shaders/Box_vs.hlsl", nullptr, nullptr, nullptr, "fx_5_0", shaderFlags, 0, &compiledShader, &compilationMsgs);

	if (compilationMsgs != nullptr) {
		MessageBoxA(nullptr, (char*)compilationMsgs->GetBufferPointer(), nullptr, 0);
	}

	HR(hr);

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, m_pDevice, &m_pEffect));

	SafeRelease(compiledShader);
	*/

	m_pTech = m_pEffect->GetTechniqueByName("ColorTech");
	m_pMatrixVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}

void DXAppShapes::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC passDesc;
	m_pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	HR(m_pDevice->CreateInputLayout(desc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout));

}

void DXAppShapes::Update(float dt)
{
	DXAppView::Update(dt);
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX worldViewProj = world*view*proj;

	m_pMatrixVariable->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
}

void DXAppShapes::Render(float)
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, Colors::CornflowerBlue);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(GeometryGenerator::Vertex);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_vBuffer, &stride, &offset);
	m_pImmediateContext->IASetIndexBuffer(m_iBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_pImmediateContext->RSSetState(m_pWireframeRS);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p) {
		m_pTech->GetPassByIndex(p)->Apply(0, m_pImmediateContext);
		m_pImmediateContext->DrawIndexed(m_IndexCount, 0, 0);
	}

	HR(m_pSwapChain->Present(0, 0));
}
