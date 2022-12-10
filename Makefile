CFLAGS        = -O -fomit-frame-pointer -fstrength-reduce

DEST          =

HDRS          = mkmf.h

LDFLAGS       = -lfloatfnc -ldos

LIBS          = 

LINKER        = gcc

CC	      = gcc

MAKEFILE      = makefile

OBJS          = file.o argument.o dir.o mkmf.o suffix.o include.o mlib.o

PRINT         = print

PROGRAM       = mkmf.x

SRCS          = file.c argument.c dir.c mkmf.c suffix.c include.c mlib.c

.c.o:
		@$(CC) -c $(CFLAGS) $*.c

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(LIBS)
		@echo Loading $(PROGRAM) ...
		@$(LINKER) $(LDFLAGS) $(OBJS) -o $(PROGRAM) $(LIBS)
		@echo done

clean:;		@rm -f $(OBJS)

depend:;	@mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@cp $(PROGRAM) $(DEST)

print:;		$(PRINT) $(HDRS) $(SRCS)

program:	$(PROGRAM)

tags:		$(HDRS) $(SRCS)
		@ctags $(HDRS) $(SRCS) >tags

$(DEST)\$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) 
		@make -f $(MAKEFILE) install
###
file.o: mkmf.h
argument.o: mkmf.h
mkmf.o: mkmf.h
suffix.o: mkmf.h
include.o: mkmf.h
mlib.o: mkmf.h
