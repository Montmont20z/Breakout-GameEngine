#include "headers/Game.h"
#include "MenuState.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include <iostream> 

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;

int main(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
 
    Game game(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT, nCmdShow);

    g_game = &game;
    g_game->RegisterLevel(0, [] { return std::make_unique<MenuState>(); });
    g_game->RegisterLevel(1, [] { return std::make_unique<Level1>(); });
	g_game->RegisterLevel(2, [] { return std::make_unique<Level2>(); });
	g_game->RegisterLevel(3, [] { return std::make_unique<Level3>(); });

    if (!game.Initialize()) {
        cout << "Game failed to Initialize" << endl;
        return -1;
    }
    game.Run();
    game.CleanUp();
    return 0;
}