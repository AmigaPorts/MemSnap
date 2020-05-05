# MemSnap

A small memory monitor utility, resurrected from Aminet and improved. Original package is available on Aminet: http://aminet.net/package/util/moni/memsnap2

## Changelog

### MemSnap version 1.2 (2020.05.05)

- First AmigaPorts release
- Added peak memory usage column
- Added CMake build system
- Reformatted readme to Markdown

### Latest Aminet version (1993.02.10)

Updates to MemSnap 1.1:

- Now uses small window, where dragbar is whole window except snap gadget;
- Has two states, small and large; menu added to choose states and quit;
- Tooltype: `SMALL=<what>` where `what` is `YES` or `NO`, determines initial state.
- When memsnap is small, clicking in snapshot gadget makes it large.

MemSnapII is really neat when started small with a small font, as a small
memory monitor. The small font tracers_bold is included for use.

### MemSnap version 1.1 (1993.02)

Now defaults to using topaz font. Font may be specified by tooltypes:

``` plain
FONTNAME=<name>   e.g. FONTNAME=courier.font
FONTHEIGHT=<size> e.g. FONTHEIGHT=15
```

(Note: font will be scaled to size if given size doesn't exist).
Can use tooltypes also to specify where window opens:

``` plain
LEFTEDGE=<num>
TOPEDGE=<num>
```

You can use Lens or something to determine preferred position of
window to set in tooltypes.

All these changes can only be used if starting from Workbench; so, if
starting MemSnap at bootup, shove it into WBStartup.

### MemSnap version 1.0 (1992.03)

Nothing new here, this is based upon a program called Memeter that
I often used under 1.3, but that program crashed under my ZKicked 2.0,
so I've written this to fill the gap. This only runs under 2.0.

It's use (apart from telling you free memory) is to indicate what
memory a program uses, and whether it frees all its memory.

It can be started from Workbench or CLI, and takes no arguments.
It opens up a window with three columns, showing chip, fast and total
memory.

Under "Current" is the current amount of free memory.
Under "Snapshot" is memory available at time of snapshot.
Under "Used" is memory used since snapshot, ie. snapshot - used.

To snapshot memory, click anywhere on the line containing the
column headings.

Quick example:

- Start memsnap.
- Open drawer containing clock and snapshot current memory.
- Start Clock program. On my system it uses about 43K
- Close down clock program.
- See if "used" column is empty ( => nothing used since snapshot).

Other features: (first two defunct in MemSnapII)

- Iconifies with zoom gadget.
- Uses screen font as set in preferences (and so hopefully copes
  with all fonts). Proportional fonts may or may not look right.
- Has a version string (wow!).
- It's small, FREE, and comes with it's C source.

## Original author

Original MemSnap code by Martin W. Scott.

Due to GDPR, the original postal address from Aminet package has been redacted.
The original message from author is found below.

> Public Domain by Martin W. Scott
> No warranties attached - use at your own risk...
>
> This is my first attempt at OS2-friendly programming. Thanks to
> author of the original memeter, WallyWare(!?).
>
> Enjoy.
