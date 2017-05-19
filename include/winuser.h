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

/* Extended windows styles */
#define WS_EX_CLIENTEDGE 0x00000200

HWND CreateWindow(const char *lpClassName, const char *lpWindowName,
  DWORD dwStyle, int x, int y, int width, int height, HWND parent);

HWND CreateWindowEx(DWORD dwExStyle, const char *lpClassName,
  const char *lpWindowName, DWORD dwStyle, int x, int y, int nWidth,
  int nHeight, HWND hWndParent);

HDC GetDC(HWND hwnd);
LONG GetWindowLong(HWND hWnd, int nIndex);
BOOL GetWindowRect(HWND wnd, LPRECT rect);

int ReleaseDC(HWND hWnd, HDC hDC);
BOOL UpdateWindow(HWND hwnd);

#endif /* __WINUSER_H__ */
