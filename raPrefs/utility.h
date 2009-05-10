/*
**
** Utility functions declaration
**
** $Id: utility.h,v 1.1 2008-09-02 23:24:26 abalabanb Exp $
**
*/

#ifndef UTILITY_H
#define UTILITY_H

#include <intuition/classusr.h>

int VARARGS68K RA_Request( Object * pWin, const char * strTitle, const char * strGadgets, const char * strFormat, ... );

void freeList( struct List * list );


#endif // UTILITY_H

