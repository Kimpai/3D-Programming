#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void Initialize(float, float, int, int);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetPosition();
	XMFLOAT4 GetSpecularColor();
	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);
	void UpdateViewMatrix();

private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_position;
	XMFLOAT3 m_lookAt;
	XMFLOAT4 m_specularColor;
	XMMATRIX m_viewMatrix, m_projectionMatrix;
};