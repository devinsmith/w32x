/*
 * Copyright (c) 2017 Devin Smith <devin@devinsmith.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __WINDOWS_H__
#error Please do not include this file directly. Use #include <windows.h> instead.
#endif

#ifndef __WINUSER_H__
#define __WINUSER_H__

/* GetWindowLong indexes */
#define GWL_EXSTYLE -20
#define GWL_STYLE -16

/* GetSystemMetrics indexes */
#define SM_CYMENU 15

/* Standard window styles (WS) */
#define WS_OVERLAPPED   0x00000000
#define WS_TABSTOP      0x00010000
#define WS_MAXIMIZEBOX  0x00010000
#define WS_MINIMIZEBOX  0x00020000
#define WS_GROUP        0x00020000
#define WS_THICKFRAME   0x00040000
#define WS_SYSMENU      0x00080000
#define WS_HSCROLL      0x00100000
#define WS_VSCROLL      0x00200000
#define WS_DLGFRAME     0x00400000
#define WS_BORDER       0x00800000
#define WS_CAPTION      0x00C00000
#define WS_MAXIMIZE     0x01000000
#define WS_CLIPCHILDREN 0x02000000
#define WS_CLIPSIBLINGS 0x04000000
#define WS_DISABLED     0x08000000
#define WS_VISIBLE      0x10000000
#define WS_MINIMIZE     0x20000000
#define WS_CHILD        0x40000000
#define WS_POPUP        0x80000000

/* Aliases for window styles */
#define WS_CHILDWINDOW WS_CHILD
#define WS_ICONIC WS_MINIMIZE
#define WS_SIZEBOX WS_THICKFRAME
#define WS_TILED WS_OVERLAPPED
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_TILEDWINDOW WS_OVERLAPPEDWINDOW
#define WS_POPUPWINDOW (WS_POPUP | WS_BORDER | WS_SYSMENU)

/* Extended (EX) windows styles (WS) */
#define WS_EX_CLIENTEDGE 0x00000200

/* Create Window (CW) flags */
#define CW_USEDEFAULT ((int)0x80000000)

typedef struct tagPAINTSTRUCT {
  HDC  hdc;
  BOOL fErase;
  RECT rcPaint;
  BOOL fRestore;
  BOOL fIncUpdate;
  BYTE rgbReserved[32];
} PAINTSTRUCT, *PPAINTSTRUCT;

HDC BeginPaint(HWND wnd, PAINTSTRUCT *lpPaint);
HWND CreateWindow(const char *lpClassName, const char *lpWindowName,
  DWORD dwStyle, int x, int y, int width, int height, HWND parent);

HWND CreateWindowEx(DWORD dwExStyle, const char *lpClassName,
  const char *lpWindowName, DWORD dwStyle, int x, int y, int nWidth,
  int nHeight, HWND hWndParent);

BOOL GetMenu(HWND hwnd);
HDC GetDC(HWND hwnd);
int GetSystemMetrics(int nIndex);
LONG GetWindowLong(HWND hWnd, int nIndex);
BOOL GetWindowRect(HWND wnd, LPRECT rect);

int ReleaseDC(HWND hWnd, HDC hDC);
BOOL SetMenu(HWND hwnd);
BOOL UpdateWindow(HWND hwnd);

#endif /* __WINUSER_H__ */
