/***************************************************************************

 openurl.library - universal URL display and browser launcher library
 Copyright (C) 1998-2005 by Troels Walsted Hansen, et al.
 Copyright (C) 2005-2009 by openurl.library Open Source Team

 This library is free software; it has been placed in the public domain
 and you can freely redistribute it and/or modify it. Please note, however,
 that some components may be under the LGPL or GPL license.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 openurl.library project: http://sourceforge.net/projects/openurllib/

 $Id: macros.h 56 2009-05-18 07:28:47Z damato $

***************************************************************************/

#ifndef _MACROS_H
#define _MACROS_H

/****************************************************************************/
/*
** Some useful macros
*/

#include "SDI_compiler.h"

#if defined(__amigaos4__)
#define GETINTERFACE(iface, base)	(iface = (APTR)GetInterface((struct Library *)(base), "main", 1L, NULL))
#define DROPINTERFACE(iface)			(DropInterface((struct Interface *)iface), iface = NULL)
#else
#define GETINTERFACE(iface, base)	TRUE
#define DROPINTERFACE(iface)
#endif

// special flagging macros
#define isFlagSet(v,f)      (((v) & (f)) == (f))  // return TRUE if the flag is set
#define hasFlag(v,f)        (((v) & (f)) != 0)    // return TRUE if one of the flags in f is set in v
#define isFlagClear(v,f)    (((v) & (f)) == 0)    // return TRUE if flag f is not set in v
#define SET_FLAG(v,f)       ((v) |= (f))          // set the flag f in v
#define CLEAR_FLAG(v,f)     ((v) &= ~(f))         // clear the flag f in v
#define MASK_FLAG(v,f)      ((v) &= (f))          // mask the variable v with flag f bitwise

// transforms a define into a string
#define STR(x)  STR2(x)
#define STR2(x) #x

#define INITPORT(p,s) (((struct MsgPort *)(p))->mp_Flags = PA_SIGNAL, \
                       ((struct MsgPort *)(p))->mp_SigBit = (UBYTE)(s), \
                       ((struct MsgPort *)(p))->mp_SigTask = FindTask(NULL), \
                       NewList(&(((struct MsgPort *)(p))->mp_MsgList)))

#define INITMESSAGE(m,p,l) (((struct Message *)(m))->mn_Node.ln_Type = NT_MESSAGE, \
                            ((struct Message *)(m))->mn_ReplyPort = ((struct MsgPort *)(p)), \
                            ((struct Message *)(m))->mn_Length = ((UWORD)l))

#if !defined(__amigaos4__) && !defined(__AROS__)
#define SYS_Error TAG_IGNORE
#endif

#if defined(__AROS__)
#define MAXRMARG  15 /* maximum arguments */
#endif

#define BOOLSAME(a,b) (((a) ? TRUE : FALSE)==((b) ? TRUE : FALSE))

// xget()
// Gets an attribute value from a MUI object
ULONG xget(Object *obj, const IPTR attr);
#if defined(__GNUC__)
  // please note that we do not evaluate the return value of GetAttr()
  // as some attributes (e.g. MUIA_Selected) always return FALSE, even
  // when they are supported by the object. But setting b=0 right before
  // the GetAttr() should catch the case when attr doesn't exist at all
  #define xget(OBJ, ATTR) ({IPTR b=0; GetAttr(ATTR, OBJ, &b); b;})
#endif

/****************************************************************************/

#endif /* _MACROS_H */
