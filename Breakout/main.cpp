#include "headers/Game.h"
#include <iostream>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;


// Assets to be preload
//std::vector<std::string> textureList = {
//    //"assets/paddle.png",
//    "assets/bg3.png",
//    "assets/singlePaddle.png",
//    "assets/ball.png",
//};

int main(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
 
  //  if (!renderer.LoadTexturesBatch(textureList)) {
  //      MessageBoxW(nullptr, L"Failed to load textures!", L"Error", MB_ICONERROR);
  //      return -1;
  

  //  //// Background (render order 0 - renders first)
  //  SpriteInstance background("assets/bg3.png", D3DXVECTOR3(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0), 0);
  //  renderer.AddRenderItem(background);

  //  // Paddle (render order 1 - renders on top of background)
  //  SpriteInstance paddle("assets/singlePaddle.png", D3DXVECTOR3(400, 500, 0), 1);
  //  renderer.AddRenderItem(paddle);



  //  // Ball (render order 2 - renders on top of everything)
  //  SpriteInstance ball("assets/ball.png", D3DXVECTOR3(500, 300, 0), 2);
  //  ball.scale = D3DXVECTOR3(0.5f, 0.5f, 1.0f); // Make ball smaller
  //  renderer.AddRenderItem(ball);


  //  // Game variables for ball movement
  //  float ballX = 500, ballY = 300;
  //  //float ballSpeedX = 5.0f, ballSpeedY = 5.0f;
  //  //D3DXVECTOR3 currentBallPos(ballX, ballY, 0); // Track current position
  //  int ballId = ball.id;
  //  D3DXVECTOR3 ballSpeed = D3DXVECTOR3(-5, -5, 0);

  //  for (auto& renderItem : renderer.renderQueue) {
  //      cout << "RenderItemID: " << renderItem.id << endl;
  //  }

  //  // deltaTime variable
  //  using clock = std::chrono::high_resolution_clock;
  //  auto last = clock::now();

  //  //SoundManager* soundManager = new SoundManager();
  //  //soundManager->InitializeAudio();
  //  //soundManager->LoadSounds();
  //  //soundManager->PlaySound1();
  //  //soundManager->PlaySoundTrack();

  //  while (window.ProcessMessages()) {
  //      // get delta time
		//auto now = clock::now();
		//std::chrono::duration<float> elapsed = now - last;
		//last = now;
		//float deltaTime = elapsed.count(); // seconds since last frame	

  //      // Game update and render calls here
  //      inputManager.Update();

 

  //      // Update ball position
  //      ball.position += ballSpeed;
  //      cout << ball.position.x << "," << ball.position.y << endl;
  //      cout << "Speed" << ballSpeed.x << "," << ballSpeed.y << endl;

  //      //// Simple bounce logic
  //      if (ballX <= 0 || ballX >= SCREEN_WIDTH ) ballSpeed.x = -(ballSpeed.x);
  //      if (ballY <= 0 || ballY >= SCREEN_HEIGHT ) ballSpeed.y = ballSpeed.y * -1;

  //      //// Update ball position in render queue
  //      //SpriteInstance updatedBall("assets/ball.png", D3DXVECTOR3(ballX, ballY, 0), 2);
  //      //updatedBall.scale = D3DXVECTOR3(0.5f, 0.5f, 1.0f);
  //      //renderer.UpdateRenderItem("assets/ball.png", currentBallPos, updatedBall);

  //      renderer.UpdateRenderItem(ball);
  //      renderer.UpdateRenderItem(paddle);
  //      renderer.Update(deltaTime);
  //      renderer.Render();
  //      
  //  }

  //  return 0;
    Game game(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT, nCmdShow);
    if (!game.Initialize()) {
        cout << "Game failed to Initialize" << endl;
        return -1;
    }
    game.Run();
    game.CleanUp();
    return 0;
}