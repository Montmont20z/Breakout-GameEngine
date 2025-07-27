#include "headers/MyWindow.h"
#include "headers/Renderer.h"
#include "headers/InputManager.h"
#include <iostream>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;

// Assets to be preload
std::vector<std::string> textureList = {
    //"assets/paddle.png",
    "assets/bg3.png",
    "assets/singlePaddle.png",
    "assets/ball.png",
};

int main(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    MyWindow window(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT, L"Breakout Game");
    Renderer renderer;
    InputManager inputManager;
    if (!window.Initialize()) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return -1;
    }

    window.Show(nCmdShow);
    if (!renderer.Initialize(window.GetHWND(), window.GetWidth(), window.GetHeight())) {
        MessageBoxW(nullptr, L"Failed to initialize renderer!", L"Error", MB_ICONERROR);
        return -1;
    }
    if (!renderer.LoadTexturesBatch(textureList)) {
        MessageBoxW(nullptr, L"Failed to load textures!", L"Error", MB_ICONERROR);
        return -1;
    }

    if (!inputManager.Initialize(window.GetHWND())) {
        MessageBoxW(nullptr, L"Failed to initialize input manager!", L"Error", MB_ICONERROR);
        return -1;
    }

    // TODO: Extract into a class 
    // Set up render queue with different layers and positions
    // Background (render order 0 - renders first)
    RenderItem background("assets/bg3.png", D3DXVECTOR3(0, 0, 0), 0);
    renderer.AddRenderItem(background);

    // Paddle (render order 1 - renders on top of background)
    RenderItem paddle("assets/singlePaddle.png", D3DXVECTOR3(400, 500, 0), 1);
    renderer.AddRenderItem(paddle);

    // Ball (render order 2 - renders on top of everything)
    RenderItem ball("assets/ball.png", D3DXVECTOR3(500, 300, 0), 2);
    ball.scale = D3DXVECTOR3(0.5f, 0.5f, 1.0f); // Make ball smaller
    renderer.AddRenderItem(ball);

    // Game variables for ball movement
    float ballX = 500, ballY = 300;
    float ballSpeedX = 2.0f, ballSpeedY = 2.0f;
    D3DXVECTOR3 currentBallPos(ballX, ballY, 0); // Track current position

    while (window.ProcessMessages()) {
        // Game update and render calls here
        inputManager.Update();

        // Update ball position
        currentBallPos = D3DXVECTOR3(ballX, ballY, 0);
        ballX += ballSpeedX;
        ballY += ballSpeedY;

        // Simple bounce logic
        if (ballX <= 0 || ballX >= SCREEN_WIDTH - 32) ballSpeedX = -ballSpeedX;
        if (ballY <= 0 || ballY >= SCREEN_HEIGHT - 32) ballSpeedY = -ballSpeedY;

        // Update ball position in render queue
        RenderItem updatedBall("assets/ball.png", D3DXVECTOR3(ballX, ballY, 0), 2);
        //updatedBall.scale = D3DXVECTOR3(0.5f, 0.5f, 1.0f);
        renderer.UpdateRenderItem("assets/ball.png", currentBallPos, updatedBall);

        renderer.Render();
        
    }

    return 0;
}