#/***************************************************************************
#
# openurl.library - universal URL display and browser launcher library
# Copyright (C) 1998-2005 by Troels Walsted Hansen, et al.
# Copyright (C) 2005-2009 by openurl.library Open Source Team
#
# This library is free software; it has been placed in the public domain
# and you can freely redistribute it and/or modify it. Please note, however,
# that some components may be under the LGPL or GPL license.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# openurl.library project: http://sourceforge.net/projects/openurllib/
#
# $Id$
#
#***************************************************************************/

#
# OpenURL global makefile
#

all: os4 mos os3

os4:
	@$(MAKE) -C library OS=os4
	@$(MAKE) -C prefs OS=os4
	@$(MAKE) -C raPrefs OS=os4
	@$(MAKE) -C cmd OS=os4

mos:
	@$(MAKE) -C library OS=mos
	@$(MAKE) -C prefs OS=mos
	@$(MAKE) -C cmd OS=mos

os3:
	@$(MAKE) -C library OS=os3
	@$(MAKE) -C prefs OS=os3
	@$(MAKE) -C cmd OS=os3
