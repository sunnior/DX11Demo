#pragma once
#include <DirectXMath.h>
#include <vector>
#include <windows.h>
using namespace DirectX;

class GeometryGenerator
{
public:
	struct Vertex {
		XMFLOAT3 Position;
		XMFLOAT4 Color;
	};

	struct MeshData {
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

	static void CreatePlaneMeshData(float xLength, float zLength, UINT xNum, UINT zNum, MeshData& meshData);
	static void CreateHillsMeshData(float xLength, float zLength, UINT xNum, UINT zNum, MeshData& meshData);
};