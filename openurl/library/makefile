#
# OpenURL.library makefile
#

VER=7

all: os4 mos 68k

os4:
	@printf '\033[1m  => Building Amiga OS4 library...\033[0m\n'
	@$(MAKE) -f makefile-os4

mos:
	@printf '\033[1m  => Building MorphOS library...\033[0m\n'
	@$(MAKE) -f makefile-mos

68k:
	@printf '\033[1m  => Building Amiga 68k library...\033[0m\n'
	@echo not supported for now, use the smakefile provided instead

revision:
	@bumprev $(VER) openurl.library
