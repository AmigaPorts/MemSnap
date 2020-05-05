/*
*	menu.c
*
*	NewMenus for MemSnap
*
*	MWS, 2/93.
*/

#include "menu.h"
#include <exec/types.h>
#include <libraries/gadtools.h>
#include <proto/gadtools.h>

static struct NewMenu gt_items[] = {
	{ NM_TITLE, (const STRPTR)"MemSnap", NULL, 0, 0, NULL },
	{ NM_ITEM,  (const STRPTR)"Small window", (const STRPTR)".", 0, 0, NULL },
	{ NM_ITEM,  (const STRPTR)"Large window", (const STRPTR)"/",0, 0, NULL },
	{ NM_ITEM,  (const STRPTR)"About...", (const STRPTR)"A",0, 0, NULL },
	{ NM_ITEM,  (const STRPTR)"Quit", (const STRPTR)"Q", 0, 0, NULL },

	{ NM_END }
};

static APTR vi;			/* visual info for gadtools */
static struct Menu *menu;

void FreeMemSnapMenu()		/* free all menustrips and related data */
{
	if (menu) FreeMenus(menu);
	if (vi) FreeVisualInfo(vi);
}

struct Menu *
AllocMemSnapMenu(struct Window *window)	/* allocate menustrips required by program */
{
	if ((menu = CreateMenusA(gt_items, NULL)) &&
	    (vi = GetVisualInfoA(window->WScreen, NULL)) &&
 	    (LayoutMenusA(menu, vi, NULL)))
		return menu;

	FreeMemSnapMenu();	/* something failed... */
	return NULL;
}
