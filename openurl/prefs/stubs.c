/*
**  OpenURL - MUI preferences for openurl.library
**
**  Written by Troels Walsted Hansen <troels@thule.no>
**  Placed in the public domain.
**
**  Developed by:
**  - Alfonso Ranieri <alforan@tin.it>
**  - Stefan Kost <ensonic@sonicpulse.de>
*/


#include "OpenURL.h"

/***********************************************************************/

APTR
NewObject(struct IClass *classPtr,UBYTE *classID,... )
{
    APTR    res;
    va_list va;

    va_start(va,classID);
    res = NewObjectA(classPtr,classID,(struct TagItem *)va->overflow_arg_area);
    va_end(va);

    return res;
}

/***********************************************************************/

APTR
MUI_NewObject(UBYTE *classID,... )
{
    APTR    res;
    va_list va;

    va_start(va,classID);
    res = MUI_NewObjectA(classID,(struct TagItem *)va->overflow_arg_area);
    va_end(va);

    return res;
}

/***********************************************************************/

APTR
DoSuperNew(struct IClass *cl,Object *obj,...)
{
    APTR    res;
    va_list va;

    va_start(va,obj);
    res = (APTR)DoSuperMethod(cl,obj,OM_NEW,(ULONG)va->overflow_arg_area,NULL);
    va_end(va);

    return res;
}

/***********************************************************************/

