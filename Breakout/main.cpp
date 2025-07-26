#include "headers/MyWindow.h"
#include "headers/Renderer.h"
#include <iostream>

using namespace std;

// Assets to be preload
std::vector<std::string> textureList = {
    "assets/paddle.png",
    "assets/bg1.png",
};

int main(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    MyWindow window(hInstance, 800, 600, L"Breakout Game");
    Renderer renderer;
    if (!window.Initialize()) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return -1;
    }

    window.Show(nCmdShow);
    if (!renderer.Initialize(window)) {
        MessageBoxW(nullptr, L"Failed to initialize renderer!", L"Error", MB_ICONERROR);
        return -1;
    }
    if (!renderer.LoadTexturesBatch(textureList)) {
        MessageBoxW(nullptr, L"Failed to load bg1.png!", L"Error", MB_ICONERROR);
        return -1;
    }

    while (window.ProcessMessages()) {
        // Game update and render calls here

        renderer.Render();
        
    }

    return 0;
}