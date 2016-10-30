#pragma once

//INCLUDES//
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;

class ShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world, view, projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor; 
		XMFLOAT4 specularColor;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct LightPositionBufferType
	{
		XMFLOAT3 lightPosition;
		float padding;
	};

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX&, XMMATRIX&, XMMATRIX&, ID3D11ShaderResourceView*, XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightPositionBuffer;

public:
	ShaderClass();
	ShaderClass(const ShaderClass&);
	~ShaderClass();
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX&, XMMATRIX&, XMMATRIX&, ID3D11ShaderResourceView*, XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);

};