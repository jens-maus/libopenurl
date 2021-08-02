/***************************************************************************

 openurl.library - universal URL display and browser launcher library
 Copyright (C) 1998-2005 by Troels Walsted Hansen, et al.
 Copyright (C) 2005-2021 openurl.library Open Source Team

 This library is free software; it has been placed in the public domain
 and you can freely redistribute it and/or modify it. Please note, however,
 that some components may be under the LGPL or GPL license.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 openurl.library project: http://github.com/jens-maus/libopenurl

 $Id$

***************************************************************************/

#ifndef HANDLERS_H
#define HANDLERS_H


#include <exec/types.h>
#include <libraries/gadtools.h>

BOOL HandleInput_Main_Win(void);
void HandleInput_Edit_Brow_Win(void);
void HandleInput_Edit_Mail_Win(void);
void HandleInput_Edit_FTP_Win(void);

#ifdef MENUCLASS
	#include <intuition/menuclass.h>
	#include <intuition/classes.h>
	extern Object *menustripobj;
#else
	extern struct Menu *MenuStrip;
#endif
ULONG HandleMenu(uint16 selection);


#endif //HANDLERS_H
