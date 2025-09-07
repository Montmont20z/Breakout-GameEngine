#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "MyWindow.h"
#include "TextureData.h"
#include "SpriteInstance.h"



class Renderer {
public:
	//std::vector<SpriteInstance> renderQueue;

	Renderer();
	~Renderer();
	bool Initialize(HWND hWnd, int width, int height);
	void Shutdown();
	int LoadTexture(const std::string& path, int logicalWidth = 0, int logicalHeight = 0);
	//bool LoadTexturesBatch(const std::vector<std::string>& textureList);
	// Update animation
	//void Update(float deltaTime);

	// Render Queue
	//void AddRenderItem(const SpriteInstance& item);
	//void RemoveRenderItem(const std::string& texturePath, const D3DXVECTOR3& position);
	//void RemoveRenderItemById(int id);
	//void ClearRenderQueue();
	//void UpdateRenderItem(const SpriteInstance& newItem);
	//void UpdateRenderItemById(int id, const SpriteInstance& newItem);

	//void Render();
	//IDirect3DDevice9* GetDevice();
	void BeginFrame();
	void EndFrame();
	void DrawSprite(const SpriteInstance& sprite);

	int CreateSolidTexture(D3DCOLOR argb);

private:
	D3DPRESENT_PARAMETERS m_d3dPP = {};
	IDirect3D9* m_direct3D9 = nullptr;
	IDirect3DDevice9* m_d3dDevice = nullptr;
	ID3DXSprite* m_spriteBrush = nullptr;
	std::unordered_map<int, TextureData> m_texturesById; // id -> texture
	std::unordered_map<std::string, int> m_texturesIdByPath; // path -> id
	int m_nextTexId = 0;


	//void SortRenderQueue();

};


