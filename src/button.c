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

#include <string.h>
#include <windows.h>

#define RBDIAM 11
#define RB_X 8

struct RadioButtonInfo {
  int activated;
};

static int RadioButtonProc(HWND wnd, unsigned int msg, WPARAM wParam,
    LPARAM lParam);

WNDCLASS ButtonClass = {
  "RadioButton", C_GRAY5, RadioButtonProc, sizeof(struct RadioButtonInfo)
};

static void
drawRadioButton(HWND wnd, int x, int y, unsigned int w,
    unsigned int h, int clicked)
{
  PAINTSTRUCT ps;
  struct RadioButtonInfo *extra;
  char label[256];

  GetWindowText(wnd, label, sizeof(label));
  extra = GetWindowLongPtr(wnd, 0);

  HDC hdc = BeginPaint(wnd, &ps);

  SelectObject(hdc, GetStockObject(DC_BRUSH));
  SelectObject(hdc, GetStockObject(BLACK_PEN));

  if (clicked) {
    extra->activated = extra->activated ? 0 : 1;
    SetDCBrushColor(hdc, RGB(0x80, 0x80, 0x80));
  } else {
    SetDCBrushColor(hdc, RGB(0xe0, 0xe0, 0xe0));
  }

  Rectangle(hdc, 0, 0, w - 1, h - 1);
  Rectangle(hdc, 1, 1, w - 2, h - 2);

  if (extra->activated) {
    SetDCBrushColor(hdc, RGB(0, 0xff, 0));
  } else {
    SetDCBrushColor(hdc, RGB(0xc0, 0xc0, 0xc0));
  }

  Ellipse(hdc, RB_X, (h - RBDIAM) / 2, RB_X + RBDIAM,
      ((h - RBDIAM) / 2) + RBDIAM);

  SelectObject(hdc, GetStockObject(SYSTEM_FONT));
  TextOut(hdc, RB_X * 2 + RBDIAM, h, label, strlen(label));

}

static int
RadioButtonProc(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
  RECT winRect;

  GetClientRect(wnd, &winRect);

  switch (msg) {
  case WM_LBUTTONDOWN:
			drawRadioButton(wnd, winRect.left, winRect.top,
			    winRect.right, winRect.bottom, 1);
		break;
  case WM_LBUTTONUP:
			drawRadioButton(wnd, winRect.left, winRect.top,
			    winRect.right, winRect.bottom, 0);
		break;
  case WM_PAINT:
			drawRadioButton(wnd, winRect.left, winRect.top,
			    winRect.right, winRect.bottom, 0);
		break;
  default:
    return DefWindowProc(wnd, msg, wParam, lParam);
    break;
  }
  return 1;
}
