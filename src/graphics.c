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

/* Pietrek - Windows Internals (p.369) */
#define PEN_MAGIC     0x4F47 /* GO */
#define BRUSH_MAGIC   0x4F48 /* HO */
#define FONT_MAGIC    0x4F49 /* IO */
#define REGION_MAGIC  0x4F4C /* LO */

#define XUI_DEFAULT_FONT "7x14"

extern Display *disp;
extern int blackpixel;
extern int whitepixel;

struct GDIOBJ {
  short obj_sig; /* see above */
  COLORREF crColor;
  int penStyle; /* used only for pens */
  int nWidth; /* width of pen */
  XFontStruct *font; /* font */
  Region region;

  BOOL selected;
};

static bool stock_inited = false;
static struct GDIOBJ *system_font = NULL;
static struct GDIOBJ *dc_brush = NULL;
static struct GDIOBJ *black_pen = NULL;

static void init_stock_objects(void)
{
  system_font = calloc(1, sizeof(struct GDIOBJ));
  system_font->obj_sig = FONT_MAGIC ;
  system_font->font = XLoadQueryFont(disp, XUI_DEFAULT_FONT);

  /* The default DC_BRUSH color is WHITE */
  dc_brush = calloc(1, sizeof(struct GDIOBJ));
  dc_brush->obj_sig = BRUSH_MAGIC;
  dc_brush->crColor = RGB(0xff, 0xff, 0xff);

  black_pen = calloc(1, sizeof(struct GDIOBJ));
  black_pen->obj_sig= PEN_MAGIC;
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

HBRUSH CreateBrushIndirect(const LOGBRUSH *lplb)
{
  struct GDIOBJ *obj;

  if (lplb == NULL)
    return NULL;

  obj = calloc(1, sizeof(struct GDIOBJ));
  obj->obj_sig = BRUSH_MAGIC;
  obj->crColor = lplb->lbColor;
  return obj;
}

HBRUSH CreateSolidBrush(COLORREF crColor)
{
  LOGBRUSH lb;

  memset(&lb, 0, sizeof(lb));
  lb.lbStyle = BS_SOLID;
  lb.lbColor = crColor;
  return CreateBrushIndirect(&lb);
}

HPEN CreatePen(int fnPenStyle, int nWidth, COLORREF crColor)
{
  struct GDIOBJ *obj = calloc(1, sizeof(struct GDIOBJ));

  obj->obj_sig = PEN_MAGIC;
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

HGDIOBJ SelectObject(HDC hdc, HGDIOBJ hgdiobj)
{
  HGDIOBJ old = NULL;
  struct GDIOBJ *obj = hgdiobj;

  /* Determine the type of object this is */
  switch (obj->obj_sig) {
  case PEN_MAGIC:
    old = hdc->selectedPen;
    hdc->selectedPen = hgdiobj;
    break;
  case BRUSH_MAGIC:
    old = hdc->selectedBrush;
    hdc->selectedBrush = hgdiobj;
    break;
  case FONT_MAGIC:
    old = hdc->selectedFont;
    hdc->selectedFont = hgdiobj;
    break;
  default:
    printf("Unknown GDI object type\n");
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
      nRightRect - nLeftRect, nBottomRect - nTopRect);

  return TRUE;
}

static void
region_get_clip_box(HRGN hrgn, XRectangle* rect)
{
	XClipBox(hrgn->region, rect);
}

static int
region_get_complexity(HRGN hrgn)
{
	int ret;
	if (XEmptyRegion(hrgn->region)) {
		ret = NULLREGION;
	} else {
		XRectangle r;
		XClipBox(hrgn->region, &r);
		if (XRectInRegion(hrgn->region, r.x, r.y, r.width, r.height)) {
			ret = SIMPLEREGION;
		} else {
			ret = COMPLEXREGION;
		}
	}
	return ret;
}

int
GetRgnBox(HRGN hrgn, RECT *r)
{
	if (hrgn->obj_sig != REGION_MAGIC)
		return RGN_ERROR;

	if (r != NULL) {
		XRectangle rect;

		region_get_clip_box(hrgn, &rect);
		r->left   = rect.x;
		r->right  = rect.x + rect.width;
		r->top    = rect.y;
		r->bottom = rect.y + rect.height;
	}
	return region_get_complexity(hrgn);
}


