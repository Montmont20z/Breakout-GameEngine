#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include "MyWindow.h"
#include <unordered_map>
#include "TextureData.h"


class Renderer {
public:
	Renderer();
	~Renderer();

	bool Initialize(MyWindow myWindow);
	bool LoadTexture(const std::string& path);
	bool LoadTexturesBatch(const std::vector<std::string>& textureList);
	void Render();
	IDirect3DDevice9* GetDevice();

private:
	D3DPRESENT_PARAMETERS m_d3dPP = {};
	IDirect3D9* m_direct3D9 = nullptr;
	IDirect3DDevice9* m_d3dDevice = nullptr;
	std::unordered_map<std::string, TextureData> m_preloadedTextures = {};
	ID3DXSprite* m_spriteBrush = nullptr;




};


