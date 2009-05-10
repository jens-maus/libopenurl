#
# OpenURL global makefile
#

all: os4 mos 68k

os4:
	@$(MAKE) -C library os4
	@$(MAKE) -C prefs os4
	@$(MAKE) -C Cmd os4

mos:
	@$(MAKE) -C library mos
	@$(MAKE) -C prefs mos
	@$(MAKE) -C Cmd mos

68k:
	@echo Building Amiga 68k version...
	@echo not supported for now, use the smakefile provided instead

revision:
	@echo Updating revisions...
	@$(MAKE) -C library revision
	@$(MAKE) -C prefs revision
	@$(MAKE) -C Cmd revision

