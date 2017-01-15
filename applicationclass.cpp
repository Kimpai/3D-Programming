#include "applicationclass.h"

ApplicationClass::ApplicationClass()
{
	m_D3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_Shader = nullptr;
	m_Light = nullptr;
	m_Terrain = nullptr;
	m_TextureManager = nullptr;
	m_SimpleShader = nullptr;
	m_DepthMap = nullptr;
	m_ParticleShader = nullptr;
	m_ParticleSystem = nullptr;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{

}

ApplicationClass::~ApplicationClass()
{

}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	//Create the Direct3D object
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}
	
	//Initialize the Direct3D object
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN,
		SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_D3D->TurnOffCulling();

	//Create the camera object
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	m_Camera->Initialize(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//Create the texture manager object
	m_TextureManager = new TextureManagerClass;
	if (!m_TextureManager)
	{
		return false;
	}

	result = m_TextureManager->Initialize(10);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture manager", L"Error", MB_OK);
		return false;
	}

	result = m_TextureManager->LoadTexture(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), "dirt01d.tga", 0);
	if (!result)
	{
		return false;
	}

	result = m_TextureManager->LoadTexture(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), "dirt01n.tga", 1);
	if (!result)
	{
		return false;
	}

	result = m_TextureManager->LoadTexture(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), L"stone01d.dds", 2);
	if (!result)
	{
		return false;
	}

	result = m_TextureManager->LoadTexture(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), L"stone01n.dds", 3);
	if (!result)
	{
		return false;
	}

	result = m_TextureManager->LoadTexture(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), L"fire01.dds", 4);
	if (!result)
	{
		return false;
	}

	//Create the model object
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	//Initialzie the model object
	result = m_Model->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), "cube.obj");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initalize the model object", L"Error", MB_OK);
		return false;
	}

	m_Terrain = new TerrainClass;
	if (!m_Terrain)
	{
		return false;
	}

	result = m_Terrain->Initialize(m_D3D->GetDevice(), "setup.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object", L"Error", MB_OK);
		return false;
	}

	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow)
	{
		return false;
	}

	result = m_FullScreenWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object", L"Error", MB_OK);
		return false;
	}


	m_DeferredBuffers = new DeferredBuffersClass;
	if (!m_DeferredBuffers)
	{
		return false;
	}

	result = m_DeferredBuffers->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the deferred buffers object", L"Error", MB_OK);
		return false;
	}

	m_DepthMap = new DepthMapClass;
	if (!m_DepthMap)
	{
		return false;
	}

	result = m_DepthMap->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the depth map object", L"Error", MB_OK);
		return false;
	}

	m_DeferredShader = new DeferredShaderClass;
	if (!m_DeferredShader)
	{
		return false;
	}

	result = m_DeferredShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the deferred shader object", L"Error", MB_OK);
		return false;
	}

	m_SimpleShader = new SimpleShaderClass;
	if (!m_SimpleShader)
	{
		return false;
	}

	result = m_SimpleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the simple shader object", L"Error", MB_OK);
		return false;
	}

	//Create the shader object
	m_Shader = new ShaderClass;
	if (!m_Shader)
	{
		return false;
	}

	//Initialize the shader object
	result = m_Shader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initalize the shader object", L"Error", MB_OK);
		return false;
	}

	m_ParticleShader = new ParticleShaderClass;
	if (!m_ParticleShader)
	{
		return false;
	}

	result = m_ParticleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the particle shader object", L"Error", MB_OK);
		return false;
	}

	m_ParticleSystem = new ParticleSystemClass;
	if (!m_ParticleSystem)
	{
		return false;
	}

	result = m_ParticleSystem->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	//Create the light object
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	m_Light->Initialize(SCREEN_DEPTH, SCREEN_NEAR, screenHeight, screenWidth);

	//Set the rendering of cell lines to enabled
	m_cellLines = true;

	//Set the user locked to the terrain height for movement
	m_heightLocked = true;

	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_ParticleSystem)
	{
		m_ParticleSystem->Shutdown();
		delete m_ParticleSystem;
		m_ParticleSystem = nullptr;
	}
	if (m_ParticleShader)
	{
		m_ParticleShader->Shutdown();
		delete m_ParticleShader;
		m_ParticleShader = nullptr;
	}
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = nullptr;
	}
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}
	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = nullptr;
	}
	if (m_Shader)
	{
		m_Shader->Shutdown();
		delete m_Shader;
		m_Shader = nullptr;
	}

	if (m_Light)
	{
		delete m_Light;
		m_Light = nullptr;
	}

	if (m_DeferredShader)
	{
		m_DeferredShader->Shutdown();
		delete m_DeferredShader;
		m_DeferredShader = nullptr;
	}

	if (m_SimpleShader)
	{
		m_SimpleShader->Shutdown();
		delete m_SimpleShader;
		m_SimpleShader = nullptr;
	}

	if (m_DeferredBuffers)
	{
		m_DeferredBuffers->Shutdown();
		delete m_DeferredBuffers;
		m_DeferredBuffers = nullptr;
	}

	if (m_DepthMap)
	{
		m_DepthMap->Shutdown();
		delete m_DepthMap;
		m_DepthMap = nullptr;
	}

	if (m_FullScreenWindow)
	{
		m_FullScreenWindow->Shutdown();
		delete m_FullScreenWindow;
		m_FullScreenWindow = nullptr;
	}

	return;
}

bool ApplicationClass::Frame(InputClass* Input, float frameTime, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
	bool result, foundHeight;
	float height;

	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	//If the height is locked to the terrain then position the camera on top of it
	if (m_heightLocked)
	{
		//Get the height of the triangle that is directly underneath the given camera position
		foundHeight = m_Terrain->GetHeightAtPosition(posX, posZ, height);
		if (foundHeight)
		{
			//If there was a triangle beneath the camera then position the camera just abouve it by on meter
			Input->UpdatePosition(posX, height + 0.01, posZ);
			m_Camera->SetPosition(posX, height + 0.01, posZ);
		}
	}

	//Generate the view matrix based on the camera's position and update it every frame
	m_Camera->Render();

	m_ParticleSystem->Frame(frameTime, m_D3D->GetDeviceContext());

	//Render the scene
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool ApplicationClass::Render()
{
	DirectX::XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	bool result;

	//Render the scene onto the 2D qaud
	result = RenderSceneToQaud();
	if (!result)
	{
		return false;
	}

	//Clear the scene
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Get the world, view and projection matrices from the camera and d3d object
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//Turn off the Z buffer to begin all 2D rendering
	m_D3D->TurnZBufferOff();

	//Put the full screen ortho window buffers on the graphics pipeline to prepare them for drawing
	m_FullScreenWindow->Render(m_D3D->GetDeviceContext());

	//Render the full screen ortho window using the deferred render buffers
	m_Shader->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_DeferredBuffers->GetShaderResourceView(0), m_Light->GetDiffuseColor(),
		m_Light->GetAmbientColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), 
		m_DeferredBuffers->GetShaderResourceView(1), 
		m_DeferredBuffers->GetShaderResourceView(2), m_Light->GetPosition(), 
		m_DepthMap->GetShaderResourceView(), m_DeferredBuffers->GetShaderResourceView(3));

	//Turn the Z buffer back on now that all 2D rendering has completed
	m_D3D->TurnZBufferOn();

	//Present the rendered scene to the screen
	m_D3D->EndScene();

	return true;
}

bool ApplicationClass::RenderSceneToQaud()
{
	bool result;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix;
	XMFLOAT3 cameraPosition, particlePosition;
	float angley, anglex, anglez;

	//Set the render buffers to be the render target
	m_DeferredBuffers->SetRenderTarget(m_D3D->GetDeviceContext());

	//Clear the render buffers
	m_DeferredBuffers->ClearRenderTargets(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < m_Terrain->GetCellCount(); i++)
	{
		//Get the matrices from the camera and d3d objects
		m_D3D->GetWorldMatrix(worldMatrix);
		m_Camera->GetViewMatrix(viewMatrix);
		m_D3D->GetProjectionMatrix(projectionMatrix);
		m_Light->GetViewMatrix(lightViewMatrix);
		m_Light->GetProjectionMatrix(lightProjectionMatrix);

		//Render terrain object
		worldMatrix = XMMatrixScaling(1.0, 1.0, 1.0) * XMMatrixTranslation(0.0f, -10.0f, 0.0);

		result = m_Terrain->RenderCell(m_D3D->GetDeviceContext(), i);
		if (!result)
		{
			return false;
		}

		result = m_DeferredShader->Render(m_D3D->GetDeviceContext(), m_Terrain->GetCellIndexCount(i), worldMatrix, viewMatrix, projectionMatrix,
			m_TextureManager->GetTexture(0), m_TextureManager->GetTexture(1), lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
		if (!result)
		{
			return false;
		}

	}

	//Reset the matrices
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	//Render model object
	worldMatrix = (XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(50.0f, 0.0f, 50.0f));

	m_Model->Render(m_D3D->GetDeviceContext());

	result = m_DeferredShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
		m_TextureManager->GetTexture(2), m_TextureManager->GetTexture(3), lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
	if (!result)
	{
		return false;
	}

	//Reset the matrices
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	cameraPosition = m_Camera->GetPosition();

	//Set the position of the particles
	particlePosition.x = 100.0f;
	particlePosition.y = 1.0f;
	particlePosition.z = 50.0f;

	angley = atan2(particlePosition.x - cameraPosition.x, particlePosition.z - cameraPosition.z);
	anglex = atan2(particlePosition.y - cameraPosition.y, particlePosition.z - cameraPosition.z);

	worldMatrix = (XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixRotationRollPitchYaw(-anglex, angley, 0.0f) * XMMatrixTranslation(particlePosition.x, particlePosition.y, particlePosition.z));

	m_ParticleSystem->Render(m_D3D->GetDeviceContext());

	result = m_ParticleShader->Render(m_D3D->GetDeviceContext(), m_ParticleSystem->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_TextureManager->GetTexture(4));
	if (!result)
	{
		return false;
	}

	//Reset the render target back to the original back buffer and not the render buffers anymore
	m_D3D->SetBackBufferRenderTarget();

	//Reset the viewport back to the original
	m_D3D->ResetViewPort();

	return true;
}

bool ApplicationClass::RenderDepth()
{
	bool result;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//Set the render buffers to be the render target
	m_DepthMap->SetRenderTarget(m_D3D->GetDeviceContext());

	//Clear the render buffers
	m_DepthMap->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//Render the terrain object
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0, -20.0, 0.0);

	for (int i = 0; i < m_Terrain->GetCellCount(); i++)
	{
		//Get the matrices from the d3d and light object
		m_D3D->GetWorldMatrix(worldMatrix);
		m_Light->GetViewMatrix(viewMatrix);
		m_Light->GetProjectionMatrix(projectionMatrix);

		result = m_Terrain->RenderCell(m_D3D->GetDeviceContext(), i);
		if (!result)
		{
			return false;
		}

		result = m_SimpleShader->Render(m_D3D->GetDeviceContext(), m_Terrain->GetCellIndexCount(i), worldMatrix, viewMatrix, projectionMatrix);
		if (!result)
		{
			return false;
		}
	}

	//Reset the matrices
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Light->GetViewMatrix(viewMatrix);
	m_Light->GetProjectionMatrix(projectionMatrix);

	//Render the model object
	worldMatrix = (XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(50.0f, -10.0f, 50.0f));

	m_Model->Render(m_D3D->GetDeviceContext());

	result = m_SimpleShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//Reset the render target back to the original back buffer and not the render buffers anymore
	m_D3D->SetBackBufferRenderTarget();

	//Reset the viewport back to the original
	m_D3D->ResetViewPort();

	return true;
}

