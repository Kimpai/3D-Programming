#include "deferredshaderclass.h"

bool DeferredShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, WCHAR* gsFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	ID3D10Blob* geometryShaderBuffer;
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//Initialize the pointers this function will use to null
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;
	geometryShaderBuffer = nullptr;

	//Compile the vertex shader code
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "DVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		//If there was nothing in the error message then it could simply not find the shader file
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Compile the geometry shader code
	result = D3DCompileFromFile(gsFilename, NULL, NULL, "DGS", "gs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&geometryShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, gsFilename);
		}
		//If there was nothing in the error message then it could simply not find the shader file
		else
		{
			MessageBox(hwnd, gsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Compile the pixel shader code
	result = D3DCompileFromFile(psFilename, NULL, NULL, "DPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
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

	//Create the geometry shader from the buffer
	result = device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &m_geometryShader);
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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//Get a count of the vertex input layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//Release the vertex shader buffer and pixel shader buffer since they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	geometryShaderBuffer->Release();
	geometryShaderBuffer = nullptr;

	//Create a wrap texture state desc
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;

}

void DeferredShaderClass::ShutdownShader()
{
	//Release stuff
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_sampleStateWrap)
	{
		m_sampleStateWrap->Release();
		m_sampleStateWrap = nullptr;
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

	if (m_geometryShader)
	{
		m_geometryShader->Release();
		m_geometryShader = nullptr;
	}
}

void DeferredShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileError;
	unsigned long bufferSize;
	std::ofstream fout;

	// Get a pointer to the error message text buffer.
	compileError = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader_error.txt");

	// Write out the error message.
	for (int i = 0; i<bufferSize; i++)
	{
		fout << compileError[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader_error.txt for message.", shaderFilename, MB_OK);

	return;

}

bool DeferredShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix, 
	XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalTexture, XMMATRIX& lightViewMatrix, XMMATRIX& lightProjectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataptr;

	//Transpose the matrices to prepare them for the shader
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	lightProjectionMatrix = XMMatrixTranspose(lightProjectionMatrix);

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
	dataptr->lightView = lightViewMatrix;
	dataptr->lightProjection = lightProjectionMatrix;

	//Unlock the constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	//Set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	//Now set the constant buffer in the vertex shader
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//Set the shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &normalTexture);

	return true;
}

void DeferredShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	//Set the vertex and pixel shader that will be used to render
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->GSSetShader(m_geometryShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//Set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);

	//Render the geometry
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

DeferredShaderClass::DeferredShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_sampleStateWrap = nullptr;
	m_matrixBuffer = nullptr;
	m_geometryShader = nullptr;
}

DeferredShaderClass::DeferredShaderClass(const DeferredShaderClass& other)
{

}

DeferredShaderClass::~DeferredShaderClass()
{

}

bool DeferredShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Initialize the vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"DeferredVertexShader.hlsl", L"DeferredPixelShader.hlsl", L"DeferredGeometryShader.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void DeferredShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool DeferredShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix, 
	XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, 
	ID3D11ShaderResourceView* normalTexture, XMMATRIX& lightViewMatrix, XMMATRIX& lightProjectionMatrix)
{
	bool result;

	//Set the shaderr parameters that will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, normalTexture, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	//Now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}
