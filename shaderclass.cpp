#include "shaderclass.h"

ShaderClass::ShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
	m_lightBuffer = nullptr;
	m_cameraBuffer = nullptr;
	m_lightPositionBuffer = nullptr;
}

ShaderClass::ShaderClass(const ShaderClass& other)
{

}

ShaderClass::~ShaderClass()
{

}

bool ShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Initialize the vertex and pixel shader
	result = InitializeShader(device, hwnd, L"VertexShader.hlsl", L"PixelShader.hlsl");
	if (!result)
	{
		return false;
	}
	return true;
}

void ShaderClass::Shutdown()
{
	//Shutdown the vertex and pixel shader as well as the related objects
	ShutdownShader();

	return;
}

bool ShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix,
	ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor,
	XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, ID3D11ShaderResourceView* normalMap, 
	ID3D11ShaderResourceView* pixelPositionMap, XMFLOAT3 lightPosition, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* lightPosMap)
{
	bool result;

	//Set the shader parameters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, diffuseColor, ambientColor,
		cameraPosition, specularColor, normalMap, pixelPositionMap, lightPosition, depthMap, lightPosMap);
	if (!result)
	{
		return false;
	}

	//Now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);
	return true;
}

bool ShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC lightPositionBufferDesc;

	//Initialize the pointer this function will use to null
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	//Compile the vertex shader code
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//If the shader failed to compile it should have writen something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}

		//If there was nothing in the error message then it simply could not find the shader file itself
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Compile the pixel shader code
	result = D3DCompileFromFile(psFilename, NULL, NULL, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}

		//If there was nothing in the error message then it simply could not find the file itself
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Create the vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//Create the pixel shader from the buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//Create the vertex input layout desc
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		
	};

	//Get a count of the element in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//Release the vertex shader buffer and pixel shader buffer since they no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//Setup the desc of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//Create the constant buffer pointer so we can access the vertex shader constant buffer form within this class
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Setup the desc of the dynamic light constatnt buffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	//Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Setup the desc of the dynamic light constatnt buffer
	lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
	lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.MiscFlags = 0;
	lightPositionBufferDesc.StructureByteStride = 0;

	//Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class
	result = device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_lightPositionBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Create the texture sampler state desc
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the texture sampler state
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void ShaderClass::ShutdownShader()
{
	//Release stuff
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = nullptr;
	}

	if (m_lightPositionBuffer)
	{
		m_lightPositionBuffer->Release();
		m_lightPositionBuffer = nullptr;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}
	return;
}

void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* S)
{
	char* compileError;
	unsigned long long bufferSize;
	std::ofstream fout;

	//Get a pointer to the error message text buffer
	compileError = (char*)(errorMessage->GetBufferPointer());

	//Get the length of the error message
	bufferSize = errorMessage->GetBufferSize();

	//Open a file to write the error message
	fout.open("Shader_error.txt");

	//Write out the error message
	for (unsigned long long i = 0; i < bufferSize; i++)
	{
		fout << compileError[i];
	}

	//Close the file
	fout.close();

	//Release the error message
	errorMessage->Release();
	errorMessage = nullptr;

	//Pop a message to the screen to notify the user to check text file
	MessageBox(hwnd, L"Error compiling shader. Check shader_error.txt", S, MB_OK);

	return;
}

bool ShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix,
	XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor,
	XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, ID3D11ShaderResourceView* normalMap, 
	ID3D11ShaderResourceView* pixelPositionMap, XMFLOAT3 lightPosition, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* lightPosMap)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataptr;
	LightBufferType* dataptr2;
	CameraBufferType* dataptr3;
	LightPositionBufferType* dataptr4;
	
	unsigned int bufferNumber = 0;

	//Transpose the matrices to prepare them for the shader
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

	//Lock the constant buffer so it can be written to
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	dataptr = (MatrixBufferType*)mappedResource.pData;
	
	//Copy the matrices into the constant buffer
	dataptr->world = worldMatrix;
	dataptr->view = viewMatrix;
	dataptr->projection = projectionMatrix;

	//Unlock the constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	//Set the position of the constant buffer to the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &normalMap);
	deviceContext->PSSetShaderResources(2, 1, &pixelPositionMap);
	deviceContext->PSSetShaderResources(3, 1, &depthMap);
	deviceContext->PSSetShaderResources(4, 1, &lightPosMap);

	//Lock the light constant buffer so it can be written to
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	dataptr2 = (LightBufferType*)mappedResource.pData;

	//Copy the lighting variables into the constant buffer
	dataptr2->diffuseColor = diffuseColor;
	dataptr2->ambientColor = ambientColor;
	dataptr2->specularColor = specularColor;

	//Unlock the constant buffer
	deviceContext->Unmap(m_lightBuffer, 0);

	//Set the position of the light constant buffer in the pixel shader
	bufferNumber = 0;

	//Finally set the light constant buffer in the pixel shader with the updated values
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	// Lock the camera constant buffer so it can be written to.
	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataptr3 = (CameraBufferType*)mappedResource.pData;

	// Copy the camera position into the constant buffer.
	dataptr3->cameraPosition = cameraPosition;
	dataptr3->padding = 0.0f;

	// Unlock the camera constant buffer.
	deviceContext->Unmap(m_cameraBuffer, 0);

	// Set the position of the camera constant buffer in the vertex shader.
	bufferNumber = 1;

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	// Lock the lightPosition constant buffer so it can be written to.
	result = deviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataptr4 = (LightPositionBufferType*)mappedResource.pData;

	// Copy the light position into the constant buffer.
	dataptr4->lightPosition = lightPosition;
	dataptr4->padding = 1.0f;

	// Unlock the lightPosition constant buffer.
	deviceContext->Unmap(m_lightPositionBuffer, 0);

	//Set the position of the constant buffer in the vertex shader
	bufferNumber = 2;

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightPositionBuffer);

	return true;
}

void ShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//Set the sampler state in the pixle shader
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//Render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

