/*
 * Simple example of w32x.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

static LRESULT CALLBACK
MainWindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CLOSE:
		DestroyWindow(wnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(wnd, msg, wParam, lParam);
	}
	return 0;
}

int
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR pCmdLine,
    int nCmdShow)
{
	HWND top;
	WNDCLASS myClass;
	MSG msg;
	BOOL bRet;
	HMENU menu, fileMenu;

	memset(&myClass, 0, sizeof(WNDCLASS));

	myClass.lpszClassName = "TopWindow";
	myClass.hbrBackground = (HBRUSH)(COLOR_BTNSHADOW + 1);
	myClass.cbWndExtra = 0;
	myClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	myClass.lpfnWndProc = MainWindowProc;
	RegisterClass(&myClass);

	menu = CreateMenu();
	fileMenu = CreatePopupMenu();
	AppendMenu(fileMenu, MF_STRING, 0, "E&xit");
	AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)fileMenu, "&File");

	/* parent window */
	top = CreateWindowEx(0, "TopWindow", "Test1",
	    WS_OVERLAPPEDWINDOW, 200, 200, 500, 300, NULL, menu, hInstance, NULL);

	CreateWindow("RadioButton", "Radio 1",
	    WS_CHILD | WS_BORDER | WS_VISIBLE, 5, 30, 100, 25, top, NULL,
	    hInstance, NULL);
	CreateWindow("RadioButton", "Radio 2",
	    WS_CHILD | WS_BORDER | WS_VISIBLE, 5, 60, 100, 25, top, NULL,
	    hInstance, NULL);

	ShowWindow(top, nCmdShow);
	UpdateWindow(top);

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		DispatchMessage(&msg);
	}

	return 0;
}
