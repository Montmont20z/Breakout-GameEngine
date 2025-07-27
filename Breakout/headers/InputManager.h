#pragma once
#define DIRECTINPUT_VERSION 0x0800  // Move this to the TOP
#include <Windows.h>
#include <dinput.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class InputManager {
public:
	InputManager();
	~InputManager();

	bool Initialize(HWND hWnd);
	void Update();


private:
	// Input Variable
	//	Direct Input object.
	LPDIRECTINPUT8 m_dInput = nullptr; 	
	//	Direct Input keyboard device.
	LPDIRECTINPUTDEVICE8  m_dInputKeyboardDevice; 
	//	Direct Input mouse device.
	LPDIRECTINPUTDEVICE8  m_dInputMouseDevice; 



};