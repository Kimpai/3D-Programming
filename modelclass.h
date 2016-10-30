#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <vector>

using namespace DirectX;

class ModelClass
{
private:

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT4 color;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float cx, cy, cz, cw;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadModel(char*);
	void ReleaseModel();
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, XMFLOAT3&, XMFLOAT3&);
	void CalculateNormal(XMFLOAT3, XMFLOAT3, XMFLOAT3&);

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount, m_faceCount, m_textureCount, m_normalCount;
	ModelType* m_model;

public:

	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	int GetIndexCount();
};