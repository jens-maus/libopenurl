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

#include "utility.h"

#include <classes/requester.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/requester.h>
#include <proto/intuition.h>
#include <proto/listbrowser.h>
//#include <proto/requester.h>

#include <reaction/reaction_macros.h>
#include "my_reaction_macros.h"

#include <stdarg.h> // for va_list handling


extern Class *RequesterClass;


int VARARGS68K RA_Request(Object * pWin, const char * strTitle, const char * strGadgets, const char * strFormat, ...)
{
	int nResult = -1;
	va_list valist;
	Object * pReq;

	va_startlinear(valist, strFormat);

	pReq = RequesterObject,
				REQ_Type,       REQTYPE_INFO,
				REQ_TitleText,  strTitle,
				REQ_BodyText,   strFormat,
				REQ_VarArgs,    va_getlinearva(valist,APTR),
				REQ_GadgetText, strGadgets,
	EndObject;

	va_end(valist);

	if(pReq)
	{
		nResult = OpenRequester(pReq, pWin);

		IIntuition->DisposeObject(pReq);
		pReq = NULL;
	}

	return nResult;
}

// libération de la liste
void freeList(struct List * list)
{
	struct Node *node;

	while((node = IExec->RemHead(list)) != NULL)
		IListBrowser->FreeListBrowserNode(node);
}

int32 CFGInteger(struct DiskObject *icon, CONST_STRPTR attr, int32 def)
{
	STRPTR val;

	val = IIcon->FindToolType(icon->do_ToolTypes, attr);
	if(val) IDOS->StrToLong(val, &def);

	return def;
}

BOOL CFGBoolean(struct DiskObject *icon, CONST_STRPTR attr, BOOL def)
{
	STRPTR val;

	val = IIcon->FindToolType(icon->do_ToolTypes, attr);
	if(!val) return def;

	if( IIcon->MatchToolValue(val,"NO")  ||
	    IIcon->MatchToolValue(val,"OFF") ||
	    IIcon->MatchToolValue(val,"FALSE")
	  ) return FALSE;

	if( IIcon->MatchToolValue(val,"YES") ||
	    IIcon->MatchToolValue(val,"ON")  ||
	    IIcon->MatchToolValue(val,"TRUE")
	  ) return TRUE;

	return def;
}
