#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "MyWindow.h"
#include "TextureData.h"
#include "RenderItem.h"


class Renderer {
public:
	Renderer();
	~Renderer();
	bool Initialize(HWND hWnd, int width, int height);
	bool LoadTexture(const std::string& path);
	bool LoadTexturesBatch(const std::vector<std::string>& textureList);

	// Render Queue
	void AddRenderItem(const RenderItem& item);
	void RemoveRenderItem(const std::string& texturePath, const D3DXVECTOR3& position);
	void ClearRenderQueue();
	void UpdateRenderItem(const std::string& texturePath, const D3DXVECTOR3& oldPos, const RenderItem& newItem);

	void Render();
	IDirect3DDevice9* GetDevice();

private:
	D3DPRESENT_PARAMETERS m_d3dPP = {};
	IDirect3D9* m_direct3D9 = nullptr;
	IDirect3DDevice9* m_d3dDevice = nullptr;
	std::unordered_map<std::string, TextureData> m_preloadedTextures = {};
	ID3DXSprite* m_spriteBrush = nullptr;

	std::vector<RenderItem> m_renderQueue;

	void SortRenderQueue();

};


