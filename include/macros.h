#ifndef _MACROS_H
#define _MACROS_H

/****************************************************************************/

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

#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#define MIN(a,b) ((a<b) ? (a) : (b))
#define MAX(a,b) ((a>b) ? (a) : (b))
#define ABS(a) (((a)>0) ? (a) : -(a))
#define BOOLSAME(a,b) (((a) ? TRUE : FALSE)==((b) ? TRUE : FALSE))

#if !defined(__amigaos4__) && !defined(__AROS__)
#define SYS_Error TAG_IGNORE
#endif

#if defined(__AROS__)
#define MAXRMARG  15 /* maximum arguments */
#endif

// xget()
// Gets an attribute value from a MUI object
//ULONG xget(Object *obj, const IPTR attr);
#if defined(__GNUC__)
  // please note that we do not evaluate the return value of GetAttr()
  // as some attributes (e.g. MUIA_Selected) always return FALSE, even
  // when they are supported by the object. But setting b=0 right before
  // the GetAttr() should catch the case when attr doesn't exist at all
  #define xget(OBJ, ATTR) ({IPTR b=0; GetAttr(ATTR, OBJ, &b); b;})
#endif

/****************************************************************************/

#endif /* _MACROS_H */
