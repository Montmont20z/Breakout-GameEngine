#include "headers/Game.h"
#include <iostream> 

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;

int main(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
 
    Game game(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT, nCmdShow);

    g_game = &game;

    if (!game.Initialize()) {
        cout << "Game failed to Initialize" << endl;
        return -1;
    }
    game.Run();
    game.CleanUp();
    return 0;
}