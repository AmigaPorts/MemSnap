#ifndef _MEMSNAP_MENU_H_
#define _MEMSNAP_MENU_H_

/*
 *	Menunumbers for MemSnap's menu.
 */

#include <intuition/intuition.h>

#define PROJECT 0

#define SMALL	  0
#define LARGE	  1
#define ABOUT	  2
#define QUIT	  3

struct Menu *AllocMemSnapMenu(struct Window *);
void FreeMemSnapMenu(void);

#endif // _MEMSNAP_MENU_H_
