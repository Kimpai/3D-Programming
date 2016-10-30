#pragma once

#include <d3d11.h>
#include <stdio.h>
#include "DDSTextureLoader.h"

class TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;

	};

	bool LoadTarga(char*, int&, int&);

	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
};