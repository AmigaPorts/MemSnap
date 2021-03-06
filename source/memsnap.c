/*
*	MemSnap.c
*
*	Just like MeMeter (a program I've been using for ages now).
*	Why write it? The MeMeter I've been using crashes under 2.0,
*	and I thought I'd take the time to learn (a bit) about 2.0
*	by programming with it. Nothing groundbreaking here, though.
*
*	Martin W Scott, 3/92.
*/
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <graphics/text.h>
#include "wintext.h"		/* font-independent positioning of text */
#include "icon.h"
#include "menu.h"
#include "version.h"

#define CHARS_ACROSS	41	/* max chars across for window texts */
#define CHARS_DOWN	 4	/* how many rows of text in window */
#define REMOVECHARS	27	/* how many cols removed when window small */

char large_header[] = "       Current Snapshot   In use     Peak";
char small_header[] = "    Memory";

WINTEXT wtexts[] =
{
    {&wtexts[1], large_header, 0, 0, 3, 0, JAM2},
    {&wtexts[2], " Chip", 0, 1, 2, 0, JAM2},
    {&wtexts[3], " Fast", 0, 2, 2, 0, JAM2},
    {NULL, "Total", 0, 3, 2, 0, JAM2}
};

LONG peakvals[3] = {0, 0, 0};
char cbuf[3][9], sbuf[3][9], ubuf[3][9], pbuf[3][9];

// "Current Snapshot" / "Memory" column
WINTEXT ctexts[] =
{   // next text x y pen bg mode
    {&ctexts[1], &cbuf[0][0], 6, 1, 1, 0, JAM2},
    {&ctexts[2], &cbuf[1][0], 6, 2, 1, 0, JAM2},
    {NULL, &cbuf[2][0], 6, 3, 1, 0, JAM2}
};

// "Snapshot" column
WINTEXT stexts[] =
{   // next text x y pen bg mode
    {&stexts[1], &sbuf[0][0], 15, 1, 1, 0, JAM2},
    {&stexts[2], &sbuf[1][0], 15, 2, 1, 0, JAM2},
    {NULL, &sbuf[2][0], 15, 3, 1, 0, JAM2}
};

// "Used" column
WINTEXT utexts[] =
{   // next text x y pen bg mode
    {&utexts[1], &ubuf[0][0], 24, 1, 1, 0, JAM2},
    {&utexts[2], &ubuf[1][0], 24, 2, 1, 0, JAM2},
    {NULL, &ubuf[2][0], 24, 3, 1, 0, JAM2}
};

// "Peak column"
WINTEXT ptexts[] =
{   // next text x y pen bg mode
    {&ptexts[1], &pbuf[0][0], 33, 1, 1, 0, JAM2},
    {&ptexts[2], &pbuf[1][0], 33, 2, 1, 0, JAM2},
    {NULL, &pbuf[2][0], 33, 3, 1, 0, JAM2}
};

// Whole window
struct Gadget drag_gadget = {
	.NextGadget = NULL,
	.Flags = GRELWIDTH | GRELHEIGHT | GADGHNONE,
	.Activation = GADGIMMEDIATE,
	.GadgetType = GTYP_WDRAGGING,

	// Rest is 0/NULL
	.LeftEdge = 0, .TopEdge = 0, .Width = 0, .Height = 0,
	.GadgetRender = NULL, .SelectRender = NULL,
	.GadgetText = NULL, .MutualExclude = 0,
	.SpecialInfo = NULL, .GadgetID = 0, .UserData = NULL
};

// Snapshot gadget
struct Gadget biggadget =	{
	.NextGadget = &drag_gadget,
	.Flags = GRELWIDTH | GADGHCOMP,
	.Activation = RELVERIFY,

	// Rest is 0/NULL
	.GadgetType = 0,
	.LeftEdge = 0, .TopEdge = 0, .Width = 0, .Height = 0,
	.GadgetRender = NULL, .SelectRender = NULL,
	.GadgetText = NULL, .MutualExclude = 0,
	.SpecialInfo = NULL, .GadgetID = 0, .UserData = NULL
};

#define LEFTEDGE 40		/* window coordinates */
#define TOPEDGE  20

struct NewWindow nw = {
    LEFTEDGE, TOPEDGE, 0, 0, -1, -1,	/* dimension, pens */
    NEWSIZE | MENUPICK | GADGETUP,	/* IDCMP flags */
    WFLG_SMART_REFRESH,		/* window flags */
    &biggadget, NULL,		/* gadgets, checkmark */
    NULL,			/* title */
    NULL, NULL,			/* screen, bitmap */
    0, 0, 0, 0,			/* extrema of dimensions */
    WBENCHSCREEN		/* screen to open onto */
};

struct TagItem wtags[] = {
    {WA_AutoAdjust, TRUE},
    {TAG_DONE}
};

struct GfxBase *GfxBase;	/* graphics pointer */
struct IntuitionBase *IntuitionBase;	/* intuition pointer */
struct Library *IconBase, *DiskfontBase, *GadToolsBase;

static struct Window *w;		/* screen pointer */
static struct Menu *menu;
static WINTEXTINFO wtinfo;

extern struct WBStartup *_WBenchMsg;

#define MEMSNAP_TIME	10L
#define MEMONLY_TIME	25L

/******************************************************************************/

/* prototypes for general routines */

static void CloseAll(void);
static BOOL long2str(LONG, char *, UWORD);

static BOOL OpenLibs()			/* open required libraries */
{
	if (
		(GfxBase = (struct GfxBase *) OpenLibrary((const STRPTR)"graphics.library", 0L)) &&
		(IntuitionBase = (struct IntuitionBase *) OpenLibrary((const STRPTR)"intuition.library", 37L)) &&
		(DiskfontBase = (struct Library *) OpenLibrary((const STRPTR)"diskfont.library", 36L)) &&
		(GadToolsBase = (struct Library *) OpenLibrary((const STRPTR)"gadtools.library", 37L)) &&
		(IconBase = (struct Library *) OpenLibrary((const STRPTR)"icon.library", 37L))
	) {
		return TRUE;
	}
	CloseAll();
	return FALSE;
}


static void CloseAll()			/* close opened libraries */
{
	if (menu) {
		ClearMenuStrip(w);
		FreeMemSnapMenu();
	}
	if (wtinfo.tf)
		CloseFont(wtinfo.tf);
	if (w)
		CloseWindow(w);
	if (IconBase)
		CloseLibrary(IconBase);
	if (DiskfontBase)
		CloseLibrary(DiskfontBase);
	if (GadToolsBase)
		CloseLibrary(GadToolsBase);
	if (GfxBase)
		CloseLibrary((struct Library*)GfxBase);
	if (IntuitionBase)
		CloseLibrary((struct Library*)IntuitionBase);
}


/* and this one is rather specific to this program... */
static BOOL long2str(LONG n, char *s, UWORD len)	/* long to string, right-adjusted */
{				/* will NOT null-terminate */
    /* len is space in buffer (excl. '\0') */
    /* also, prints nothin if zero */
    short sign;			/* minus sign required? */
    char *t = &s[len - 1];	/* get last space in string */

    if (n < 0)			/* get sign of n */
    {
	n = -n;
	sign = -1;
	len--;			/* reduce space (we'll need it for '-') */
    }
    else
	sign = 0;

    while (n && len)		/* work to do and space to do it */
    {
	*t = '0' + (n % 10);	/* get rightmost digit */
	t--;			/* mave back up string */
	len--;
	n /= 10;
    }

    if (sign)
	*t-- = '-';		/* put sign in now */

    while (len--)		/* fill remainder with spaces */
	*t-- = ' ';

    if (n)
	return FALSE;		/* failure */
    return TRUE;
}


/******************************************************************************/

/* Memory data management/manipulation routines */

static void obtainmem(LONG *), submem(LONG *, LONG *, LONG *), updatemem(LONG *, WINTEXT *);

#define CHIP 0
#define FAST 1
#define TOTAL 2

#define clearmem(mem) 		mem[CHIP] = mem[FAST] = mem[TOTAL] = 0L

static void obtainmem(LONG * mem)		/* store current memory */
{
    mem[TOTAL] = mem[CHIP] = AvailMem(MEMF_CHIP);
    mem[TOTAL] += (mem[FAST] = AvailMem(MEMF_FAST));
}


static void submem(LONG * to, LONG * from, LONG * howmuch)	/* to = from - howmuch */
{
    to[CHIP] = from[CHIP] - howmuch[CHIP];
    to[FAST] = from[FAST] - howmuch[FAST];
    to[TOTAL] = from[TOTAL] - howmuch[TOTAL];
}


static void updatemem(LONG * mem, WINTEXT * memtext)	/* update specified display */
{
    long2str(mem[CHIP], memtext[CHIP].text, 8);
    long2str(mem[FAST], memtext[FAST].text, 8);
    long2str(mem[TOTAL], memtext[TOTAL].text, 8);

    RenderWinTexts(&wtinfo, memtext);
}

static void updatepeak(LONG * mem, WINTEXT * memtext, LONG *peakvals)	/* update specified display */
{
		if(peakvals[CHIP] < mem[CHIP]) {
			peakvals[CHIP] = mem[CHIP];
    	long2str(mem[CHIP], memtext[CHIP].text, 8);
		}

		if(peakvals[FAST] < mem[FAST]) {
			peakvals[FAST] = mem[FAST];
    	long2str(mem[FAST], memtext[FAST].text, 8);
		}

		if(peakvals[TOTAL] < mem[TOTAL]) {
			peakvals[TOTAL] = mem[TOTAL];
    	long2str(mem[TOTAL], memtext[TOTAL].text, 8);
		}

    RenderWinTexts(&wtinfo, memtext);
}

static void clearpeak(LONG *peakvals) {
	peakvals[CHIP] = 0;
	peakvals[FAST] = 0;
	peakvals[TOTAL] = 0;
}

/* pop up a requester */
static void MsgBox(const char *str) {
    struct EasyStruct es;

    es.es_StructSize = sizeof(struct EasyStruct);

    es.es_Flags = 0L;
    es.es_Title = (UBYTE*)"MemSnap Message";
    es.es_TextFormat = (UBYTE*)str;
    es.es_GadgetFormat = (UBYTE*)"OK";
    EasyRequestArgs(NULL, &es, NULL, NULL);
}

/******************************************************************************/


int main(void)		/* provide a memory 'meter' */
{
	struct IntuiMessage *msg;				// Our window messages.
	LONG cmem[3], smem[3], umem[3]; // Storage of memory information.
	ULONG class;										// Message class.
	UWORD code;											// And code.
	WORD smallwidth, largewidth;		// Possible window sizes.
	BOOL small;											// Are we small?

	// Failure => under 1.3
	if (!OpenLibs()) {
		return EXIT_FAILURE;
	}

	GetOurIcon(_WBenchMsg);
	if (InitWinTextInfo(&wtinfo)) {
		// Size window to fit screen font
		nw.LeftEdge = TTInt("LEFTEDGE", LEFTEDGE);
		nw.TopEdge = TTInt("TOPEDGE", TOPEDGE);
		small = TTBool("SMALL", FALSE);
		FreeOurIcon(); // finished with it

		nw.Height = CHARS_DOWN * wtinfo.font_y + wtinfo.toffset + wtinfo.boffset;
		largewidth = CHARS_ACROSS * wtinfo.font_x + wtinfo.loffset + wtinfo.roffset;
		smallwidth = largewidth - REMOVECHARS * wtinfo.font_x;
		nw.Width = small ? smallwidth : largewidth;
		wtexts[0].text = small ? small_header : large_header;

		/* and set up big gadget */
		biggadget.LeftEdge = wtinfo.loffset;
		biggadget.TopEdge = wtinfo.toffset;
		biggadget.Width = -wtinfo.roffset - wtinfo.loffset;
		biggadget.Height = wtinfo.font_y;

		if (w = OpenWindowTagList(&nw, wtags)) {
			if (w->Width != nw.Width) {
				MsgBox("Window too small for text\nChoose a smaller font");
				CloseAll();
				return EXIT_FAILURE;
			}

			menu = AllocMemSnapMenu(w);
			if(menu != NULL) {
				SetMenuStrip(w, menu);
			}
			else {
				MsgBox("Couldn't create menus");
				CloseAll();
				return EXIT_FAILURE;
			}

			wtinfo.window = w;
			SetFont(w->RPort, wtinfo.tf);
			RenderWinTexts(&wtinfo, wtexts); /* draw initial texts */

			clearmem(smem); /* initialize memory display */

			// Main event loop
			for (;;) {
				while (msg = (struct IntuiMessage *)GetMsg(w->UserPort)) {
					class = msg->Class;
					code = msg->Code;
					ReplyMsg((struct Message *)msg);

					if (class == GADGETUP) {
						if (!small) {
							// New snapshot
							obtainmem(smem);
							updatemem(smem, stexts);
							clearpeak(peakvals);
						}
						else {
							// Naughty but so what?
							goto makelarge;
						}
					}
					else if (class == NEWSIZE) {
						RenderWinTexts(&wtinfo, wtexts); /* redraw texts */
						if (!small) {
							updatemem(smem, stexts);
						}
					}
					else if (class == MENUPICK) {
						switch (ITEMNUM(code)) {
						case SMALL:
							if (!small) {
								wtexts[0].text = small_header;
								SizeWindow(w, smallwidth - largewidth, 0);
								small = TRUE;
							}
							break;

						case LARGE:
						makelarge:
							// todo: move if nec.
							if (small) {
								WORD movex;
								wtexts[0].text = large_header;
								movex = w->WScreen->Width - w->LeftEdge - largewidth;
								if (movex < 0)
									MoveWindow(w, movex, 0);
								SizeWindow(w, largewidth - smallwidth, 0);
								small = FALSE;
							}
							break;

						case ABOUT:
							MsgBox(about_text);
							break;

						case QUIT:
							CloseAll();
							return EXIT_FAILURE;
						}
					}
				}

				// Do memory window stuff
				obtainmem(cmem);
				updatemem(cmem, ctexts);

				if (small) {
					Delay(MEMONLY_TIME);
				}
				else {
					submem(umem, smem, cmem);
					updatemem(umem, utexts);
					updatepeak(umem, ptexts, peakvals);

					Delay(MEMSNAP_TIME);
				}
			}
		}
		else {
			MsgBox("Can't open window");
		}
	}
	else {
		MsgBox("Can't open font");
	}

	FreeOurIcon();
	CloseAll();
	return EXIT_SUCCESS;
}
