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

#ifndef __WINDOWS_H__
#error Please do not include this file directly. Use #include <windows.h> instead.
#endif

#ifndef __WINGDI_H__
#define __WINGDI_H__

/* Stock objects */
#define WHITE_BRUSH 0
#define LTGRAY_BRUSH 1
#define GRAY_BRUSH 2
#define DKGRAY_BRUSH 3
#define BLACK_BRUSH 4
#define NULL_BRUSH 5
#define HOLLOW_BRUSH NULL_BRUSH
#define WHITE_PEN 6
#define BLACK_PEN 7
#define NULL_PEN 8
#define OEM_FIXED_FONT 10
#define ANSI_FIXED_FONT 11
#define ANSI_VAR_FONT 12
#define SYSTEM_FONT 13
#define DEVICE_DEFAULT_FONT 14
#define DEFAULT_PALETTE 15
#define SYSTEM_FIXED_FONT 16
#define DEFAULT_GUI_FONT 17
#define DC_BRUSH 18
#define DC_PEN 19
#define STOCK_LAST 19

#define ERROR 0
#define NULLREGION 1
#define SIMPLEREGION 2
#define COMPLEXREGION 3
#define RGN_ERROR ERROR

/* Brush styles */
#define BS_SOLID 0

/* GDI objects */
typedef struct GDIOBJ *HGDIOBJ;
typedef struct GDIOBJ *HFONT;
typedef struct GDIOBJ *HBRUSH;
typedef struct GDIOBJ *HPEN;
typedef struct GDIOBJ *HRGN;

typedef struct tagLOGBRUSH {
  UINT      lbStyle;
  COLORREF  lbColor;
  ULONG_PTR lbHatch;
} LOGBRUSH, *PLOGBRUSH;

HBRUSH CreateSolidBrush(COLORREF crColor);
HBRUSH CreateBrushIndirect(const LOGBRUSH *lplb);

int GetRgnBox(HRGN hrgn, RECT *lprc);
BOOL SetRectRgn(HRGN hrgn, int left, int top, int right, int bottom);

#endif /* __WINGDI_H__ */
