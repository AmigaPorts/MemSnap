#ifndef _MEMSNAP_ICON_H_
#define _MEMSNAP_ICON_H_

#include <workbench/startup.h>

BOOL GetOurIcon(struct WBStartup *WBenchMsg);

void FreeOurIcon(void);

char *TTString(char *name, char *def);

LONG TTInt(char *name, LONG def);

BOOL TTBool(char *name, BOOL def);

#endif // _MEMSNAP_ICON_H_
