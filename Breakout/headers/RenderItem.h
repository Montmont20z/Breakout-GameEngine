#pragma once
#include <string>
#include <d3dx9.h>


struct RenderItem {
	std::string texturePath;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	float rotation;
	D3DCOLOR color;
	int renderOrder; // Lower values render first (background), higher value render last (foreground)
	bool visible;

	RenderItem()
		: position(0, 0, 0)
		, scale(1.0f, 1.0f, 1.0f)
		, rotation(0.0f)
		, color(D3DCOLOR_XRGB(255,255,255))
		, renderOrder(0)
		, visible(true) {}

	RenderItem(const std::string& path, const D3DXVECTOR3& pos, int order = 0)
		: texturePath(path)
		, position(pos)
		, scale(1.0f, 1.0f, 1.0f)
		, rotation(0.0f)
		, color(D3DCOLOR_XRGB(255,255,255))
		, renderOrder(order)
		, visible(true) {}
};