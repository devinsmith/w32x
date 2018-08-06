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

struct w32x_menuitem {
	MENUITEMINFO info;
	TAILQ_ENTRY(w32x_menuitem) list;
};

struct WndMenu {
	int magic;
	unsigned int nitem;
	HWND menuwnd;
	HMENU parent;
	TAILQ_HEAD(menulist, w32x_menuitem) item;
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
	menu->nitem = 0;
	menu->parent = NULL;
	TAILQ_INIT(&menu->item);

	return menu;
}

HMENU
CreatePopupMenu(void)
{
	return CreateMenu();
}

BOOL
AppendMenu(HMENU menu, UINT flags, UINT_PTR id, LPCSTR title)
{
	return InsertMenu(menu, 0xffffffff, flags | MF_BYPOSITION, id, title);
}

static BOOL
menu_insert_item(HMENU menu, UINT id, struct w32x_menuitem *item)
{
	struct w32x_menuitem *after;
	HMENU submenu;

	TAILQ_FOREACH(after, &menu->item, list) {
		if (after->info.wID == id) {
			TAILQ_INSERT_BEFORE(after, item, list);
			menu->nitem++;
			return TRUE;
		}
		if (after->info.hSubMenu != NULL) {
			submenu = after->info.hSubMenu;
			if (menu_insert_item(submenu, id, item))
				return TRUE;
		}
	}

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
InsertMenu(HMENU menu, UINT pos, UINT flags, UINT_PTR id, LPCSTR ptr)
{
	MENUITEMINFO info;

	if (!IsMenu(menu)) {
		return FALSE;
	}
	memset(&info, 0, sizeof(info));
	info.cbSize = sizeof(info);
	info.fMask = MIIM_STATE | MIIM_ID | MIIM_TYPE;
	info.fState = MFS_ENABLED;
	if (flags & MF_POPUP) {
		info.hSubMenu = (HMENU)id;
		info.fMask |= MIIM_SUBMENU;
	} else {
		info.hSubMenu = NULL;
	}
	if (flags & MF_CHECKED) {
		info.fState = MFS_CHECKED;
	}
	info.wID = id;
	info.dwTypeData = (LPSTR)ptr;
	if (flags & MF_BITMAP) {
		info.fType = MFT_BITMAP;
	} else if (flags & MF_OWNERDRAW) {
		info.fType = MFT_OWNERDRAW;
	} else if (flags & MF_SEPARATOR) {
		info.fType = MFT_SEPARATOR;
	} else {
		/* Default: MF_STRING */
		info.fType = MFT_STRING;
		if (ptr != NULL) {
			info.cch = strlen(ptr);
		} else {
			info.cch = 0;
			info.dwTypeData = "";
		}
	}
	return InsertMenuItem(menu, pos, (flags & MF_BYPOSITION) ? TRUE : FALSE,
			&info);
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

	if (bypos != FALSE) {
		unsigned int i;
		/*
		 * pos == position (nth item)
		 */
		after = TAILQ_FIRST(&menu->item);
		for (i = 0; after != NULL && i < pos; i++) {
			after = TAILQ_NEXT(after, list);
		}
		if (after != NULL) {
			TAILQ_INSERT_BEFORE(after, item, list);
		} else {
			TAILQ_INSERT_TAIL(&menu->item, item, list);
		}
		menu->nitem++;
	} else {
		/*
		 * pos == item ID
		 */
		if (!menu_insert_item(menu, pos, item)) {
			printf("%s: No specified ID %u in HMENU %p\n", __func__,
			    pos, menu);
			free(item);
			return FALSE;
		}
	}
	if (item->info.hSubMenu != NULL) {
		HMENU psm = item->info.hSubMenu;
		psm->parent = menu;
	}
	return TRUE;
}
