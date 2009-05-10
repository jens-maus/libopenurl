/*
**
** Utility functions
**
** $Id: utility.c,v 1.1 2008-09-02 23:24:26 abalabanb Exp $
**
*/

#include "utility.h"

#include <classes/requester.h>

#include <proto/exec.h>
#include <proto/requester.h>
#include <proto/intuition.h>
#include <proto/listbrowser.h>

#include <reaction/reaction_macros.h>

#include <stdarg.h> // for va_list handling


int VARARGS68K RA_Request( Object * pWin, const char * strTitle, const char * strGadgets, const char * strFormat, ... )
{
	int nResult = -1;
	va_list		valist;


	va_startlinear(valist, strFormat);

	Object * pReq = RequesterObject,
						REQ_Type,		REQTYPE_INFO,
						REQ_TitleText,	strTitle,
						REQ_BodyText,	strFormat,
						REQ_VarArgs,	va_getlinearva(valist,APTR),
						REQ_GadgetText,	strGadgets,
					EndObject;

	va_end(valist);

	if( pReq )
	{
		nResult = OpenRequester( pReq, pWin );

		IIntuition->DisposeObject( pReq );
		pReq = NULL;
	}

	return nResult;
}

// libération de la liste
void freeList( struct List * list )
{
    struct Node * node = NULL;

    while( node = IExec->RemHead(list) ) IListBrowser->FreeListBrowserNode( node );
}


