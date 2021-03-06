#pragma once

#include "textureclass.h"

class TextureManagerClass
{
public:

	TextureManagerClass();
	TextureManagerClass(const TextureManagerClass&);
	~TextureManagerClass();

	bool Initialize(int);
	void Shutdown();

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*, int);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, WCHAR*, int);

	ID3D11ShaderResourceView* GetTexture(int);

private:

	TextureClass* m_TextureArray;
	int m_textureCount;
};