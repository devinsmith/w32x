/*	$Id: rect.c,v 1.12 2006/01/31 17:09:35 tkent Exp $	*/

/*
 * Copyright 2000 Masaru OKI
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

#include <windows.h>

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

BOOL
SetRect(RECT *r, int left, int top, int right, int bottom)
{
	r->top = top;
	r->bottom = bottom;
	r->left = left;
	r->right = right;
	return TRUE;
}

/*
 * all members are set to zero
 */
BOOL
SetRectEmpty(RECT *r)
{
	r->top = 0;
	r->bottom = 0;
	r->left = 0;
	r->right = 0;
	return TRUE;
}

/*
 * no area?
 */
BOOL
IsRectEmpty(const RECT *r)
{
	if (r->top >= r->bottom || r->left >= r->right)
		return TRUE;
	return FALSE;
}

BOOL
IntersectRect(RECT *r1, const RECT *r2, const RECT *r3)
{
	if (r2->right < r3->left || r3->right < r2->left) {
		SetRectEmpty(r1);
		return FALSE;
	}
	if (r2->bottom < r3->top || r3->bottom < r2->top) {
		SetRectEmpty(r1);
		return FALSE;
	}
	r1->left = r2->left > r3->left ? r2->left : r3->left;
	r1->top = r2->top > r3->top ? r2->top : r3->top;
	r1->right = r2->right < r3->right ? r2->right : r3->right;
	r1->bottom = r2->bottom < r3->bottom ? r2->bottom : r3->bottom;
	if (r1->left >= r1->right || r1->top >= r1->bottom) {
		SetRectEmpty(r1);
		return FALSE;
	}
	return TRUE;
}

BOOL
InflateRect(RECT *r, int dx, int dy)
{
	r->left -= dx;
	r->right += dx;
	r->top -= dy;
	r->bottom += dy;
	return TRUE;
}

BOOL
OffsetRect(RECT *r, int xoff, int yoff)
{
	r->top += yoff;
	r->bottom += yoff;
	r->left += xoff;
	r->right += xoff;
	return TRUE;
}

BOOL
EqualRect(const RECT *r1, const RECT *r2)
{
	if (r1->top != r2->top) return FALSE;
	if (r1->bottom != r2->bottom) return FALSE;
	if (r1->left != r2->left) return FALSE;
	if (r1->right != r2->right) return FALSE;
	return TRUE;
}

BOOL
PtInRect(const RECT *r, POINT p)
{
	if (r->top > p.y || r->bottom < p.y) return FALSE;
	if (r->left > p.x || r->right < p.x) return FALSE;
	return TRUE;
}

/*
 * copies the all members of rectangle
 */
BOOL
CopyRect(RECT *dst, const RECT *src)
{
	dst->top = src->top;
	dst->left = src->left;
	dst->bottom = src->bottom;
	dst->right = src->right;
	return TRUE;
}

/*
 * dst = r1 | r2;
 */
BOOL
UnionRect(RECT *dst, const RECT *r1, const RECT *r2)
{
	dst->top = min(r1->top, r2->top);
	dst->left = min(r1->left, r2->left);
	dst->bottom = max(r1->bottom, r2->bottom);
	dst->right = max(r1->right, r2->right);
	return TRUE;
}

/*
 * dst = r1 - r2;
 * This makes sense only when vertical or horizontal position of r2 includes r1.
 */
BOOL
SubtractRect(RECT *dst, const RECT *r1, const RECT *r2)
{
	RECT ret;

				/* r2 includes r1. */
	if (r2->left <= r1->left && r1->right <= r2->right
	    && r2->top <= r1->top && r1->bottom <= r2->bottom) {
		SetRectEmpty(dst); /* Returns an empty rectangle. */
		return TRUE;
	}

	ret = *r1;

	/* Vertical position of r2 includes r1. */
	if (r2->top <= ret.top && ret.bottom <= r2->bottom) {

		/* not overlapped horizontally ==> nothing to do */
		if (r2->right < ret.left || ret.right < r2->left) {
			*dst = ret;
			return TRUE;
		}

		/* overlapped but r2 is narrower ==> nothing to do */
		if (ret.left < r2->left && r2->right < ret.right) {
			*dst = ret;
			return TRUE;
		}

		/* r2 overlaps the left side of r1 */
		if (r2->right < ret.right)
			ret.left = r2->right;

		/* r2 overlaps the right side of r1 */
		if (ret.left < r2->left)
			ret.right = r2->left;

	/* Horizontal position of r2 includes r1.
	 * No need to handle `horizontal && vertical.' */
	} else if (r2->left <= ret.left && ret.right <= r2->right) {

		/* not overlapped vertically ==> nothing to do */
		if (r2->bottom < ret.top || ret.bottom < r2->top) {
			*dst = ret;
			return TRUE;
		}

		/* overlapped but r2 is narrower ==> nothing to do */
		if (ret.top < r2->top && r2->bottom < ret.bottom) {
			*dst = ret;
			return TRUE;
		}

		/* r2 overlaps the upper side of r1 */
		if (r2->bottom < ret.bottom)
			ret.top = r2->bottom;

		/* r2 overlaps the bottom side of r1 */
		if (ret.top < r2->top)
			ret.bottom = r2->top;
	}
	*dst = ret;
	return TRUE;
}
