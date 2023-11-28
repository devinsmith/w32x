/*
 * Copyright (c) 2000 Masaru OKI
 * Copyright (c) 2001 TAMURA Kent
 * Copyright (c) 2017-2018 Devin Smith <devin@devinsmith.net>
 * Copyright (c) 2018 Robert Butler <me@r-butler.net>
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

#define _GNU_SOURCE
#include <sys/queue.h>
#include <sys/select.h>
#include <errno.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#include <windows.h>
#include "w32x_priv.h"

Colormap colormap;
int blackpixel;
int whitepixel;
Display *disp;
XContext ctxt;

static DWORD last_error;

struct msgq_entry {
	MSG msg;
	TAILQ_ENTRY(msgq_entry) entries;
};

struct paint_entry {
	Wnd *wnd;

	TAILQ_ENTRY(paint_entry) entries;
};

TAILQ_HEAD(msg_queue, msgq_entry) g_msg_queue;
TAILQ_HEAD(paint_queue, paint_entry) g_paint_queue;

static WndClass *class_list = NULL;

extern WNDCLASS ButtonClass;
extern WNDCLASS MenuClass;

extern int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPTSTR pCmdLine, int nCmdShow);

/* Special atom for deleted messages */
Atom WM_DELETE_WINDOW;

struct dl_phdr_info *module;

static int
callback(struct dl_phdr_info *info, size_t size, void *data)
{
	module = info;
	return 0;
}

int
main(int argc, char *argv[])
{
	int i;
	size_t length = 0;
	int result = 1;
	const char *display = getenv("DISPLAY");
	dl_iterate_phdr(callback, NULL);

	if ((display == NULL) ||
	    (disp = XOpenDisplay(display)) == NULL) {
		fprintf(stderr, "Unable to open display.\n");
		exit(1);
	}

	colormap = DefaultColormap(disp, DefaultScreen(disp));

	/* Initialize color */
	blackpixel = BlackPixel(disp, DefaultScreen(disp));
	whitepixel = WhitePixel(disp, DefaultScreen(disp));

	/* Initialize lpCmdLine. */
	for (i=0; i < argc; i++) {
		length += strlen(argv[i]);
		length++; /* space or final '\0' character */
	}

	char *lpCmdLine = calloc(1, length);
	length = 0;

	for (i = 0; i < argc; i++) {
		strcat(lpCmdLine + length, argv[i]);
		length += strlen(argv[i]);
		lpCmdLine[length++] = ' ';
	}
	lpCmdLine[length] = '\0';

	/* Capture the WM_DELETE_WINDOW atom, if it exists. We'll use this later
	 * to tell the window manager that we're interested in handling the
	 * close/delete events for top level windows */
	WM_DELETE_WINDOW = XInternAtom(disp, "WM_DELETE_WINDOW", 0);
	ctxt = XUniqueContext();

	TAILQ_INIT(&g_msg_queue);
	TAILQ_INIT(&g_paint_queue);

	/* Register built in classes. */
	RegisterClass(&ButtonClass);
	RegisterClass(&MenuClass);

	result = WinMain((HINSTANCE) module->dlpi_addr, (HINSTANCE) 0, lpCmdLine,
	    SW_SHOWNORMAL);

	free(lpCmdLine);

	return result;
}

WndClass *get_class_by_name(const char *name)
{
	WndClass *wc;

	/* Iterate through list of window classes to see if this
	 * class has already been registered. */
	for (wc = class_list; wc != NULL; wc = wc->next) {
		if (strcmp(wc->name, name) == 0) {
			return wc;
		}
	}

	return NULL;
}

int
RegisterClass(WNDCLASS *wndClass)
{
	WndClass *wc;
	XColor back_col;
	char rgb_template[13];
	COLORREF cr;

	if (wndClass == NULL)
		return -1;

	wc = get_class_by_name(wndClass->lpszClassName);
	if (wc != NULL) {
		/* Already registered */
		return 1;
	}

	if (wndClass->hbrBackground >= (HBRUSH)(COLOR_SCROLLBAR + 1) &&
	    wndClass->hbrBackground <= (HBRUSH)(COLOR_MENUBAR + 1)) {
		cr = GetSysColor(((long)wndClass->hbrBackground) - 1);
	} else {
		LOGBRUSH lb;

		GetObject(wndClass->hbrBackground, sizeof(LOGBRUSH), &lb);
		cr = lb.lbColor;
	}

	snprintf(rgb_template, sizeof(rgb_template), "rgb:%02x/%02x/%02x",
	    (cr & 0xff), ((cr & 0xffff) >> 8) & 0xff, (cr >> 16) & 0xff);
	/* Lookup colors */
	XParseColor(disp, colormap, rgb_template, &back_col);
	XAllocColor(disp, colormap, &back_col);

	/* Register a new class */
	wc = calloc(1, sizeof(WndClass));
	wc->name = strdup(wndClass->lpszClassName);
	wc->border_pixel = blackpixel;
	wc->background_pixel = back_col.pixel;
	wc->wndExtra = wndClass->cbWndExtra;
	wc->proc = wndClass->lpfnWndProc;
	wc->next = class_list;
	class_list = wc;

	return 0;
}

void
ShowWindow(HWND wnd, int nCmdShow)
{
	switch (nCmdShow) {
	case SW_HIDE:
		XUnmapWindow(disp, wnd->window);
		break;
	case SW_SHOW:
	case SW_SHOWNORMAL:
		XMapWindow(disp, wnd->window);
		break;
	default:
		break;
	}
}

void
DestroyWindow(HWND wnd)
{
	SendMessage(wnd, WM_DESTROY, 0, 0);

	XDestroyWindow(disp, wnd->window);
}

void
SetLastError(DWORD err)
{
  last_error = err;
}

void
SetWindowName(HWND wnd, const char *name)
{
	XTextProperty wname;

	if (XStringListToTextProperty((char **)&name, 1, &wname) == 0) {
		fprintf(stderr, "Can't allocate window name\n");
		return;
	}
	XSetWMIconName(disp, wnd->window, &wname);
	XSetWMName(disp, wnd->window, &wname);
	XFree(wname.value);
}

int
GetWindowText(HWND wnd, char *lpString, int nMaxCount)
{
	snprintf(lpString, nMaxCount, "%s", wnd->label);
	return strlen(lpString);
}

int
SendMessage(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC proc = (wnd == NULL || wnd->proc == NULL)
	    ? DefWindowProc : wnd->proc;

	return proc(wnd, msg, wParam, lParam);
}

void *GetWindowLongPtr(HWND wnd, int nIndex)
{
	return wnd->wndExtra;
}

void PostQuitMessage(int nExitCode)
{
	struct msgq_entry *q_msg;

	q_msg = malloc(sizeof(struct msgq_entry));
	q_msg->msg.message = WM_QUIT;

	TAILQ_INSERT_TAIL(&g_msg_queue, q_msg, entries);
}

static void translate_xevent_to_msg(XEvent *e, LPMSG msg)
{
	RECT r;
	Wnd *win = NULL;

	XFindContext(e->xany.display, e->xany.window, ctxt, (XPointer *)&win);

	msg->hwnd = win;

	/* Translate XEvent structure to WM messages */
	switch (e->type) {
	case ReparentNotify:
	case MapNotify:
	case VisibilityNotify:
		// Nothing to do for this event.
		break;
	case ConfigureNotify:
		// What do we do for this?
		msg->message = WM_SIZE;
		msg->wParam = 0;
		break;
	case ButtonPress:
		if (e->xbutton.button == 1) {
			msg->message = WM_LBUTTONDOWN;
			msg->wParam = 0;
			msg->lParam = 0;
		}
		break;
	case ButtonRelease:
		if (e->xbutton.button == 1) {
			msg->message = WM_LBUTTONUP;
			msg->wParam = 0;
			msg->lParam = 0;
		}
		break;
	case Expose:
		/*
		 * set clip rectangle for client area.
		 */
		r.left = e->xexpose.x - 0;
		if (r.left < 0)
			r.left = 0;
		r.top = e->xexpose.y - 0;
		if (r.top < 0)
			r.top = 0;
		r.right = e->xexpose.x + e->xexpose.width;
		if (r.right < 0)
			r.right = 0;
		r.bottom = e->xexpose.y + e->xexpose.height;
		if (r.bottom < 0)
			r.bottom = 0;

		InvalidateRect(msg->hwnd, &r, TRUE);
		if (e->xexpose.count == 0) {
#if 0
			/* Don't bubble this up, instead put it in a queue */
			msg->message = WM_PAINT;
			msg->wParam = 0;
			msg->lParam = 0;
#endif
			UpdateWindow(msg->hwnd);
		}
		break;
	case ClientMessage:
		if (e->xclient.format == 32 && e->xclient.data.l[0] == WM_DELETE_WINDOW) {
			msg->message = WM_CLOSE;
			msg->wParam = 0;
			msg->lParam = 0;
		} else {
			printf("Unhandled client message!\n");
		}
		break;
	default:
		printf("Unhandled event %d\n", e->type);
		break;
	}
}

static void w32x_process_xevent(LPMSG msg)
{
	XEvent event;

	XNextEvent(disp, &event);

	/* Compress configure events */
	if (event.xany.type == ConfigureNotify) {
		XEvent e;

		while (XCheckTypedWindowEvent(disp, event.xconfigure.window,
		    ConfigureNotify, &e)) {
			event.xconfigure.width = e.xconfigure.width;
			event.xconfigure.height = e.xconfigure.height;
			if (e.xconfigure.send_event) {
				/* MOVE */
				event.xconfigure.x = e.xconfigure.x;
				event.xconfigure.y = e.xconfigure.y;
			}
		}
	}

	/* Process events */
	translate_xevent_to_msg(&event, msg);
}

BOOL GetMessage(LPMSG msg, HWND wnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	struct msgq_entry *q_msg;
	struct paint_entry *q_paint;
	int x_fd, fds, nf;
	fd_set readset;
	struct timeval tmout, *tptr;

	msg->message = 0;

	/* Something waiting, handle right away */
	if (XPending(disp)) {
		w32x_process_xevent(msg);
		if (msg->message != 0) {
			return TRUE;
		}
	}

	/* Do a quick poll */
	x_fd = ConnectionNumber(disp);
	fds = x_fd + 1;

	FD_ZERO(&readset);
	FD_SET(x_fd, &readset);

	tmout.tv_sec = tmout.tv_usec = 0;
	nf = select(fds, &readset, NULL, NULL, &tmout);
	if (nf <= 0) {
		FD_ZERO(&readset);
	}

	while (1) {

		if ((nf > 0) || ((nf == -1) && (errno == EINTR))) {
			if (FD_ISSET(x_fd, &readset)) {
				w32x_process_xevent(msg);
				if (msg->message != 0) {
					return TRUE;
				}
				continue;
			} else {
//				return TRUE;
			}
		}

		/* Anything in the queue? */
		q_msg = TAILQ_FIRST(&g_msg_queue);
		if (q_msg != NULL) {
			if (q_msg->msg.message == WM_QUIT) {
				free(q_msg);
				return FALSE;
			}

			TAILQ_REMOVE(&g_msg_queue, q_msg, entries);
			msg->hwnd = q_msg->msg.hwnd;
			msg->message = q_msg->msg.message;
			msg->lParam = q_msg->msg.lParam;
			msg->wParam = q_msg->msg.wParam;
			free(q_msg);
			return TRUE;
		}

		/* Anything in the paint queue? */
		TAILQ_FOREACH(q_paint, &g_paint_queue, entries) {
			printf("TODO: Handle Event.\n");
		
		}

		tptr = NULL;
		fds = x_fd + 1;
		tmout.tv_sec = tmout.tv_usec = 0;
		FD_ZERO(&readset);
		FD_SET(x_fd, &readset);

		nf = select(fds, &readset, NULL, NULL, &tmout);
		if (nf <= 0) {
			FD_ZERO(&readset);
		} else {
			continue;
		}

		/* sleep until next event */
		fds = x_fd + 1;
		FD_ZERO(&readset);
		FD_SET(x_fd, &readset);

		nf = select(fds, &readset, NULL, NULL, tptr);
		if (nf == -1) {
			FD_ZERO(&readset);
		}
	}

	return TRUE;
}

HWND GetParent(HWND wnd)
{
	HWND parent = 0;

	if (IsWindow(wnd))
	{
		parent = wnd->parent;
	}

	return parent;
}

int DispatchMessage(const MSG *msg)
{
	Wnd *wnd = msg->hwnd;

	wnd->proc(wnd, msg->message, msg->wParam, msg->lParam);

	return 0;
}
