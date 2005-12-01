/*
**  openurl.library - universal URL display and browser
**  launcher library
**
**  Written by Troels Walsted Hansen <troels@thule.no>
**  Placed in the public domain.
**
**  Developed by:
**  - Alfonso Ranieri <alforan@tin.it>
**  - Stefan Kost <ensonic@sonicpulse.de>
*/


#include "lib.h"
#include <dos/dostags.h>

/***********************************************************************/

void
freeBase(void)
{
    if (lib_prefs)
    {
        URL_FreePrefsA(lib_prefs,NULL);
        lib_prefs = NULL;
    }

    if (RexxSysBase)
    {
        CloseLibrary((struct Library *)RexxSysBase);
        RexxSysBase = NULL;
    }

    if (IFFParseBase)
    {
        CloseLibrary(IFFParseBase);
        IFFParseBase = NULL;
    }

    if (UtilityBase)
    {
        CloseLibrary(UtilityBase);
        UtilityBase = NULL;
    }

    if (DOSBase)
    {
        CloseLibrary((struct Library *)DOSBase);
        DOSBase = NULL;
    }

    if (lib_pool)
    {
        DeletePool(lib_pool);
        lib_pool = NULL;
    }

    lib_flags &= ~BASEFLG_Init;
}

/***********************************************************************/

ULONG
initBase(void)
{
    if ((lib_pool = CreatePool(MEMF_PUBLIC|MEMF_CLEAR,16384,8192)) &&
        (DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",36)) &&
        (UtilityBase = OpenLibrary("utility.library",36)) &&
        (IFFParseBase = OpenLibrary("iffparse.library",36)) &&
        (RexxSysBase = (struct RxsLib *)OpenLibrary("rexxsyslib.library",33)) &&
        (lib_prefs = loadPrefsNotFail()))
    {
        lib_flags |= BASEFLG_Init;

        return TRUE;
    }

    freeBase();

    return FALSE;
}

/***********************************************************************/
