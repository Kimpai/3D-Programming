#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_model = nullptr;
	
}

ModelClass::ModelClass(const ModelClass& other)
{

}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename)
{
	bool result;

	//Load the model data
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	//Calculate the normal, tangent and binormal
	CalculateModelVectors();

	//Initilaize the vertex and index buffers
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	//Shutdown stuff
	ShutdownBuffers();
	ReleaseModel();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//Create the vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//Create the index array
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//Load the vertex and index array with data
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].color = XMFLOAT4(m_model[i].cx, m_model[i].cy, m_model[i].cz, m_model[i].cw);
		vertices[i].tangent = XMFLOAT3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = XMFLOAT3(m_model[i].bx, m_model[i].by, m_model[i].bz);
		
		indices[i] = i;
	}

	//Set up the desc of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresources struct a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Set up the desc of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give the subresource struct a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Release the arrays now that the data is copied in to the buffers
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	//Release the buffers
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

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride, offset;

	//Set the vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from our buffers
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadModel(char* filename)
{
	std::vector<VertexType> vertices, texcoords, normals;
	VertexType tmpVertices, tmpTexcoords, tmpNormals;
	std::vector<FaceType> faces;
	std::vector<char*> materials;
	FaceType tmpFaces;
	char materialFile[10];
	XMFLOAT4 materialColor;
	std::ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex;
	char input, input2;
	std::ofstream fout;

	//Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	//Open the file.
	fin.open(filename);

	//Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	//Read in the vertices, texture coordinates, and normals into the data structures.
	//Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			//Read in the vertices.
			if (input == ' ')
			{
				fin >> tmpVertices.position.x >> tmpVertices.position.y >> tmpVertices.position.z;

				//Invert the Z vertex to change to left hand system.
				tmpVertices.position.z = tmpVertices.position.z * -1.0f;
				vertices.push_back(tmpVertices);
				vertexIndex++;
			}

			//Read in the texture uv coordinates.
			if (input == 't')
			{
				fin >> tmpTexcoords.texture.x >> tmpTexcoords.texture.y;

				//Invert the V texture coordinates to left hand system.
				tmpTexcoords.texture.y = 1.0f - tmpTexcoords.texture.y;
				texcoords.push_back(tmpTexcoords);
				texcoordIndex++;
			}

			//Read in the normals.
			if (input == 'n')
			{
				fin >> tmpNormals.normal.x >> tmpNormals.normal.y >> tmpNormals.normal.z;

				//Invert the Z normal to change to left hand system.
				tmpNormals.normal.z = tmpNormals.normal.z * -1.0f;
				normals.push_back(tmpNormals);
				normalIndex++;
			}
		}

		//Read in the faces.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				//Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> tmpFaces.vIndex3 >> input2 >> tmpFaces.tIndex3 >> input2 >> tmpFaces.nIndex3
					>> tmpFaces.vIndex2 >> input2 >> tmpFaces.tIndex2 >> input2 >> tmpFaces.nIndex2
					>> tmpFaces.vIndex1 >> input2 >> tmpFaces.tIndex1 >> input2 >> tmpFaces.nIndex1;
				faces.push_back(tmpFaces);
				faceIndex++;
			}
		}

		if (input == 'm')
		{
			fin.get(input);
			if (input == 't')
			{
				fin.get(input);
				if (input == 'l')
				{
					fin.get(input);
					if (input == 'l')
					{
						fin.get(input);
						if (input == 'i')
						{
							fin.get(input);
							if (input == 'b')
							{
								fin.get(input);
								if (input == ' ')
								{

									fin >> materialFile;
								}
							}
						}
					}
				}
			}
		}

		//Read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		//Start reading the beginning of the next line.
		fin.get(input);
	}

	//Close the file.
	fin.close();

	//Open the file.
	fin.open(materialFile);

	//Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	fin.get(input);
	while (!fin.eof())
	{
		fin.get(input);
		if (input == '#')
		{
			fin.get(input);
			while (input != '\n')
			{
				fin.get(input);
			}
		}

		if (input == 'K')
		{
			fin.get(input);
			if (input == 'd')
			{
				fin.get(input);

				fin >> materialColor.x;
				fin >> materialColor.y;
				fin >> materialColor.z;
			}
		}

		if (input == 'T')
		{
			fin.get(input);
			if (input == 'r')
			{
				fin.get(input);
				
				fin >> materialColor.w;
			}
		}

	}

	//Close the file.
	fin.close();

	m_vertexCount = faces.size() * 3;
	m_indexCount = m_vertexCount;

	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	//Now loop through all the faces and output the three vertices for each face.
	for (int i = 0, j = 0; i < faces.size(); i++, j+=3)
	{

		m_model[j].x = vertices[faces[i].vIndex1 - 1].position.x;
		m_model[j].y = vertices[faces[i].vIndex1 - 1].position.y;
		m_model[j].z = vertices[faces[i].vIndex1 - 1].position.z;
		m_model[j].tu = texcoords[faces[i].tIndex1 - 1].texture.x;
		m_model[j].tv = texcoords[faces[i].tIndex1 - 1].texture.y;
		m_model[j].cx = materialColor.x;
		m_model[j].cy = materialColor.y;
		m_model[j].cz = materialColor.z;
		m_model[j].cw = materialColor.w;
		m_model[j].nx = normals[faces[i].nIndex1 - 1].normal.x;
		m_model[j].ny = normals[faces[i].nIndex1 - 1].normal.y;
		m_model[j].nz = normals[faces[i].nIndex1 - 1].normal.z;

		m_model[j + 1].x = vertices[faces[i].vIndex2 - 1].position.x;
		m_model[j + 1].y = vertices[faces[i].vIndex2 - 1].position.y;
		m_model[j + 1].z = vertices[faces[i].vIndex2 - 1].position.z;
		m_model[j + 1].tu = texcoords[faces[i].tIndex2 - 1].texture.x;
		m_model[j + 1].tv = texcoords[faces[i].tIndex2 - 1].texture.y;
		m_model[j + 1].cx = materialColor.x;
		m_model[j + 1].cy = materialColor.y;
		m_model[j + 1].cz = materialColor.z;
		m_model[j + 1].cw = materialColor.w;
		m_model[j + 1].nx = normals[faces[i].nIndex3 - 1].normal.x;
		m_model[j + 1].ny = normals[faces[i].nIndex3 - 1].normal.y;
		m_model[j + 1].nz = normals[faces[i].nIndex3 - 1].normal.z;

		m_model[j + 2].x = vertices[faces[i].vIndex3 - 1].position.x;
		m_model[j + 2].y = vertices[faces[i].vIndex3 - 1].position.y;
		m_model[j + 2].z = vertices[faces[i].vIndex3 - 1].position.z;
		m_model[j + 2].tu = texcoords[faces[i].tIndex3 - 1].texture.x;
		m_model[j + 2].tv = texcoords[faces[i].tIndex3 - 1].texture.y;
		m_model[j + 2].cx = materialColor.x;
		m_model[j + 2].cy = materialColor.y;
		m_model[j + 2].cz = materialColor.z;
		m_model[j + 2].cw = materialColor.w;
		m_model[j + 2].nx = normals[faces[i].nIndex3 - 1].normal.x;
		m_model[j + 2].ny = normals[faces[i].nIndex3 - 1].normal.y;
		m_model[j + 2].nz = normals[faces[i].nIndex3 - 1].normal.z;

	}

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = nullptr;
	}

	return;
}

void ModelClass::CalculateModelVectors()
{
	int faceCount, index;
	TempVertexType vertex1, vertex2, vertex3;
	XMFLOAT3 tangent, binormal, normal;

	//Calculate the number of faces in the model
	faceCount = m_vertexCount / 3;

	//Initialize the index to the model data
	index = 0;

	//Go through all the faces and calculate the tangentt, binormal and normal
	for (int i = 0; i < faceCount; i++)
	{
		//Get the three vertices for this face from the model
		vertex1.x = m_model[index].x;
		vertex1.y = m_model[index].y;
		vertex1.z = m_model[index].z;
		vertex1.tu = m_model[index].tu;
		vertex1.tv = m_model[index].tv;
		vertex1.nx = m_model[index].nx;
		vertex1.ny = m_model[index].ny;
		vertex1.nz = m_model[index].nz;
		index++;

		vertex2.x = m_model[index].x;
		vertex2.y = m_model[index].y;
		vertex2.z = m_model[index].z;
		vertex2.tu = m_model[index].tu;
		vertex2.tv = m_model[index].tv;
		vertex2.nx = m_model[index].nx;
		vertex2.ny = m_model[index].ny;
		vertex2.nz = m_model[index].nz;
		index++;

		vertex3.x = m_model[index].x;
		vertex3.y = m_model[index].y;
		vertex3.z = m_model[index].z;
		vertex3.tu = m_model[index].tu;
		vertex3.tv = m_model[index].tv;
		vertex3.nx = m_model[index].nx;
		vertex3.ny = m_model[index].ny;
		vertex3.nz = m_model[index].nz;
		index++;

		//Calculate the tangent and binormal of this face
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		//Calculate the new normal with the tangent and binormal
		CalculateNormal(tangent, binormal, normal);

		//Store the normal, tangent, and binormal for this face back in the model structure.
		m_model[index - 1].nx = normal.x;
		m_model[index - 1].ny = normal.y;
		m_model[index - 1].nz = normal.z;
		m_model[index - 1].tx = tangent.x;
		m_model[index - 1].ty = tangent.y;
		m_model[index - 1].tz = tangent.z;
		m_model[index - 1].bx = binormal.x;
		m_model[index - 1].by = binormal.y;
		m_model[index - 1].bz = binormal.z;

		m_model[index - 2].nx = normal.x;
		m_model[index - 2].ny = normal.y;
		m_model[index - 2].nz = normal.z;
		m_model[index - 2].tx = tangent.x;
		m_model[index - 2].ty = tangent.y;
		m_model[index - 2].tz = tangent.z;
		m_model[index - 2].bx = binormal.x;
		m_model[index - 2].by = binormal.y;
		m_model[index - 2].bz = binormal.z;

		m_model[index - 3].nx = normal.x;
		m_model[index - 3].ny = normal.y;
		m_model[index - 3].nz = normal.z;
		m_model[index - 3].tx = tangent.x;
		m_model[index - 3].ty = tangent.y;
		m_model[index - 3].tz = tangent.z;
		m_model[index - 3].bx = binormal.x;
		m_model[index - 3].by = binormal.y;
		m_model[index - 3].bz = binormal.z;

	}

	return;
}

void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, XMFLOAT3& tangent, XMFLOAT3& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;

	//Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	//Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	//Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	//Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	//Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	//Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	//Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;

}

void ModelClass::CalculateNormal(XMFLOAT3 tangent, XMFLOAT3 binormal, XMFLOAT3& normal)
{
	float length;

	//Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	//Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	//Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;

}
