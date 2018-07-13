/*
 * Copyright (c) 2000 Masaru OKI
 * Copyright (c) 2001 TAMURA Kent
 * Copyright (c) 2017 Devin Smith <devin@devinsmith.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

static int
handle_ncpaint(HWND hwnd)
{
	RECT wr;
	LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	/* Not technically correct, because the X11 WM paints our NC area. */
	GetClientRect(hwnd, &wr);

	if ((exStyle & WS_EX_CLIENTEDGE)) {
		int top = 0;
		if (GetMenu(hwnd)) {
			top += GetSystemMetrics(SM_CYMENU);
		}
		HDC hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hdc, RGB(0xd0, 0xd0, 0xd0));
		Rectangle(hdc, 0, top, wr.right - 1, wr.bottom - 1);
		SetDCBrushColor(hdc, RGB(0xc0, 0xc0, 0xc0));
		Rectangle(hdc, 1, top + 1, wr.right - 2, wr.bottom - 2);
		SetDCBrushColor(hdc, RGB(0x00, 0x00, 0x00));
		Rectangle(hdc, 2, top + 2, wr.right - 3, wr.bottom - 3);

		ReleaseDC(hwnd, hdc);
	}
	return 0;
}

LRESULT
DefWindowProc(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (msg) {
	case WM_NCPAINT:
		handle_ncpaint(wnd);
		break;
	case WM_PAINT:
		BeginPaint(wnd, &ps);
		break;
	case WM_CLOSE:
		printf("Request to close window\n");
		DestroyWindow(wnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		printf("DefWindowProc: %d\n", msg);
		break;
	}
	return 0;
}
