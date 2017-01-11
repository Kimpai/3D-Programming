#pragma once

#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "shaderclass.h"
#include "lightclass.h"
#include "terrainclass.h"
#include "texturemanager.h"
#include "deferredbuffersclass.h"
#include "deferredshaderclass.h"
#include "orthowindowclass.h"
#include "simpleshaderclss.h"
#include "inputclass.h"
#include "depthMap.h"
#include "particleshaderclass.h"
#include "Particlesystemclass.h"

//GLOBALS//
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const int SHADOWMAP_HEIGHT = 1024;
const int SHADOWMAP_WIDTH = 1024;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass*, float, float, float, float, float, float, float);
	bool Render();
	bool RenderSceneToQaud();
	bool RenderDepth();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ShaderClass* m_Shader;
	LightClass* m_Light;
	TerrainClass* m_Terrain;
	TextureManagerClass* m_TextureManager;
	DeferredBuffersClass* m_DeferredBuffers;
	DepthMapClass* m_DepthMap;
	DeferredShaderClass* m_DeferredShader;
	OrthoWindowClass* m_FullScreenWindow;
	SimpleShaderClass* m_SimpleShader;
	ParticleShaderClass* m_ParticleShader;
	ParticleSystemClass* m_ParticleSystem;
	bool m_cellLines, m_heightLocked;
};
