#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <stdio.h>
#include "terraincellclass.h"

using namespace DirectX;
using namespace std;

class TerrainClass
{
private:
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float r, g, b;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float r, g, b;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	bool LoadSetupFile(char*);
	bool LoadBitmapHeightMap();
	void ShutdownHeightMap();
	void SetTerrainCoordinates();
	bool CalculateNormals();
	bool LoadColorMap();
	bool BuildTerrainModel();
	void ShutdownTerrainModel();
	void CalculateTerrainVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, XMFLOAT3&, XMFLOAT3&);
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);
	bool LoadTerrainCells(ID3D11Device*);
	void ShutdownTerrainCells();

	int m_terrainHeight, m_terrainWidth, m_vertexCount;
	float m_heightScale;
	char *m_terrainFilename, *m_colorMapFilename;
	HeightMapType* m_heightMap;
	ModelType* m_terrainModel;
	TerrainCellClass* m_TerrainCells;
	int m_cellCount;

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();
	bool Initialize(ID3D11Device*, char*);
	void Shutdown();
	bool RenderCell(ID3D11DeviceContext*, int);
	void RenderCellLines(ID3D11DeviceContext*, int);
	int GetCellIndexCount(int);
	int GetCellLinesIndexCount(int);
	int GetCellCount();
	bool GetHeightAtPosition(float, float, float&);
};