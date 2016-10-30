#pragma once

//INCLUDES//
#include <DirectXMath.h>

using namespace DirectX;

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void Initialize(int, int, float, float);

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(DirectX::XMMATRIX&);
	void RenderBaseViewMatrix();
	void GetBaseViewMatrix(DirectX::XMMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix, m_baseViewMatrix;
	
};