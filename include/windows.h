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
#define __WINDOWS_H__

#include <stdint.h>

#define C_RED    "rgb:ff/00/00"
#define C_GREEN  "rgb:00/ff/00"
#define C_GREEN2 "rgb:00/ee/00"
#define C_BLUE   "rgb:00/00/ff"
#define C_GRAY1  "rgb:40/40/40"
#define C_GRAY2  "rgb:80/80/80"
#define C_GRAY3  "rgb:c0/c0/c0"
#define C_GRAY4  "rgb:d0/d0/d0"
#define C_GRAY5  "rgb:e0/e0/e0"
#define C_GRAY6  "rgb:a0/a0/a0"

#define C_BLACK  "rgb:00/00/00"

/* TRUE/FALSE */
#define FALSE   0
#define TRUE    1

/*
 * Basic typedefs are mostly from:
 * https://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx
 * However:
 * The below typedefs and definitions are not necessarily in alliance with
 * the Windows API, but they make sense for Unix/Linux environments.
 *
 * On Windows, both 32 bit and 64 bit environments the typedef is:
 * typedef long LONG
 *
 * But a long on Unix and Linux is different sizes depending on if a 32 bit
 * or 64 bit CPU is present. Therefore we use int here to ensure that it is
 * 32 bit and the API behaves similarily to Windows.
 */

typedef unsigned char BYTE;
typedef int BOOL;
typedef int LONG;
typedef unsigned int DWORD;
typedef unsigned int UINT;

typedef long LONG_PTR; // 32 bit on 32 bit, 64 bit on 64 bit.
typedef unsigned long ULONG_PTR; // 32 bit on 32 bit, 64 bit on 64 bit.
typedef uintptr_t UINT_PTR; // 32 bit on 32 bit, 64 bit on 64 bit.

typedef char *LPTSTR;

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;

typedef DWORD COLORREF;
#define RGB(r,g,b) ((COLORREF)((r) | ((g) << 8) | ((b) << 16)))

/* Events */
#define WM_CREATE                       0x0001
#define WM_DESTROY                      0x0002
#define WM_SIZE                         0x0005
#define WM_PAINT                        0x000F
#define WM_CLOSE                        0x0010
#define WM_QUIT                         0x0012
#define WM_ERASEBKGND                   0x0014
#define WM_NCPAINT                      0x0085
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202

/* Type declarations */
typedef void *HINSTANCE;

typedef struct WndClass WndClass;
typedef struct WndDC *HDC;
typedef struct Wnd *HWND;
typedef struct WndMenu *HMENU;
typedef int (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct tagWNDCLASS {
  const char *Name;
  const char *BackgroundColor;
  WNDPROC EventProc;
  size_t wndExtra;
} WNDCLASS;

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

typedef struct tagPOINT {
  LONG x;
  LONG y;
} POINT, *PPOINT;

typedef struct tagRECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT;
typedef RECT *LPRECT;

typedef struct tagMSG {
  HWND hwnd;
  UINT message;
  WPARAM wParam;
  LPARAM lParam;
} MSG;
typedef MSG *LPMSG;

#include <winuser.h>
#include <wingdi.h>

int w32x_init(const char *display_name);


int RegisterClass(WNDCLASS *wndclass);

void ShowWindow(HWND wnd);
void SetWindowName(HWND wnd, const char *name);
int GetWindowText(HWND wnd, char *lpString, int nMaxCount);
void DestroyWindow(HWND wnd);

HGDIOBJ GetStockObject(int fnObject);
HPEN CreatePen(int fnPenStyle, int nWidth, COLORREF crColor);

BOOL DeleteObject(HGDIOBJ hObject);
HGDIOBJ SelectObject(HDC hdc, HGDIOBJ hgdiobj);
COLORREF SetDCBrushColor(HDC hdc, COLORREF crColor);
BOOL TextOut(HDC hdc, int nXStart, int nYStart, const char *lpString,
    size_t cchString);
BOOL Ellipse(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
    int nBottomRect);
BOOL Rectangle(HDC hdc, int nLeftRect, int nTopRect,
  int nRightRect, int nBottomRect);

void *GetWindowLongPtr(HWND wnd, int nIndex);
int SendMessage(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam);
int DefWindowProc(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam);

BOOL GetMessage(LPMSG msg, HWND wnd);
int DispatchMessage(const MSG *msg);
void PostQuitMessage(int nExitCode);

/* Rect utility functions (rect.c) */
BOOL SetRect(RECT *r, int left, int top, int right, int bottom);
BOOL SetRectEmpty(RECT *r);
BOOL IsRectEmpty(const RECT *r);
BOOL IntersectRect(RECT *r1, const RECT *r2, const RECT *r3);
BOOL InflateRect(RECT *r, int dx, int dy);
BOOL OffsetRect(RECT *r, int xoff, int yoff);
BOOL EqualRect(const RECT *r1, const RECT *r2);
BOOL PtInRect(const RECT *r, POINT p);
BOOL CopyRect(RECT *dst, const RECT *src);
BOOL UnionRect(RECT *dst, const RECT *r1, const RECT *r2);
BOOL SubtractRect(RECT *dst, const RECT *r1, const RECT *r2);

/* Menus */
HMENU CreateMenu(void);
BOOL IsMenu(HMENU menu);


#endif /* __WINDOWS_H__ */
