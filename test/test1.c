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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

static int
MainWindowProc(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(wnd, msg, wParam, lParam);
}

static int
MenuBarProc(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_PAINT:
    printf("Need to paint menubar\n");
    break;
  default:
    break;
  }
  return DefWindowProc(wnd, msg, wParam, lParam);
}

int
main(int argc, char *argv[])
{
  HWND top;
  WNDCLASS myClass;
  WNDCLASS mbClass;
  MSG msg;
  BOOL bRet;

  /* Open display */
  if (w32x_init(NULL) != 0) {
    fprintf(stderr, "Cannot open display.\n");
    exit(1);
  }

  myClass.Name = "TopWindow";
  myClass.BackgroundColor = C_GRAY2;
  myClass.wndExtra = 0;
  myClass.EventProc = MainWindowProc;

  RegisterClass(&myClass);

  /* Setup menu bar class */
  memset(&mbClass, 0, sizeof(WNDCLASS));
  mbClass.Name = "MenuBar";
  mbClass.BackgroundColor = C_GRAY4;
  mbClass.wndExtra = 0;
  mbClass.EventProc = MenuBarProc;
  RegisterClass(&mbClass);


  /* parent window */
  top = CreateWindowEx(WS_EX_CLIENTEDGE, "TopWindow", "Test1", WS_BORDER,
      200, 200, 500, 300, NULL);
  SetMenu(top);

  CreateWindow("MenuBar", "Nothing", 0, 0, 0, 500, 25, top);

  CreateWindow("RadioButton", "Radio 1", WS_BORDER, 5, 30, 100, 25, top);
  CreateWindow("RadioButton", "Radio 2", WS_BORDER, 5, 60, 100, 25, top);

  ShowWindow(top);
  UpdateWindow(top);

  while ((bRet = GetMessage(&msg, NULL)) != 0) {
    DispatchMessage(&msg);
  }

  return 0;
}
