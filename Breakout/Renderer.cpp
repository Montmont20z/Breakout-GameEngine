#include "headers/Renderer.h"
#include "headers/MyWindow.h"

Renderer::Renderer() {

}


Renderer::~Renderer() {
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

bool Renderer::Initialize(MyWindow myWindow) {
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
    m_d3dPP.BackBufferWidth = myWindow.GetWidth();
    m_d3dPP.BackBufferHeight = myWindow.GetHeight();
    m_d3dPP.BackBufferFormat = D3DFMT_UNKNOWN; // Use desktop format
    m_d3dPP.BackBufferCount = 1;
    m_d3dPP.hDeviceWindow = myWindow.GetHWND();
    m_d3dPP.EnableAutoDepthStencil = TRUE;
    m_d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dPP.AutoDepthStencilFormat = D3DFMT_D16;

    // Create device
    HRESULT hr = m_direct3D9->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        myWindow.GetHWND(),
        D3DCREATE_HARDWARE_VERTEXPROCESSING, // Hardware = GPU
        &m_d3dPP,
        &m_d3dDevice
    );

    if (FAILED(hr)) {
        // Failback to software vertex processing
        hr = m_direct3D9->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            myWindow.GetHWND(),
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

bool Renderer::LoadTexture(const std::string& path) {
    // Check if already loaded
    if (m_preloadedTextures.count(path))
        return true;

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

void Renderer::Render() {
    //	Clear the back buffer.
    m_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    // Begin scene
    if (SUCCEEDED(m_d3dDevice->BeginScene())) {
        m_spriteBrush->Begin(D3DXSPRITE_ALPHABLEND);


        for (auto& kv : m_preloadedTextures) {
            const TextureData& textureData = kv.second;
            if (textureData.texture) {
                D3DXVECTOR3 pos(0,0,0);
                m_spriteBrush->Draw(textureData.texture, nullptr, nullptr, &pos, D3DCOLOR_XRGB(255, 255, 255));
            }
        }
  
        m_spriteBrush->End();
        // End scene
        m_d3dDevice->EndScene();
    }
    else {
        MessageBox(nullptr, L"Fail to begin scene", L"Error", MB_ICONERROR);
        return;
    }



    m_d3dDevice->Present(NULL, NULL, NULL, NULL);
}