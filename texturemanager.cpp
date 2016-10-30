#include "texturemanager.h"

TextureManagerClass::TextureManagerClass()
{
	m_TextureArray = nullptr;
}

TextureManagerClass::TextureManagerClass(const TextureManagerClass& other)
{

}

TextureManagerClass::~TextureManagerClass()
{

}

bool TextureManagerClass::Initialize(int count)
{
	m_textureCount = count;

	//Create the texture object
	m_TextureArray = new TextureClass[m_textureCount];
	if (!m_TextureArray)
	{
		return false;
	}

	return true;
}

void TextureManagerClass::Shutdown()
{
	if (m_TextureArray)
	{

		for (int i = 0; i < m_textureCount; i++)
		{
			m_TextureArray[i].Shutdown();
		}

		delete[] m_TextureArray;
		m_TextureArray = nullptr;
	}

	return;
}

bool TextureManagerClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, int location)
{
	bool result;

	//Initialize the texture object
	result = m_TextureArray[location].Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextureManagerClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* filename, int location)
{
	bool result;

	//Initialize the texture object
	result = m_TextureArray[location].Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView* TextureManagerClass::GetTexture(int id)
{
	return m_TextureArray[id].GetTexture();
}
