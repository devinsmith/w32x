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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define COLOR_SCROLLBAR 0
#define COLOR_BACKGROUND 1
#define COLOR_ACTIVECAPTION 2
#define COLOR_INACTIVECAPTION 3
#define COLOR_MENU 4
#define COLOR_WINDOW 5
#define COLOR_WINDOWFRAME 6
#define COLOR_MENUTEXT 7
#define COLOR_WINDOWTEXT 8
#define COLOR_CAPTIONTEXT 9
#define COLOR_ACTIVEBORDER 10
#define COLOR_INACTIVEBORDER 11
#define COLOR_APPWORKSPACE 12
#define COLOR_HIGHLIGHT 13
#define COLOR_HIGHLIGHTTEXT 14
#define COLOR_BTNFACE 15
#define COLOR_BTNSHADOW 16
#define COLOR_GRAYTEXT 17
#define COLOR_BTNTEXT 18
#define COLOR_INACTIVECAPTIONTEXT 19
#define COLOR_BTNHIGHLIGHT 20

#define COLOR_3DDKSHADOW 21
#define COLOR_3DLIGHT 22
#define COLOR_INFOTEXT 23
#define COLOR_INFOBK 24
#define COLOR_HOTLIGHT 26
#define COLOR_GRADIENTACTIVECAPTION 27
#define COLOR_GRADIENTINACTIVECAPTION 28
#define COLOR_MENUHILIGHT 29
#define COLOR_MENUBAR 30

#define COLOR_DESKTOP COLOR_BACKGROUND
#define COLOR_3DFACE COLOR_BTNFACE
#define COLOR_3DSHADOW COLOR_BTNSHADOW
#define COLOR_3DHIGHLIGHT COLOR_BTNHIGHLIGHT
#define COLOR_3DHILIGHT COLOR_BTNHIGHLIGHT
#define COLOR_BTNHILIGHT COLOR_BTNHIGHLIGHT

/* TRUE/FALSE */
#define FALSE   0
#define TRUE    1

#define CALLBACK

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
typedef unsigned short WORD;
typedef int BOOL;
typedef int LONG;
typedef unsigned int DWORD;
typedef unsigned int UINT;

typedef long LONG_PTR; // 32 bit on 32 bit, 64 bit on 64 bit.
typedef unsigned long ULONG_PTR; // 32 bit on 32 bit, 64 bit on 64 bit.
typedef uintptr_t UINT_PTR; // 32 bit on 32 bit, 64 bit on 64 bit.
typedef ULONG_PTR DWORD_PTR;
typedef ULONG_PTR *PDWORD_PTR; // 32 bit on 32 bit, 64 bit on 64.
typedef LONG_PTR LRESULT;

typedef char *LPTSTR;
typedef char *LPSTR;
typedef char *LPCSTR;
typedef void *LPVOID;

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
typedef void *HANDLE;

typedef struct WndClass WndClass;
typedef struct WndDC *HDC;
typedef struct Wnd *HWND;
typedef struct WndMenu *HMENU;
typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

/* GDI objects */
typedef struct GDIOBJ *HGDIOBJ;
typedef struct GDIOBJ *HFONT;
typedef struct GDIOBJ *HBRUSH;
typedef struct GDIOBJ *HPEN;
typedef struct GDIOBJ *HRGN;

/* XXX: Fix */
typedef void *HCURSOR;
typedef void *HBITMAP;

typedef struct tagWNDCLASS {
  const char *lpszClassName;
  HBRUSH hbrBackground;
  WNDPROC lpfnWndProc;
  HCURSOR hCursor;
  size_t cbWndExtra;
} WNDCLASS;

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

typedef struct tagMENUITEMINFO {
	UINT cbSize;
	UINT fMask;
	UINT fType;
	UINT fState;
	UINT wID;
	HMENU hSubMenu;
	HBITMAP hbmpChecked;
	HBITMAP hbmpUnchecked;
	DWORD dwItemData;
	LPTSTR dwTypeData;
	UINT cch;
} MENUITEMINFO,*LPMENUITEMINFO;
typedef MENUITEMINFO const *LPCMENUITEMINFO;

#include <winuser.h>
#include <wingdi.h>

int RegisterClass(WNDCLASS *wndclass);

#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_SHOW 5

void ShowWindow(HWND wnd, int nCmdShow);
void SetWindowName(HWND wnd, const char *name);
int GetWindowText(HWND wnd, char *lpString, int nMaxCount);
void DestroyWindow(HWND wnd);
HWND GetParent(HWND wnd);
BOOL IsWindow(HWND wnd);

HGDIOBJ GetStockObject(int fnObject);
DWORD GetSysColor(int nIndex);
HPEN CreatePen(int fnPenStyle, int nWidth, COLORREF crColor);

BOOL DeleteObject(HGDIOBJ hObject);
HGDIOBJ SelectObject(HDC hdc, HGDIOBJ hgdiobj);
int GetObject(HANDLE h, int c, LPVOID pv);
COLORREF SetDCBrushColor(HDC hdc, COLORREF crColor);
BOOL TextOut(HDC hdc, int nXStart, int nYStart, const char *lpString,
    size_t cchString);
BOOL Ellipse(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
    int nBottomRect);
BOOL Rectangle(HDC hdc, int nLeftRect, int nTopRect,
  int nRightRect, int nBottomRect);
BOOL FillRect(HDC hdc, const RECT *lprc, HBRUSH hbr);

void *GetWindowLongPtr(HWND wnd, int nIndex);
int SendMessage(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam);
LRESULT DefWindowProc(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam);

BOOL GetMessage(LPMSG msg, HWND wnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
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

#define MF_BITMAP	4
#define MF_CHECKED	8
#define MF_DISABLED	2
#define MF_ENABLED	0
#define MF_GRAYED	1
#define MF_MENUBARBREAK	32
#define MF_MENUBREAK	64
#define MF_OWNERDRAW	256
#define MF_POPUP	16
#define MF_SEPARATOR	0x800
#define MF_STRING	0
#define MF_UNCHECKED	0
#define MF_DEFAULT	4096
#define MF_SYSMENU	0x2000
#define MF_HELP		0x4000
#define MF_END	128
#define MF_RIGHTJUSTIFY 0x4000
#define MF_MOUSESELECT 0x8000
#define MIIM_CHECKMARKS	8
#define MIIM_DATA	32
#define MIIM_ID	2
#define MIIM_STATE	1
#define MIIM_SUBMENU	4
#define MIIM_TYPE	16
#define MIIM_STRING	0x040
#define MIIM_BITMAP	0x080
#define MIIM_FTYPE	0x0100
#define MFT_BITMAP	4
#define MFT_MENUBARBREAK	32
#define MFT_MENUBREAK	64
#define MFT_OWNERDRAW	256
#define MFT_RADIOCHECK	512
#define MFT_RIGHTJUSTIFY	0x4000
#define MFT_SEPARATOR	0x800
#define MFT_STRING	0
#define MFS_CHECKED	8
#define MFS_DEFAULT	4096
#define MFS_DISABLED	3
#define MFS_ENABLED	0
#define MFS_GRAYED	3
#define MFS_HILITE	128
#define MFS_UNCHECKED	0
#define MFS_UNHILITE	0

#define MF_BYPOSITION	0x400


/* Menus */
BOOL AppendMenu(HMENU menu, UINT flags, UINT_PTR id, LPCSTR title);
BOOL InsertMenu(HMENU menu, UINT pos, UINT flags, UINT_PTR id, LPCSTR ptr);
BOOL InsertMenuItem(HMENU menu, UINT pos, BOOL bypos, LPCMENUITEMINFO info);
HMENU CreateMenu(void);
HMENU CreatePopupMenu(void);
BOOL IsMenu(HMENU menu);

#ifdef __cplusplus
}
#endif

#endif /* __WINDOWS_H__ */
