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
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#include <windows.h>
#include "w32x_priv.h"

extern Display *disp;
extern XContext ctxt;
extern Atom WM_DELETE_WINDOW;

/* internals */
static void w32x_get_parent_client_offset(HWND parent, int *x, int *y)
{
  if (parent == NULL)
    return;

  if ((parent->dwExStyle & WS_EX_CLIENTEDGE)) {
    *x += 3;
    *y += 3;
  }

  if (GetMenu(parent)) {
    *y += GetSystemMetrics(SM_CYMENU);
  }
}

HDC BeginPaint(HWND wnd, PAINTSTRUCT *lpPaint)
{
  lpPaint->hdc = GetDC(wnd);
  lpPaint->fErase = FALSE;

  SendMessage(wnd, WM_NCPAINT, 0, 0);

  return lpPaint->hdc;
}

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

  wnd->width = width;
  wnd->height = height;

  /* Create our GC */
  wnd->hdc = w32x_CreateDC();

  /* Save the styles */
  wnd->dwStyle = dwStyle;
  wnd->dwExStyle = dwExStyle;

//  w32x_get_parent_client_offset(parent, &x, &y);

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

BOOL GetClientRect(HWND wnd, LPRECT rect)
{
  rect->left = 0;
  rect->top = 0;
  rect->right = wnd->width;
  rect->bottom = wnd->height;

  return TRUE;
}


HDC GetDC(HWND hwnd)
{
  /* For now, just return the private windows DC */
  hwnd->hdc->wnd = hwnd;
  return hwnd->hdc;
}

BOOL GetMenu(HWND hwnd)
{
  return hwnd->hasMenu;
}

int GetSystemMetrics(int nIndex)
{
  switch (nIndex) {
  case SM_CYMENU:
    return 19;
  }
  return 0;
}

/*
 * Get a region to be updated (in the client coordinates?)
 */
int
GetUpdateRgn(HWND hwnd, HRGN rgn, BOOL erase)
{
	if (erase && hwnd->update != NULL &&
	    NULLREGION != GetRgnBox(hwnd->update, NULL)) {
		HDC hdc = GetDC(hwnd);
		SendMessage(hwnd, WM_ERASEBKGND, (WPARAM)hdc, 0);
		ReleaseDC(hwnd, hdc);
	}
	if (hwnd->update == NULL) {
		SetRectRgn(rgn, 0, 0, 0, 0); /* XXX: Correct? */
		return NULLREGION;
	}
	return CombineRgn(rgn, hwnd->update, NULL, RGN_COPY);
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
  Window child;
  int x_return, y_return;

  XTranslateCoordinates(disp, wnd->window, DefaultRootWindow(disp),
      0, 0, &x_return, &y_return, &child);

  rect->left = x_return;
  rect->top = y_return;
  rect->right = x_return + wnd->width;
  rect->bottom = y_return + wnd->height;

  return TRUE;
}

BOOL
InvalidateRect(HWND hwnd, const RECT *r, BOOL erase)
{
	if (hwnd == NULL) {
		fprintf(stderr, "InvalidateRect(NULL, ...) - Not currently supported\n");
		return TRUE;
	}

	hwnd->erase = erase;
	/* A null rect value indicates that the entire client rect should be
	 * invalidated. */
	if (r == NULL) {
		RECT client;
		if (hwnd->update) {
			DeleteObject(hwnd->update);
		}
		GetClientRect(hwnd, &client);
		hwnd->update = CreateRectRgnIndirect(&client);
		return TRUE;
	}
	RECT client;
	RECT fixed = *r;
	RECT fixed2;
	HRGN fixedRgn;

	if (fixed.left > fixed.right) {
		fixed.left = r->right;
		fixed.right = r->left;
	}
	if (fixed.top > fixed.bottom) {
		fixed.top = r->bottom;
		fixed.bottom = r->top;
	}
	GetClientRect(hwnd, &client);
	if (!IntersectRect(&fixed2, &client, &fixed))
		return TRUE;
	fixedRgn = CreateRectRgnIndirect(&fixed2);
	if (fixedRgn == NULL)
		return FALSE;
	if (hwnd->update == NULL) {
		hwnd->update = fixedRgn;
	} else {
		CombineRgn(hwnd->update, hwnd->update,
				 fixedRgn, RGN_OR);
		DeleteObject(fixedRgn);
	}
	return TRUE;
}

int ReleaseDC(HWND hwnd, HDC hdc)
{
  /* This is currently a no-op until we have other types of DCs */
  return 0;
}

BOOL SetMenu(HWND hwnd)
{
  hwnd->hasMenu = 1;

  return TRUE;
}

BOOL UpdateWindow(HWND hwnd)
{
  SendMessage(hwnd, WM_PAINT, 0, 0);

  return TRUE;
}
