/***************************************************************************

 openurl.library - universal URL display and browser launcher library
 Copyright (C) 1998-2005 by Troels Walsted Hansen, et al.
 Copyright (C) 2005-2022 openurl.library Open Source Team

 This library is free software; it has been placed in the public domain
 and you can freely redistribute it and/or modify it. Please note, however,
 that some components may be under the LGPL or GPL license.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 openurl.library project: http://github.com/jens-maus/libopenurl

 $Id$

***************************************************************************/

#ifndef UTILITY_H
#define UTILITY_H


#include <intuition/classusr.h>
#include <workbench/workbench.h>

int VARARGS68K RA_Request( Object * pWin, const char * strTitle, const char * strGadgets, const char * strFormat, ... );
void freeList( struct List * list );
int32 CFGInteger(struct DiskObject *, CONST_STRPTR attr, int32 def);

#define iget(obj, attr)         ({uint32 b=0; IIntuition->GetAttr(attr, (obj), &b); b;})
#define gadset(gad, win, ...)   IIntuition->SetGadgetAttrs((gad), (win), NULL, __VA_ARGS__, TAG_DONE)
#define iset(obj, ...)          IIntuition->SetAttrs((obj), __VA_ARGS__, TAG_DONE)


#endif // UTILITY_H
