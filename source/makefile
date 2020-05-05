CFLAGS = -cisq -rr -v -Hinclude:intui.sym -Hinclude:pragmas.sym -O
OBJS = memsnap.o wintext.o icon.o menu.o

memsnap: $(OBJS) wintext.h
	blink FROM lib:c.o $(OBJS) TO memsnap LIB lib:lcr.lib SC SD ND

