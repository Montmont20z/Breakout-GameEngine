#include "headers/MyWindow.h"

// Constructor
MyWindow::MyWindow(HINSTANCE hInstance, int width, int height, const std::wstring& title)
: m_hInstance(hInstance)
, m_hWnd(nullptr)
, m_width(width)
, m_height(height)
, m_title(title)
, m_windowClass(0)
{}

MyWindow::~MyWindow() {
	if (m_windowClass) {
		UnregisterClassW(m_title.c_str(), m_hInstance);
	}
}

bool MyWindow::Initialize() {
	// Register window class
	WNDCLASSEX wc = {};
	// Required Field
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = MyWindow::StaticWindowProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = m_title.c_str();
	// Optional Field
	wc.style = CS_HREDRAW | CS_VREDRAW; // redraw horizontally | vertically when resize window
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // make own icon with LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON))
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON))
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // make own icon with IDC_HAND
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // handle window's client area erased automatically

	m_windowClass = RegisterClassExW(&wc);
	if (!m_windowClass) {
		return false;
	}

	RECT rect = { 0,0,m_width,m_height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	m_hWnd = CreateWindowExW(
		WS_EX_CLIENTEDGE, // window edge style
		wc.lpszClassName,
		m_title.c_str(), //window name
		WS_OVERLAPPEDWINDOW, // window with title bar, sizing border, minimize button, system menu | Alternative: WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU (no resize)
		CW_USEDEFAULT, CW_USEDEFAULT, // default window show up position
		rect.right - rect.left, rect.bottom - rect.top, // Window Size
		nullptr, nullptr, m_hInstance,
		this // pass pointer for WM_NCCREATE
	);

	return (m_hWnd != nullptr);
}

void MyWindow::Show(int nCmdShow) const {
	ShowWindow(m_hWnd, nCmdShow);
}

bool MyWindow::ProcessMessages() const {
	MSG msg = {};
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

int MyWindow::GetWidth() const {
	return m_width;
}
int MyWindow::GetHeight() const {
	return m_height;
}

HWND MyWindow::GetHWND() const {
	return m_hWnd;
}

LRESULT CALLBACK MyWindow::StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_NCCREATE) {
		// Extract and store instance pointer
		const CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		MyWindow* self = static_cast<MyWindow*>(cs->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		self->m_hWnd = hWnd;
	}

	MyWindow* self = reinterpret_cast<MyWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	if (self) {
		return self->WindowProc(uMsg, wParam, lParam);
	}
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MyWindow::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	//case WM_KEYDOWN:
	//	if (wParam == VK_ESCAPE) {
	//		PostQuitMessage(0);
	//		return 0;
	//	}
	//	break;
	}
	return DefWindowProcW(m_hWnd, uMsg, wParam, lParam);
}
