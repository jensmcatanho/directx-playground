/*
-----------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2018 Jean Michel Catanho

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "utils/Window.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace utils {


Window::Window() {

}

bool Window::Create(HINSTANCE hInstance, std::string title, int screenWidth, int screenHeight, int nShowCmd) {
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = "WindowClass1";
	RegisterClassEx(&windowClass);

	RECT clientRect = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);

	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	m_WindowHandler = CreateWindowEx(NULL, "WindowClass1", "RGB Cube", WS_OVERLAPPEDWINDOW, xPos, yPos, width, height, NULL, NULL, hInstance, NULL);
	if (m_WindowHandler == NULL) {
		// DWORD errCode = GetLastError();
		return false;
	}
	
	ShowWindow(m_WindowHandler, nShowCmd);
	return true;
}

}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}