# $Id$

CFLAGS=/Sp1 /Q+ /W3 /Wuse+ /O+ /Oc+ /Gl+ /Gs+ /I../KLayer
LFLAGS=/B"/OPTF /EXEP:2"

{../Klayer}.cpp.obj:
    icc /c $(CFLAGS) /Fo$@ $<

.cpp.obj:
    icc /c $(CFLAGS) /Fo$@ $<

.c.obj:
    icc /c $(CFLAGS) /Fo$@ $<

all: ..\bin\layout.exe

!include depends

..\bin\layout.exe: $(OBJLST)
   $(CC) $(CFLAGS) $(LFLAGS) $** /Fe$@

clean:
   @-erase *.o* 2> nul

