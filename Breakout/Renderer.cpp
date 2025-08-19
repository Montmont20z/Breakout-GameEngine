#include "headers/Renderer.h"
#include "headers/MyWindow.h"
#include <chrono>
#include <iostream>

Renderer::Renderer() = default;

Renderer::~Renderer() {
    // Clean up textures
    for (auto& kv : m_preloadedTextures) {
        if (kv.second.texture) {
            kv.second.texture->Release();
            kv.second.texture = nullptr;
        }
    }
    m_preloadedTextures.clear();

    if (m_spriteBrush) {
        m_spriteBrush->Release();
        m_spriteBrush = nullptr;
    }
    if (m_d3dDevice) {
        m_d3dDevice->Release();
        m_d3dDevice = nullptr;
    }
    if (m_direct3D9) {
        m_direct3D9->Release();
        m_direct3D9 = nullptr;
    }
}

bool Renderer::Initialize(HWND hWnd, int width, int height) {
    // Create Direct3D 9 object
    m_direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_direct3D9) {
        return false;
    }

    // Get display mode for fullscreen
    D3DDISPLAYMODE displayMode;
    if (FAILED(m_direct3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode))) {
        return false;
    }

    // Windowed settings
    m_d3dPP.Windowed = TRUE;
    m_d3dPP.BackBufferWidth = width;
    m_d3dPP.BackBufferHeight = height;
    m_d3dPP.BackBufferFormat = D3DFMT_UNKNOWN; // Use desktop format
    m_d3dPP.BackBufferCount = 1;
    m_d3dPP.hDeviceWindow = hWnd;
    m_d3dPP.EnableAutoDepthStencil = TRUE;
    m_d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dPP.AutoDepthStencilFormat = D3DFMT_D16;

    // Create device
    HRESULT hr = m_direct3D9->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, // Hardware = GPU
        &m_d3dPP,
        &m_d3dDevice
    );

    if (FAILED(hr)) {
        // Failback to software vertex processing
        hr = m_direct3D9->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hWnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, // Software = CPU
            &m_d3dPP,
            &m_d3dDevice
        );

        if (FAILED(hr)) {
            return false;
        }
    }

    //	Create sprite
    hr = D3DXCreateSprite(m_d3dDevice, &m_spriteBrush);
    if (FAILED(hr)) {
        MessageBoxW(nullptr, L"Failed to create sprite brush!", L"Error", MB_ICONERROR);
        return false;
    }

    return true;

}

bool Renderer::LoadTexture(const std::string& path, int logicalWidth, int logicalHeight) {
     // If already loaded, optionally update logical sizes
    auto it = m_preloadedTextures.find(path);
    if (it != m_preloadedTextures.end()) {
        if (logicalWidth > 0) it->second.logicalWidth = logicalWidth;
        if (logicalHeight > 0) it->second.logicalHeight = logicalHeight;
        return true;
    }

    TextureData textureData = {};
    textureData.filename = path;

     HRESULT hr = D3DXCreateTextureFromFileExA(
        m_d3dDevice,
        path.c_str(),
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        &textureData.info,
        nullptr,
        &textureData.texture
    );

    if (SUCCEEDED(hr) && textureData.texture) {
         // If caller provided logical sizes, use them. Otherwise default to actual texture size.
        textureData.logicalWidth = (logicalWidth > 0) ? logicalWidth : textureData.info.Width;
        textureData.logicalHeight = (logicalHeight > 0) ? logicalHeight : textureData.info.Height;

        m_preloadedTextures[path] = std::move(textureData);
        return true;
    }
    else {
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"Failed to load: %hs\nHRESULT: 0x%08X", path.c_str(), hr);
        MessageBoxW(nullptr, errorMsg, L"Error", MB_ICONERROR);
        return false;
    }
}

bool Renderer::LoadTexturesBatch(const std::vector<std::string>& textureList) {
    bool allSuccessful = true;

    for (const auto& texturePath : textureList) {
        if (!texturePath.empty()) {
            if (!LoadTexture(texturePath)) {
                allSuccessful = false;
            }
        }
    }

    return allSuccessful;
}

void Renderer::Update(float deltaTime){
    for (auto& item : m_renderQueue) {
        // skip animation 
        if (!item.visible) continue;
        if (item.framesPerState <= 1) continue; // nothing to animate // no animation sprite
        if (!item.playing) continue;

        item.elapsedSinceFrame += deltaTime;
        // create animation base on time
        while (item.elapsedSinceFrame >= item.frameDuration && item.frameDuration > 0.0f) {
            item.elapsedSinceFrame -= item.frameDuration;
            item.currentFrame++;
            if (item.currentFrame >= item.framesPerState) {
                if (item.looping) {
                    item.currentFrame = 0;
                }
                else {
                    item.currentFrame = item.framesPerState - 1;
                    item.playing = false; // stop at last frame
                    break;
                }
            }
            
        }

    }
}

void Renderer::AddRenderItem(const SpriteInstance& item) {
    // if texture not loaded, attempt load (keeps behavior)
    if (m_preloadedTextures.find(item.texturePath) == m_preloadedTextures.end()) {
        LoadTexture(item.texturePath);
    }

    // if an item with same id exists, replace (but preserve runtime fields)
    for (auto& existing : m_renderQueue) {
        if (existing.id == item.id) {
            // preserve runtime animation values from existing
            int savedFrame = existing.currentFrame;
            float savedElapsed = existing.elapsedSinceFrame;

            existing = item; // copy all fields
            existing.currentFrame = savedFrame;
            existing.elapsedSinceFrame = savedElapsed;
            SortRenderQueue();
            return;
        }
    }

    // otherwise push new
    m_renderQueue.push_back(item);
    SortRenderQueue();
}

void Renderer::RemoveRenderItem(const std::string& texturePath, const D3DXVECTOR3& position) {
    m_renderQueue.erase(
        std::remove_if(m_renderQueue.begin(), m_renderQueue.end(),
            [&](const SpriteInstance& item) {
                return item.texturePath == texturePath &&
                    item.position.x == position.x &&
                    item.position.y == position.y &&
                    item.position.z == position.z;
            }),
        m_renderQueue.end()
    );

}

void Renderer::ClearRenderQueue() {
    m_renderQueue.clear();
}

void Renderer::UpdateRenderItem(const SpriteInstance& item) {
    for (auto& existing : m_renderQueue) {
        if (existing.id == item.id) {
            // preserve runtime fields
            int savedCurrentFrame = existing.currentFrame;
            float savedElapsed = existing.elapsedSinceFrame;

            // copy static/display fields
            existing.texturePath = item.texturePath;
            existing.position = item.position;
            existing.scale = item.scale;
            existing.rotation = item.rotation;
            existing.color = item.color;
            existing.renderOrder = item.renderOrder;

            existing.animationRows = item.animationRows;
            existing.animationCols = item.animationCols;
            existing.framesPerState = item.framesPerState;
            existing.frameDuration = item.frameDuration;
            existing.looping = item.looping;

            // state is part of static animation properties
            existing.state = item.state;

            // playing flag allowed to change
            existing.playing = item.playing;

            // restore runtime animation progress so animation continues
            existing.currentFrame = savedCurrentFrame;
            existing.elapsedSinceFrame = savedElapsed;

            SortRenderQueue();
            return;
        }
    }

    // not found => add as new item
    AddRenderItem(item);
}

void Renderer::UpdateRenderItemById(int id, const SpriteInstance& newItem) {
    for (auto& item : m_renderQueue) {
        if (item.id == id) {
            item = newItem;
            SortRenderQueue();
            return;
        }
    }
    // optional: if not found, push it
    m_renderQueue.push_back(newItem);
    SortRenderQueue();
}

void Renderer::SortRenderQueue() {
    std::sort(m_renderQueue.begin(), m_renderQueue.end(),
        [](const SpriteInstance& a, const SpriteInstance& b) {
            if (a.renderOrder != b.renderOrder) {
                return a.renderOrder < b.renderOrder; // Lower order renders first
            }
            // If same render order, sort by Z position (back to front for transparency)
            return a.position.z < b.position.z;
        });
}

void Renderer::Render() {
    if (!m_d3dDevice) return;
    //	Clear the back buffer.
    m_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    // Begin scene
    if (SUCCEEDED(m_d3dDevice->BeginScene())) {
        m_spriteBrush->Begin(D3DXSPRITE_ALPHABLEND);

        // Render items from the render queue in sorted order
        for (auto& item : m_renderQueue) {
            if (!item.visible) continue;

            auto textureIt = m_preloadedTextures.find(item.texturePath);
            if (textureIt == m_preloadedTextures.end()) continue; // return control to game loop if there is nothing in render queue
            const TextureData& textureData = textureIt->second;

            // Build the source RECT fot the current frame
            // debug
            //std::cout << "tex W=" << textureData.info.Width << std::endl
            //    << "H=" << textureData.info.Height << std::endl
            //    << "rows= " << item.animationRows << "cols= " << item.animationCols << std::endl
            //    << "frameW= " << textureData.info.Width / item.animationCols << "frameH= " << textureData.info.Height / item.animationRows << std::endl << std::endl;

            RECT srcRect = { 0,0,0,0 };
            // compute logical frame size
            int logicalW = (textureData.logicalWidth > 0) ? textureData.logicalWidth : textureData.info.Width;
            int logicalH = (textureData.logicalHeight> 0) ? textureData.logicalHeight: textureData.info.Height;
            int frameW = (item.animationCols > 0) ? (logicalW / item.animationCols) : logicalW;
            int frameH = (item.animationRows > 0) ? (logicalH / item.animationRows) : logicalH;
            // use logical width, height if provided
            if (item.framesPerState > 1 && logicalW > 0 && logicalH > 0) {
                srcRect = item.GetSourceRect(logicalW, logicalH);
            }
            else {
                // entire texture 
                srcRect.left = 0;
                srcRect.top = 0;
                srcRect.right = textureData.info.Width;
                srcRect.bottom = textureData.info.Height;
            }

            // Find the center of the sprite
            D3DXVECTOR3 center((float)frameW * 0.5f, (float)frameH * 0.5f, 0.0f);

            // Build transform:
			// 1) translate center -> origin (-center)
			// 2) scale
			// 3) rotate around Z (item.rotation is in radians)
			// 4) translate origin -> final position (item.position + center) so top-left remains item.position
			// Create transformation matrix
			D3DXMATRIX transform, scaleMatrix, rotationMatrix, translationMatrix, mTransToOrigin;
			//D3DXMatrixIdentity(&transform);

			// Apply transformation matrix
            D3DXMatrixTranslation(&mTransToOrigin, -center.x, -center.y, 0.0f);
			D3DXMatrixScaling(&scaleMatrix, item.scale.x, item.scale.y, item.scale.z);
			D3DXMatrixRotationZ(&rotationMatrix, item.rotation);
			D3DXMatrixTranslation(&translationMatrix, item.position.x, item.position.y, item.position.z);

			transform = mTransToOrigin * scaleMatrix * rotationMatrix * translationMatrix;
			m_spriteBrush->SetTransform(&transform);

			m_spriteBrush->Draw(
				textureData.texture, 
				&srcRect,  // Source rectangle (null = entire texture)
				nullptr,  // Center point (null = top-left)
				nullptr,  // Position (handled by transform matrix)
				item.color
			);
        }

        // Reset transform
        D3DXMATRIX identity;
        D3DXMatrixIdentity(&identity);
        m_spriteBrush->SetTransform(&identity);
  
        m_spriteBrush->End();
        m_d3dDevice->EndScene();
    }
    else {
        MessageBox(nullptr, L"Fail to begin scene", L"Error", MB_ICONERROR);
        return;
    }

    m_d3dDevice->Present(NULL, NULL, NULL, NULL);
}