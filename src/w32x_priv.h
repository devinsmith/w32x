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

#ifndef __W32X_PRIV_H__
#define __W32X_PRIV_H__

struct Wnd {
  Window window;
  DWORD dwStyle;
  DWORD dwExStyle;
  Wnd *parent;
  char *label;
  int isTopLevel;
  HDC hdc;
  int (*proc)(Wnd *wnd, unsigned int msg, WPARAM wParam, LPARAM lParam);

  char wndExtra[];
};

struct WndClass {
  struct WndClass *next;
  char *name;
  unsigned long border_pixel;
  unsigned long background_pixel;
  int (*proc)(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam);
  size_t wndExtra;
};

struct WndDC {
  HWND wnd;

  int fgPixel;
  int bgPixel;
  GC gc;
  struct GDIOBJ *selectedPen;
  struct GDIOBJ *selectedBrush;
  struct GDIOBJ *selectedFont;
};

HDC w32x_CreateDC(void);
WndClass *get_class_by_name(const char *name);

#endif /* __W32X_PRIV_H__ */
