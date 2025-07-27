#include "headers/InputManager.h"
#include <iostream>

InputManager::InputManager() : m_dInput(nullptr), m_dInputKeyboardDevice(nullptr), m_dInputMouseDevice(nullptr) {
    // Initialize member variables
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
    //	Key input buffer
    BYTE  diKeys[256];
    // Input
    m_dInputKeyboardDevice->GetDeviceState(256, diKeys);

    if (diKeys[DIK_UP] & 0x80)
    {
        std::cout << "UP" << std::endl;
    }
    if (diKeys[DIK_DOWN] & 0x80)
    {
        std::cout << "DOWN" << std::endl;
    }
    if (diKeys[DIK_LEFT] & 0x80)
    {
        std::cout << "LEFT" << std::endl;
    }
    if (diKeys[DIK_RIGHT] & 0x80)
    {
        std::cout << "RIGHT" << std::endl;
    }

    
}