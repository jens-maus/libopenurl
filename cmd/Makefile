#/***************************************************************************
#
# openurl.library - universal URL display and browser launcher library
# Copyright (C) 1998-2005 by Troels Walsted Hansen, et al.
# Copyright (C) 2005-2022 openurl.library Open Source Team
#
# This library is free software; it has been placed in the public domain
# and you can freely redistribute it and/or modify it. Please note, however,
# that some components may be under the LGPL or GPL license.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# openurl.library project: http://github.com/jens-maus/libopenurl
#
# $Id$
#
#***************************************************************************/

###########################################################################
# This makefile is a very generic one. It tries to identify both, the host
# and the target operating system for which YAM should be compiled.
# However, this auto-detection can be easily overridden by directly
# specifying an option on the commandline while calling 'make'.
#
# Example:
#
# # to explicitly compile for AmigaOS3
# > make OS=os3
#
# # to compile for AmigaOS4 but without debugging
# > make OS=os4 DEBUG=
#

#############################################
# find out the HOST operating system
# on which this makefile is run
HOST ?= $(shell uname)
ifeq ($(HOST), AmigaOS)
  ifeq ($(shell uname -m), powerpc)
    HOST = AmigaOS4
  endif
  ifeq ($(shell uname -m), ppc)
    HOST = AmigaOS4
  endif
endif

# if no host is identifed (no uname tool)
# we assume a AmigaOS build
ifeq ($(HOST),)
  HOST = AmigaOS
endif

#############################################
# now we find out the target OS for
# which we are going to compile YAM in case
# the caller didn't yet define OS himself
ifndef (OS)
  ifeq ($(HOST), AmigaOS4)
    OS = os4
  else
  ifeq ($(HOST), AmigaOS)
    OS = os3
  else
  ifeq ($(HOST), MorphOS)
    OS = mos
  else
  ifeq ($(HOST), AROS)
    # now we find out which CPU system aros will be used
    ifeq ($(shell uname -m), powerpc)
      OS = aros-ppc
    endif
	 ifeq ($(shell uname -m), ppc)
      OS = aros-ppc
    endif
	 ifeq ($(shell uname -m), i386)
      OS = aros-i386
    endif
	 ifeq ($(shell uname -m), i686)
      OS = aros-i686
    endif
	 ifeq ($(shell uname -m), x86_64)
      OS = aros-x86_64
    endif
  else
    OS = os4
  endif
  endif
  endif
  endif
endif

#############################################
# define common commands we use in this
# makefile. Please note that each of them
# might be overridden on the commandline.

# common commands
FLEX    = flex
FC      = flexcat
EXPR    = expr
DATE    = date
RM      = delete force
RMDIR   = delete force all
MKDIR   = makedir
CHMOD   = protect FLAGS=rwed
SED     = sed
CP      = copy
CC      = gcc
STRIP   = strip
OBJDUMP = objdump

# path definitions
CDUP  = /
CDTHIS=

# override some variables for non-native builds (cross-compiler)
ifneq ($(HOST), AmigaOS)
ifneq ($(HOST), AmigaOS4)
ifneq ($(HOST), MorphOS)

  # when we end up here this is either a unix or Aros host
  # so lets use unix kind of commands
  RM    = rm -f
  RMDIR = rm -rf
  MKDIR = mkdir -p
  CHMOD = chmod 755
  CP    = cp -f

  CDUP  = ../
  CDTHIS= ./

endif
endif
endif

###########################################################################
# CPU and DEBUG can be defined outside, defaults to above
# using e.g. "make DEBUG= CPU=-mcpu=603e" produces optimized non-debug
# PPC-603e version
#
# OPTFLAGS are disabled by DEBUG normally!
#
# ignored warnings are:
# none - because we want to compile with -Wall all the time

# Common Directories
PREFIX   = $(CDTHIS)
OBJDIR   = .obj_$(OS)
BINDIR   = bin_$(OS)
LOCALE   = ../locale
VPATH    = $(OBJDIR)
DEPFILE  = Makefile.dep

# target definition
TARGET   = $(BINDIR)/OpenURL

# Common compiler/linker flags
WARN     = -W -Wall -Wwrite-strings -Wpointer-arith -Wsign-compare
OPTFLAGS = -O3 -fomit-frame-pointer 
DEBUG    = -DDEBUG -O0
DEBUGSYM = -g -gstabs
CFLAGS   = -I. -I../include $(CPU) $(WARN) $(OPTFLAGS) $(DEBUG) $(DEBUGSYM) $(USER_CFLAGS)
LDFLAGS  = $(CPU) $(DEBUGSYM)
LDLIBS   =

# different options per target OS
ifeq ($(OS), os4)

  ##############################
  # AmigaOS4

  # Compiler/link/strip commands
  ifneq ($(HOST), AmigaOS4)
    CC      = ppc-amigaos-gcc
    STRIP   = ppc-amigaos-strip
    OBJDUMP = ppc-amigaos-objdump
  endif

  # Compiler/Linker flags
  CRT      = newlib
  CPU      = -mcpu=powerpc
  WARN     += -Wdeclaration-after-statement -Wdisabled-optimization
  REDEFINE =
  CFLAGS   += -mcrt=$(CRT) -D__USE_INLINE__ -D__NEW_TIMEVAL_DEFINITION_USED__ \
              $(REDEFINE) -Wa,-mregnames
  LDFLAGS  += -mcrt=$(CRT)

else
ifeq ($(OS), os3)

  ##############################
  # AmigaOS3

  # Compiler/link/strip commands
  ifneq ($(HOST), AmigaOS)
    CC      = m68k-amigaos-gcc
    STRIP   = m68k-amigaos-strip
    OBJDUMP = m68k-amigaos-objdump
  endif

  # Compiler/Linker flags
  CPU	    = -m68020-60 -msoft-float
  CFLAGS  += -mcrt=clib2 -DNO_INLINE_STDARG
  LDFLAGS += -mcrt=clib2
  LDLIBS  += -ldebug

else
ifeq ($(OS), mos)

  ##############################
  # MorphOS

  # Compiler/link/strip commands
  ifneq ($(HOST), MorphOS)
    CC      = ppc-morphos-gcc
    STRIP   = ppc-morphos-strip
    OBJDUMP = ppc-morphos-objdump
  endif

  # Compiler/Linker flags
  CPU     = -mcpu=powerpc
  CFLAGS  += -noixemul
  LDFLAGS += -noixemul
  LDLIBS  += -ldebug

else
ifeq ($(OS), aros-i386)

  ##############################
  # AROS (i386)

  ifneq ($(HOST), AROS)
    CC      = i386-aros-gcc
    STRIP   = i386-aros-strip
    OBJDUMP = i386-aros-objdump
  endif

  # Compiler/Linker flags
  OPTFLAGS = -O2 -fomit-frame-pointer 
  CFLAGS += -Wno-pointer-sign -DNO_INLINE_STDARG
  LDLIBS += -larossupport -lrom

else
ifeq ($(OS), aros-ppc)

  ##############################
  # AROS (PPC)

  ifneq ($(HOST), AROS)
    CC      = ppc-aros-gcc
    STRIP   = ppc-aros-strip
    OBJDUMP = ppc-aros-objdump
  endif

  # Compiler/Linker flags
  OPTFLAGS = -O2 -fomit-frame-pointer 
  CFLAGS += -Wno-pointer-sign -DNO_INLINE_STDARG
  LDLIBS += -larossupport -lrom

else
ifeq ($(OS), aros-x86_64)

  ##############################
  # AROS (x86_64)

  ifneq ($(HOST), AROS)
    CC      = x86_64-aros-gcc
    STRIP   = x86_64-aros-strip
    OBJDUMP = x86_64-aros-objdump
  endif

  # Compiler/Linker flags
  OPTFLAGS = -O2 -fomit-frame-pointer 
  CFLAGS += -Wno-pointer-sign -DNO_INLINE_STDARG
  LDLIBS += -larossupport -lrom

endif
endif
endif
endif
endif
endif

###########################################################################
# Here starts all stuff that is common for all target platforms and
# hosts.

OBJS += OpenURL.o \
        SmartReadArgs.o \
        debug.o

# main target
.PHONY: all
ifeq ($(wildcard $(DEPFILE)),$(DEPFILE))
# great, we have a dependecies file, let's make our target
all: $(BINDIR) $(OBJDIR) $(TARGET)
else
# no dependecies, create it and then call make again
all: depend
	@make --no-print-directory all
endif

# make the object directories
$(OBJDIR):
	@echo "  MK $@"
	@$(MKDIR) $(OBJDIR)

# make the object directories
$(BINDIR):
	@echo "  MK $@"
	@$(MKDIR) $(BINDIR)

# for compiling single .c files
$(OBJDIR)/%.o: %.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# for linking the target
$(TARGET): $(addprefix $(OBJDIR)/,$(OBJS))
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(addprefix $(OBJDIR)/,$(OBJS)) $(LDLIBS) -Wl,--cref,-M,-Map=$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug
	@$(CHMOD) $@

# for creating a .dump file
.PHONY: dump
dump:
	-$(OBJDUMP) --section-headers --all-headers --reloc --disassemble-all $(TARGET).debug > $(TARGET).dump

# cleanup target
.PHONY: clean
clean:
	-$(RM) $(TARGET) $(TARGET).debug $(TARGET).map $(addprefix $(OBJDIR)/,$(OBJS))

# clean all including .obj directory
.PHONY: cleanall
cleanall: clean
	-$(RMDIR) $(OBJDIR)

# clean all stuff, including our autotools
.PHONY: distclean
distclean: cleanall
	-$(RMDIR) $(BINDIR)

#install the newly built library to LIBS:
.PHONY: install
install: all
	-$(CP) $(TARGET) LIBS:

## DEPENDENCY GENERATION ##############

$(DEPFILE): ;
	@echo "WARNING: $(DEPFILE) missing. Please run 'make depend'"

## DEPENDENCY GENERATION ##############

.PHONY: depend
depend:
	@echo "  MK $(DEPFILE)"
	@echo "# AUTOGENERATED! DO NOT EDIT!!!" >$(DEPFILE)
	@$(CC) -MM -MG $(CFLAGS) $(wildcard *.c) >>$(DEPFILE)
	@echo "# AUTOGENERATED! DO NOT EDIT!!!" >>$(DEPFILE)
	@$(SED) -i 's,^\(.*\)\.o:,$$\(OBJDIR\)/\1.o:,g' $(DEPFILE)

# include dependencies file
-include $(DEPFILE)
