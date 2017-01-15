#include "Particlesystemclass.h"

bool ParticleSystemClass::InitializeParticleSystem()
{
	m_particleDeviationX = 5.0f;
	m_particleDeviationY = 5.0f;
	m_particleDeviationZ = 5.0f;

	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	m_particleSize = 0.5f;

	m_particlesPerSecond = 250.0f;

	m_maxParticles = 100;

	m_particleList = new ParticleType[m_maxParticles];
	if (!m_particleList)
	{
		return false;
	}

	for (int i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	m_currentParticleCount = 0;

	m_accumulatedTime = 0.0f;

	return true;
}

void ParticleSystemClass::ShutdownParticleSystem()
{
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = nullptr;
	}

	return;
}

bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = m_maxParticles * 6;
	m_indexCount = m_vertexCount;

	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] indices;
	indices = 0;

	return true;
}

void ParticleSystemClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}

void ParticleSystemClass::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	int i, j, index;
	float positionX, positionY, positionZ, velocity, red, green, blue;

	m_accumulatedTime += frameTime;

	emitParticle = false;

	//Check to see if a new particle should be added or not
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;
		
		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		//For blending purposes the particle array have to be sorted using the depth, so this will find where in the list the particle should be inserted
		index = 0;
		found = false;
		while (!found)
		{
			if ((m_particleList[index].active == false) || (m_particleList[index].positionz < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		//Copy the array over by one position from the index to make room for new particle
		i = m_currentParticleCount;
		j = i - 1;

		while (i != index)
		{
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionz = m_particleList[j].positionz;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}

		//Insert into the correct depth order
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionz = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;
	}
}

void ParticleSystemClass::UpdateParticles(float frameTime)
{
	for (int i = 0; i < m_currentParticleCount; i++)
	{
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
	}

	return;
}

void ParticleSystemClass::KillParticles()
{
	//Kills any partticle that has dropped below the given height range from their original position
	for (int i = 0; i < m_maxParticles; i++)
	{
		if ((m_particleList[i].active == true) && (m_particleList[i].positionY < -3.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			for (int j = i; j < m_maxParticles - 1; j++)
			{
				m_particleList[j].positionX = m_particleList[j + 1].positionX;
				m_particleList[j].positionY = m_particleList[j + 1].positionY;
				m_particleList[j].positionz = m_particleList[j + 1].positionz;
				m_particleList[j].red = m_particleList[j + 1].red;
				m_particleList[j].green = m_particleList[j + 1].green;
				m_particleList[j].blue = m_particleList[j + 1].blue;
				m_particleList[j].velocity = m_particleList[j + 1].velocity;
				m_particleList[j].active = m_particleList[j + 1].active;
			}
		}

	}
}

bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	index = 0;

	for (int i = 0; i < m_currentParticleCount; i++)
	{
		//Bottom left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionz);
		m_vertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		
		index++;

		//Top left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionz);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		//Bottom right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionz);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		//Bottom right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionz);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		//Top left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionz);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		//Top right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionz);
		m_vertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	verticesPtr = (VertexType*)mappedResource.pData;

	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;

}

ParticleSystemClass::ParticleSystemClass()
{
	m_particleList = nullptr;
	m_vertices = nullptr;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}

ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass& other)
{

}

ParticleSystemClass::~ParticleSystemClass()
{

}

bool ParticleSystemClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	bool result;

	result = InitializeParticleSystem();
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystemClass::Shutdown()
{
	ShutdownBuffers();

	ShutdownParticleSystem();

	return;
}

bool ParticleSystemClass::Frame(float frameTime, ID3D11DeviceContext* deviceContext)
{
	bool result;

	//Check if any particles need killing
	KillParticles();

	//Check if need to add particle
	EmitParticles(frameTime);

	//Update movement
	UpdateParticles(frameTime);

	result = UpdateBuffers(deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

int ParticleSystemClass::GetIndexCount()
{
	return m_indexCount;
}
