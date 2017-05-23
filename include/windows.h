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

typedef int BOOL;
typedef int LONG;
typedef unsigned int DWORD;
typedef unsigned int UINT;

typedef long LONG_PTR; // 32 bit on 32 bit, 64 bit on 64 bit.
typedef unsigned long ULONG_PTR; // 32 bit on 32 bit, 64 bit on 64 bit.
typedef uintptr_t UINT_PTR; // 32 bit on 32 bit, 64 bit on 64 bit.

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;

typedef DWORD COLORREF;
#define RGB(r,g,b) ((COLORREF)((r) | ((g) << 8) | ((b) << 16)))

/* GDI objects */
typedef void *HGDIOBJ;
typedef void *HFONT;
typedef void *HBRUSH;
typedef void *HPEN;

/* Events */
#define WM_CREATE                       0x0001
#define WM_DESTROY                      0x0002
#define WM_PAINT                        0x000F
#define WM_CLOSE                        0x0010
#define WM_QUIT                         0x0012
#define WM_NCPAINT                      0x0085
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202

#define WS_BORDER   0x00800000L

#define MSG_INT 1
#define MSG_PTR 2

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


#define CLR_INVALID 0xFFFFFFFF


/* Forward declartions */
struct Wnd;
struct WndClass;

typedef struct Wnd Wnd;
typedef struct WndClass WndClass;
typedef struct WndDC *HDC;
typedef struct Wnd *HWND;
typedef struct WndMenu *HMENU;

typedef struct tagWNDCLASS {
  const char *Name;
  const char *BackgroundColor;
  int (*EventProc)(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam);
  size_t wndExtra;
} WNDCLASS;

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

int w32x_init(const char *display_name);


int RegisterClass(WNDCLASS *wndclass);

void ShowWindow(HWND wnd);
void SetWindowName(HWND wnd, const char *name);
int GetWindowText(HWND wnd, char *lpString, int nMaxCount);
void DestroyWindow(HWND wnd);

HDC BeginPaint(HWND wnd);
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

BOOL GetMessage(LPMSG msg, Wnd *wnd);
int DispatchMessage(const MSG *msg);
void PostQuitMessage(int nExitCode);

#include <winuser.h>
#include <wingdi.h>

#endif /* __WINDOWS_H__ */
