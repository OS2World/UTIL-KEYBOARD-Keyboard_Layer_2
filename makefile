# $Id$

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
