# $Id: makefile,v 1.8 2004/08/10 12:49:51 glass Exp $

CFLAGS=-Gm -Sp1 -Q+ -W3 -Wuse+ -O+ -Gl+ -I..\VDD
DFLAGS=-B"/OPTF /EXEP:2 /PM:PM /DLL /MAP:kl_engine.map"
EFLAGS=-B"/OPTF /EXEP:2 /PM:PM /MAP:kl_layer.map"

!ifdef DEBUG
CFLAGS=$(CFLAGS) -Ti+
!endif

all: ..\bin\layer3.dll \
     ..\bin\layer3.cat \
     ..\bin\layer3.exe

!include depends

..\bin\layer3.cat: layer_001.msg \
                   layer_007.msg
    gencat $@ $**

..\bin\layer3.dll: $(DLLLST) kl_engine.def
    $(CC) $(CFLAGS) $(DFLAGS) kl_engine.def -Fe$@ @<<
    $(DLLLST)
<<

kl_engine.obj: kl_engine.cpp
    icc $(CFLAGS) -Ge- /c $**

kl_layer.lib: ..\bin\layer3.dll kl_engine.def
    implib /nologo kl_layer.lib ..\bin\layer3.dll

..\bin\layer3.exe: $(OBJLST) kl_main.res kl_main.def
    $(CC) $(CFLAGS) $(EFLAGS) kl_main.def -Fe$@ @<<
    $(OBJLST)
<<
    rc -n kl_main.res $*.exe

kl_main.res: kl_main.rc kl_ids.h
    rc -n -r $*.rc $*.res

clean:
   @-erase *.obj 2> nul
   @-erase *.res 2> nul
   @-erase *.map 2> nul
   @-erase *.bak 2> nul
   @-erase *.lib 2> nul
