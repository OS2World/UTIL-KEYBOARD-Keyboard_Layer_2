# $Id: makefile,v 1.7 2004/07/21 06:49:55 glass Exp $

CFLAGS=/Sm /Sp1 /Gv /Rn /Gr /Gn /O /Gs /Ss /Tdc /W1 /Wuse+ /I../KLayer /Q /DM_I386 /DCYRILLIC
LFLAGS=/B"/NOI /NOE /NOD /NOL /EXEP:2 /VDD /MAP"
LLIBLS=vdh.lib

.c.obj:
    icc /c $(CFLAGS) /Fo$@ $<

{..\KLayer}.cpp.obj:
    icc /c $(CFLAGS) /Fo$@ $<

all: ..\bin\layer3.sys

!include depends

..\bin\layer3.sys: $(OBJLST) kl_vdd.def
    $(CC) $(CFLAGS) $(LFLAGS) $(LLIBLS) kl_vdd.def $(OBJLST) /Fe$@

clean:
   @-erase *.o*  2> nul
   @-erase *.map 2> nul
