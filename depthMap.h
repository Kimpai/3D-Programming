#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

class DepthMapClass
{
public:
	DepthMapClass();
	DepthMapClass(const DepthMapClass&);
	~DepthMapClass();

	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	int m_textureWidth, m_textureHeight;
	ID3D11Texture2D* m_renderTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	D3D11_VIEWPORT m_viewport;
};