#include "GeometryGenerator.h"

void GeometryGenerator::CreatePlaneMeshData(float xLength, float zLength, UINT xNum, UINT zNum, MeshData& meshData)
{
	float dx = xLength / (xNum - 1);
	float dz = zLength / (zNum - 1);

	meshData.Vertices.resize(zNum*xNum);
	meshData.Indices.resize((xNum - 1)*(zNum - 1) * 6);
	for (UINT z = 0; z < zNum; z++) {
		float zPos = 0.5f*zLength - z*dz;
		for (UINT x = 0; x < xNum; x++) {
			meshData.Vertices[z*xNum + x].Position = XMFLOAT3(-0.5f*xLength + x*dx, 0.0, zPos);
			meshData.Vertices[z*xNum + x].Color = XMFLOAT4(1.0, sin(x*dx / xLength), sin(x*dx / xLength), 1.0);
		}
	}

	UINT k = 0;
	for (UINT z = 0; z < zNum - 1; z++) {
		for (UINT x = 0; x < xNum - 1; x++) {
			meshData.Indices[k] = z*xNum + x;
			meshData.Indices[k + 1] = z*xNum + x + 1;
			meshData.Indices[k + 2] = (z + 1)*xNum + x;
			meshData.Indices[k + 3] = (z + 1)*xNum + x;
			meshData.Indices[k + 4] = z*xNum + x + 1;
			meshData.Indices[k + 5] = (z + 1)*xNum + x+1;
			k += 6;
		}
	}
}

void GeometryGenerator::CreateHillsMeshData(float xLength, float zLength, UINT xNum, UINT zNum, MeshData& meshData)
{
	CreatePlaneMeshData(xLength, zLength, xNum, zNum, meshData);
	for (size_t i = 0; i < meshData.Vertices.size(); ++i)
	{
		XMFLOAT3 p = meshData.Vertices[i].Position;
		XMFLOAT4 color;

		p.y = 0.3f*(p.z*sinf(0.1f*p.x) + p.x*cosf(0.1f*p.z));

		meshData.Vertices[i].Position = p;

		// Color the vertex based on its height.
		if (p.y < -10.0f)
		{
			// Sandy beach color.
			color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (p.y < 5.0f)
		{
			// Light yellow-green.
			color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (p.y < 12.0f)
		{
			// Dark yellow-green.
			color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (p.y < 20.0f)
		{
			// Dark brown.
			color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// White snow.
			color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		meshData.Vertices[i].Color = color;
	}
}
