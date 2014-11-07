# $Id: makefile,v 1.3 2004/01/21 16:42:31 glass Exp $

all:
  @cd KLayer
  @$(MAKE) clean all
  @cd ..
  @cd VDD
  @$(MAKE) clean all
  @cd ..
  @cd Layout
  @$(MAKE) clean all
  @cd ..
  @$(MAKE) clean

clean:
  @cd KLayer
  @$(MAKE) clean
  @cd ..
  @cd VDD
  @$(MAKE) clean
  @cd ..
  @cd Layout
  @$(MAKE) clean
  @cd ..
