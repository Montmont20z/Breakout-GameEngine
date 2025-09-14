#pragma once
#define DIRECTINPUT_VERSION 0x0800  // Move this to the TOP
#include <Windows.h>
#include <dinput.h>
#include <d3d9.h>
#include <d3dx9.h>

class InputManager {
public:
	InputManager();
	~InputManager();

	bool Initialize(HWND hWnd);
	void Update();

	// Query helpers
	bool IsKeyDown(unsigned char diKey) const;
	bool IsKeyPressed(unsigned char diKey) const; // for edge cases

	void SetLogicalSize(int w, int h) { m_logicalW = w; m_logicalH = h; }
	int  GetMouseX() const { return m_cursorX; }
	int  GetMouseY() const { return m_cursorY; }
	bool IsMouseDown(int button) const;
	bool IsMousePressed(int button) const;
	D3DXVECTOR2 GetMousePos() const { return D3DXVECTOR2((float)m_mouseX, (float)m_mouseY); }


private:
	// Input Variable
	//	Direct Input object.
	LPDIRECTINPUT8 m_dInput = nullptr; 	
	//	Direct Input keyboard device.
	LPDIRECTINPUTDEVICE8  m_dInputKeyboardDevice; 
	//	Direct Input mouse device.
	LPDIRECTINPUTDEVICE8  m_dInputMouseDevice;

	BYTE m_keysCurrent[256];
	BYTE m_keysPrev[256];

	DIMOUSESTATE m_mouseCurrent{};
	DIMOUSESTATE m_mousePrev{};
	int m_mouseX = 0;
	int m_mouseY = 0;
	int   m_logicalW = 1000, m_logicalH = 600;  // match your game backbuffer
	int   m_cursorX  = 500,  m_cursorY  = 300;  // start centered
	float m_sensitivity = 1.0f;
};