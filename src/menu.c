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
#include <sys/queue.h>
#include <windows.h>

#define MENU_MAGIC 0x574d4e55 /* 'WMNU' */

struct WndMenu {
	int magic;
	HWND menuwnd;
};

struct w32x_menuitem {
	MENUITEMINFO info;
	TAILQ_ENTRY(w32x_menuItem) list;
};

static LRESULT CALLBACK MenuWindowProc(HWND wnd, unsigned int msg,
    WPARAM wParam, LPARAM lParam);

WNDCLASS MenuClass = {
	"#32768", (HBRUSH)(COLOR_MENUBAR + 1), MenuWindowProc, NULL, 0
};

static LRESULT CALLBACK
MenuWindowProc(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_PAINT:
		printf("Need to paint menubar\n");
		break;
	case WM_SIZE:
		printf("need to size\n");
		break;
	default:
		break;
	}
	return DefWindowProc(wnd, msg, wParam, lParam);
}

BOOL
IsMenu(HMENU menu)
{
	if (menu == NULL)
		return FALSE;

	return menu->magic == MENU_MAGIC;
}

HMENU
CreateMenu(void)
{
	struct WndMenu *menu;

	menu = calloc(1, sizeof(struct WndMenu));
	menu->magic = MENU_MAGIC;

	return menu;
}

HMENU
CreatePopupMenu(void)
{
	return CreateMenu();
}

BOOL
AppendMenu(HMENU menu, UINT flags, UINT id, LPCSTR title)
{
	return TRUE;
}

static BOOL
menuitem_insert_item(struct w32x_menuitem *mi, UINT id,
    struct w32x_menuitem *item)
{
	struct w32x_menuitem *after;

	return FALSE;
}

static void
copy_menuiteminfo(MENUITEMINFO *dinfo, DWORD mask, LPCMENUITEMINFO sinfo)
{
	if (mask & MIIM_STATE)
		dinfo->fState = sinfo->fState;
	if (mask & MIIM_ID)
		dinfo->wID = sinfo->wID;
	if (mask & MIIM_SUBMENU)
		dinfo->hSubMenu = sinfo->hSubMenu;
	if (mask & MIIM_CHECKMARKS) {
		dinfo->hbmpChecked = sinfo->hbmpChecked;
		dinfo->hbmpUnchecked = sinfo->hbmpUnchecked;
	}
	if (mask & MIIM_TYPE) {
		dinfo->fType = sinfo->fType;
		dinfo->dwTypeData = sinfo->dwTypeData;
	}
	if (mask & MIIM_DATA)
		dinfo->dwItemData = sinfo->dwItemData;
	if (mask & MIIM_STRING) {
		dinfo->dwTypeData = sinfo->dwTypeData;
		dinfo->cch = sinfo->cch;
	}
#if (_WIN32_WINNT >= 0x0500)
	if (mask & MIIM_BITMAP)
		dinfo->hbmpItem = sinfo->hbmpItem;
#endif
	if (mask & MIIM_FTYPE)
		dinfo->fType = sinfo->fType;
	dinfo->fMask = sinfo->fMask;
	if (dinfo->fMask & MIIM_TYPE)
		dinfo->fMask |= MIIM_FTYPE | MIIM_STRING;
}

BOOL
InsertMenuItem(HMENU menu, UINT pos, BOOL bypos, LPCMENUITEMINFO info)
{
	struct w32x_menuitem *item, *after;

	if (!IsMenu(menu)) {
		return FALSE;
	}

	item = malloc(sizeof(struct w32x_menuitem));
	memset(item, 0, sizeof(struct w32x_menuitem));
	item->info.cbSize = sizeof(item->info);
	copy_menuiteminfo(&item->info, info->fMask, info);
	if (info->fType == MFT_STRING) {
		item->info.dwTypeData = strdup(info->dwTypeData);
	}

	return TRUE;
}
