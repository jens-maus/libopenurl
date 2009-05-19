#
# OpenURL global makefile
#

all: os4 mos os3

os4:
	@$(MAKE) -C library OS=os4
	@$(MAKE) -C prefs OS=os4
	@$(MAKE) -C Cmd OS=os4

mos:
	@$(MAKE) -C library OS=mos
	@$(MAKE) -C prefs OS=mos
	@$(MAKE) -C Cmd OS=mos

os3:
	@$(MAKE) -C library OS=os3
	@$(MAKE) -C prefs OS=os3
	@$(MAKE) -C Cmd OS=os3
