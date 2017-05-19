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
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#include <windows.h>
#include "w32x_priv.h"

extern Display *disp;
extern XContext ctxt;
extern Atom WM_DELETE_WINDOW;

HWND CreateWindow(const char *lpClassName, const char *lpWindowName,
  DWORD dwStyle, int x, int y, int width, int height, HWND parent)
{
  return CreateWindowEx(0, lpClassName, lpWindowName, dwStyle, x, y,
      width, height, parent);
}

HWND CreateWindowEx(DWORD dwExStyle, const char *lpClassName,
  const char *lpWindowName, DWORD dwStyle, int x, int y, int width,
  int height, HWND parent)
{
  XClassHint class_hint;
  Window parent_win;
  Wnd *wnd;
  WndClass *wc = get_class_by_name(lpClassName);

  if (wc == NULL) {
    /* Can't create window, no class */
    return NULL;
  }

  wnd = calloc(1, sizeof(Wnd) + wc->wndExtra);

  if (parent != NULL) {
    parent_win = parent->window;
  } else {
    parent_win = DefaultRootWindow(disp);
  }

  /* Create our GC */
  wnd->hdc = w32x_CreateDC();

  /* Save the styles */
  wnd->dwStyle = dwStyle;
  wnd->dwExStyle = dwExStyle;

  /* parent window */
  wnd->window = XCreateSimpleWindow(disp, parent_win,
    x, y, width, height,
    dwStyle & WS_BORDER ? 1 : 0,
    wc->border_pixel, wc->background_pixel);
  wnd->label = strdup(lpWindowName);
  wnd->proc = wc->proc;
  wnd->parent = parent;
  class_hint.res_name = wnd->label;
  class_hint.res_class = wc->name;
  XSetClassHint(disp, wnd->window, &class_hint);

  XSaveContext(disp, wnd->window, ctxt, (XPointer)wnd);

  XSelectInput(disp, wnd->window,
    ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask |
    StructureNotifyMask);

  /* Parent will explicitly call ShowWindow when ready */
  if (parent != NULL)
    ShowWindow(wnd);

  /* For top level windows we want to do some extra special case
   * processing */
  if (parent == NULL) {
    SetWindowName(wnd, lpWindowName);

    /* Use the WM_DELETE_WINDOW atom to tell the window manager that we want
     * to handle when this window is closed/destroyed */
    XSetWMProtocols(disp, wnd->window, &WM_DELETE_WINDOW, 1);
  }

  return wnd;
}

HDC GetDC(HWND hwnd)
{
  /* For now, just return the private windows DC */
  hwnd->hdc->wnd = hwnd;
  return hwnd->hdc;
}

LONG GetWindowLong(HWND hWnd, int nIndex)
{
  /* TODO: Not all indexes are handled. */
  if (hWnd == NULL)
    return 0;

  if (nIndex == GWL_EXSTYLE)
    return hWnd->dwExStyle;
  else if (nIndex == GWL_STYLE)
    return hWnd->dwStyle;
  else
    return 0;
}

BOOL GetWindowRect(HWND wnd, LPRECT rect)
{
  Window root_return;
  int x_return, y_return;
  unsigned int width_return, height_return;
  unsigned int border_width_return;
  unsigned int depth_return;

  XGetGeometry(disp, wnd->window, &root_return, &x_return, &y_return,
    &width_return, &height_return, &border_width_return, &depth_return);

  rect->left = x_return;
  rect->top = y_return;
  rect->right = width_return;
  rect->bottom = height_return;

  return TRUE;
}

int ReleaseDC(HWND hwnd, HDC hdc)
{
  /* This is currently a no-op until we have other types of DCs */
  return 0;
}

BOOL UpdateWindow(HWND hwnd)
{
  SendMessage(hwnd, WM_PAINT, 0, 0);

  return TRUE;
}
