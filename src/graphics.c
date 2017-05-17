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

#define PEN_TYPE 1
#define BRUSH_TYPE 2
#define FONT_TYPE 3

#define XUI_DEFAULT_FONT "7x14"

extern Display *disp;
extern int blackpixel;
extern int whitepixel;

struct GDIOBJ {
  int objType; /* see above */
  COLORREF crColor;
  int penStyle; /* used only for pens */
  int nWidth; /* width of pen */
  XFontStruct *font; /* font */

  BOOL selected;
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

static bool stock_inited = false;
static struct GDIOBJ *system_font = NULL;
static struct GDIOBJ *dc_brush = NULL;
static struct GDIOBJ *black_pen = NULL;

static void init_stock_objects(void)
{
  system_font = calloc(1, sizeof(struct GDIOBJ));
  system_font->objType = FONT_TYPE;
  system_font->font = XLoadQueryFont(disp, XUI_DEFAULT_FONT);

  /* The default DC_BRUSH color is WHITE */
  dc_brush = calloc(1, sizeof(struct GDIOBJ));
  dc_brush->objType = BRUSH_TYPE;
  dc_brush->crColor = RGB(0xff, 0xff, 0xff);

  black_pen = calloc(1, sizeof(struct GDIOBJ));
  black_pen->objType = PEN_TYPE;
  black_pen->crColor = RGB(0x00, 0x00, 0x00);

  stock_inited = true;
}

/* The GetStockObject function retrieves a handle to one of the stock
 * pens, brushes, fonts, or palettes. */
HGDIOBJ GetStockObject(int fnObject)
{
  if (!stock_inited) {
    init_stock_objects();
  };

  if (fnObject == SYSTEM_FONT)
    return system_font;
  else if (fnObject == DC_BRUSH)
    return dc_brush;
  else if (fnObject == BLACK_PEN)
    return black_pen;

  return NULL;
}

HBRUSH CreateSolidBrush(COLORREF crColor)
{
  struct GDIOBJ *obj = calloc(1, sizeof(struct GDIOBJ));

  obj->objType = BRUSH_TYPE;
  obj->crColor = crColor;
  return obj;
}

HPEN CreatePen(int fnPenStyle, int nWidth, COLORREF crColor)
{
  struct GDIOBJ *obj = calloc(1, sizeof(struct GDIOBJ));

  obj->objType = PEN_TYPE;
  obj->crColor = crColor;
  obj->penStyle = fnPenStyle;
  obj->nWidth = nWidth;
  return obj;
}

/* The DeleteObject function deletes a logical pen, brush, font, bitmap,
 * region, or palette, freeing all system resources associated with the object.
 * After the object is deleted, the specified handle is no longer valid.
 */
BOOL DeleteObject(HGDIOBJ hObject)
{
  struct GDIOBJ *obj = hObject;

  if (obj->selected)
    return FALSE;

  free(hObject);
  return TRUE;
}

BOOL TextOut(HDC hdc, int nXStart, int nYStart, const char *lpString,
    size_t cchString)
{
  XFontStruct *font;
  XTextItem ti[1];
  struct GDIOBJ *gdi_font = (struct GDIOBJ *)hdc->selectedFont;

  font = gdi_font->font;

  ti[0].chars = (char *)lpString;
  ti[0].nchars = cchString;
  ti[0].delta = 0;
  ti[0].font = font->fid;

  XDrawText(disp, hdc->wnd->window, hdc->gc, nXStart,
      (nYStart - (font->ascent + font->descent)) / 2 + font->ascent, ti, 1);

  //XUnloadFont(disp, font->fid);

  return TRUE;
}

/* Private functions */
HDC w32x_CreateDC(void)
{
  XGCValues gcv;
  HDC dc;

  gcv.background = whitepixel;
  gcv.foreground = blackpixel;
  dc = calloc(1, sizeof(struct WndDC));
  dc->gc = XCreateGC(disp, DefaultRootWindow(disp),
      GCForeground | GCBackground, &gcv);

  return dc;
}

HDC BeginPaint(HWND wnd)
{
  wnd->hdc->wnd = wnd;

  SendMessage(wnd, WM_NCPAINT, 0, 0);

  return wnd->hdc;
}

HGDIOBJ SelectObject(HDC hdc, HGDIOBJ hgdiobj)
{
  HGDIOBJ old = NULL;
  struct GDIOBJ *obj = hgdiobj;

  /* Determine the type of object this is */
  switch (obj->objType) {
  case PEN_TYPE:
    old = hdc->selectedPen;
    hdc->selectedPen = hgdiobj;
    break;
  case BRUSH_TYPE:
    old = hdc->selectedBrush;
    hdc->selectedBrush = hgdiobj;
    break;
  case FONT_TYPE:
    old = hdc->selectedFont;
    hdc->selectedFont = hgdiobj;
    break;
  }

  return old;
}

COLORREF SetDCBrushColor(HDC hdc, COLORREF crColor)
{
  COLORREF old_val;
  struct GDIOBJ *selected = hdc->selectedBrush;

  if (selected != dc_brush)
    return CLR_INVALID;

  old_val = selected->crColor;

  selected->crColor = crColor;

  return old_val;
}

static void setFgColor(HDC hdc, COLORREF cr)
{
  XGCValues gcv;

  if (hdc->fgPixel != cr) {
    gcv.foreground = cr;
    XChangeGC(disp, hdc->gc, GCForeground, &gcv);
    hdc->fgPixel = cr;
  }
}

BOOL Ellipse(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
    int nBottomRect)
{
  setFgColor(hdc, hdc->selectedBrush->crColor);

  XFillArc(disp, hdc->wnd->window, hdc->gc, nLeftRect,
      nTopRect, nRightRect - nLeftRect, nBottomRect - nTopRect, 0, 360 * 64);

  setFgColor(hdc, hdc->selectedPen->crColor);

  XDrawArc(disp, hdc->wnd->window, hdc->gc, nLeftRect, nTopRect,
      nRightRect - nLeftRect, nBottomRect - nTopRect, 0, 360 * 64);
  return TRUE;

}

BOOL Rectangle(HDC hdc, int nLeftRect, int nTopRect,
  int nRightRect, int nBottomRect)
{
  setFgColor(hdc, hdc->selectedBrush->crColor);

  XDrawRectangle(disp, hdc->wnd->window, hdc->gc, nLeftRect, nTopRect,
      nRightRect, nBottomRect);

  return TRUE;
}



