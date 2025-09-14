#include "InputManager.h"
#include <iostream>

using namespace std;

InputManager::InputManager() : m_dInput(nullptr), m_dInputKeyboardDevice(nullptr), m_dInputMouseDevice(nullptr) {
    // Clean up previous buffer
    ZeroMemory(m_keysCurrent, sizeof(m_keysCurrent));
    ZeroMemory(m_keysPrev, sizeof(m_keysPrev));
}
InputManager::~InputManager() {
    //	Release keyboard device.
    if (m_dInputKeyboardDevice) {
        m_dInputKeyboardDevice->Unacquire();
        m_dInputKeyboardDevice->Release();
        m_dInputKeyboardDevice = NULL;
    }
    //	Release keyboard device.
    if (m_dInputMouseDevice) {
        m_dInputMouseDevice->Unacquire();
        m_dInputMouseDevice->Release();
        m_dInputMouseDevice = NULL;
    }
    //	Release DirectInput.
    if (m_dInput) {
        m_dInput->Release();
        m_dInput = NULL;
    }
}



bool InputManager::Initialize(HWND hWnd) {
    
    //	Create the Direct Input object.
    HRESULT hr = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_dInput, NULL);
    if (FAILED(hr)) {
        wchar_t msg[256];
        swprintf_s(msg, L"Error Creating Direct Input (HR: 0x%08X)", hr);
        MessageBox(nullptr, msg, L"Error", MB_ICONERROR | MB_OK);
        return false;
    }

    //	Create the keyboard device.
    hr = m_dInput->CreateDevice(GUID_SysKeyboard, &m_dInputKeyboardDevice, NULL);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"Error Creating Keyboard Device", L"Error", MB_ICONERROR | MB_OK);
        return false;
    }
    //	Create the mouse device.
    hr = m_dInput->CreateDevice(GUID_SysMouse, &m_dInputMouseDevice, NULL);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"Error Creating Mouse Device", L"Error", MB_ICONERROR | MB_OK);
        return false;
    }

    //	Set the input data format.
    m_dInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
    m_dInputMouseDevice->SetDataFormat(&c_dfDIMouse);
    //	Set the cooperative level.
    m_dInputKeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    m_dInputMouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    //	Acquire the device.
    m_dInputKeyboardDevice->Acquire();
    m_dInputMouseDevice->Acquire();

    return true;
}

void InputManager::Update() {
    // -------- keyboard: copy prev -> read current --------
    memcpy(m_keysPrev, m_keysCurrent, sizeof(m_keysCurrent));
    ZeroMemory(m_keysCurrent, sizeof(m_keysCurrent));

    if (m_dInputKeyboardDevice) {
        HRESULT hr = m_dInputKeyboardDevice->GetDeviceState(sizeof(m_keysCurrent), (LPVOID)m_keysCurrent);
        if (FAILED(hr)) {
            // try to recover/reacquire device
            if (SUCCEEDED(m_dInputKeyboardDevice->Acquire())) {
                m_dInputKeyboardDevice->GetDeviceState(sizeof(m_keysCurrent), (LPVOID)m_keysCurrent);
            }
        }
    }

    // -------- mouse: copy prev -> read current (ALWAYS) --------
    m_mousePrev = m_mouseCurrent;
    ZeroMemory(&m_mouseCurrent, sizeof(m_mouseCurrent));

    if (m_dInputMouseDevice) {
        HRESULT hr = m_dInputMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseCurrent);
        if (FAILED(hr)) {
            // reacquire, then try once more
			if (SUCCEEDED(m_dInputMouseDevice->Acquire())) {
				hr = m_dInputMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseCurrent);
			}
			if (FAILED(hr)) {
				// device still not giving data this frame -> no movement, keep last cursor
				ZeroMemory(&m_mouseCurrent, sizeof(m_mouseCurrent));
			}
        } 
        // accumulate relative deltas into logical-coord cursor
		// (don’t zero these anywhere else; they persist across frames)
		m_cursorX += m_mouseCurrent.lX;
		m_cursorY += m_mouseCurrent.lY;

		// clamp to logical backbuffer size
		if (m_cursorX < 0) m_cursorX = 0;
		if (m_cursorY < 0) m_cursorY = 0;
		if (m_cursorX >= m_logicalW) m_cursorX = m_logicalW - 1;
		if (m_cursorY >= m_logicalH) m_cursorY = m_logicalH - 1;
    }
}


bool InputManager::IsKeyDown(unsigned char diKey) const {
    return (m_keysCurrent[diKey] & 0x80) != 0;
}

bool InputManager::IsKeyPressed(unsigned char diKey) const {
    return ((m_keysCurrent[diKey] & 0x80) != 0) && ((m_keysPrev[diKey] & 0x80) == 0);
}

bool InputManager::IsMouseDown(int button) const {
    if (button < 0 || button > 2) return false;
    return (m_mouseCurrent.rgbButtons[button] & 0x80) != 0;
}
bool InputManager::IsMousePressed(int button) const {
    if (button < 0 || button > 2) return false;
    return ((m_mouseCurrent.rgbButtons[button] & 0x80) != 0) &&
        ((m_mousePrev.rgbButtons[button] & 0x80) == 0);
}