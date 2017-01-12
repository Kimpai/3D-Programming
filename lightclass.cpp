#include "lightclass.h"

LightClass::LightClass()
{

}

LightClass::LightClass(const LightClass& other)
{

}

LightClass::~LightClass()
{

}

void LightClass::Initialize(float screenDepth, float screenNear, int screenHeight, int screenWidth)
{	
	float fieldOfView, screenAspect;
	XMVECTOR position, up, lookAt;

	//Initialize color and poition
	m_diffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_ambientColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_position = XMFLOAT3(50.0f, 1.0f, 40.0f);
	m_lookAt = XMFLOAT3(50.0f, 1.0f, 50.0f);
	m_specularColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

	//Setup the projection matrix
	fieldOfView = XM_PI / 2.0f;
	screenAspect = 1.0f;

	//Create the projection matrix
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//Setup view matrix
	position = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	position = XMLoadFloat3(&m_position);
	lookAt = XMLoadFloat3(&m_lookAt);

	m_viewMatrix = DirectX::XMMatrixLookAtLH(position, lookAt, up);

}

XMFLOAT4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}

XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT3 LightClass::GetPosition()
{
	return m_position;
}

XMFLOAT4 LightClass::GetSpecularColor()
{
	return m_specularColor;
}

void LightClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void LightClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void LightClass::UpdateViewMatrix()
{
	XMVECTOR position, up, lookAt;

	//Setup view matrix
	position = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	up = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	position = XMLoadFloat3(&m_position);
	up = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
	lookAt = XMLoadFloat3(&m_lookAt);

	m_viewMatrix = DirectX::XMMatrixLookAtLH(position, lookAt, up);
}