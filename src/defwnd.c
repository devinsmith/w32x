/*
 * Copyright (c) 2015-2016 Devin Smith <devin@devinsmith.net>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

static int handle_ncpaint(HWND hwnd)
{
  RECT wr;
  LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

  GetWindowRect(hwnd, &wr);


  if ((exStyle & WS_EX_CLIENTEDGE)) {
    int top = 0;
    if (GetMenu(hwnd)) {
      top += 25;
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

int
DefWindowProc(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_NCPAINT:
    handle_ncpaint(wnd);
    break;
  case WM_PAINT:
    BeginPaint(wnd);
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
