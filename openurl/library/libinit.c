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
#include "openurl.library_rev.h"

/****************************************************************************/

UBYTE                  lib_name[] = PRG;
UBYTE                  lib_ver[] = VSTRING;
UBYTE                  lib_fullName[] = PRGNAME;
ULONG                  lib_version = VERSION;
ULONG                  lib_revision = REVISION;

struct ExecBase        *SysBase = NULL;
struct DosLibrary      *DOSBase = NULL;
struct Library         *UtilityBase = NULL;
struct Library         *IFFParseBase = NULL;
struct RxsLib          *RexxSysBase = NULL;

struct SignalSemaphore lib_sem = {0};
struct SignalSemaphore lib_prefsSem = {0};
struct SignalSemaphore lib_memSem = {0};

APTR                   lib_pool = NULL;
struct URL_Prefs       *lib_prefs = NULL;

struct Library         *lib_base = NULL;
ULONG                  lib_segList = NULL;
ULONG                  lib_use = 0;
ULONG                  lib_flags = 0;

/****************************************************************************/

#ifdef __MORPHOS__
#include <exec/resident.h>

static struct Library *initLib ( struct Library *base , BPTR segList , struct ExecBase *sys );
static struct Library *openLib ( void );
static ULONG expungeLib ( void );
static ULONG closeLib ( void );
static ULONG nil (void );
static ULONG first(void) __attribute((unused));

/****************************************************************************/

static ULONG
first(void)
{
    return -1;
}

/****************************************************************************/

static const APTR funcTable[] =
{
    (APTR)FUNCARRAY_BEGIN,
    (APTR)FUNCARRAY_32BIT_NATIVE,
    (APTR)openLib,
    (APTR)closeLib,
    (APTR)expungeLib,
    (APTR)nil,

    (APTR)LIB_URL_OpenA,
    (APTR)LIB_URL_OldGetPrefs,
    (APTR)LIB_URL_OldFreePrefs,
    (APTR)LIB_URL_OldSetPrefs,
    (APTR)LIB_URL_OldGetDefaultPrefs,
    (APTR)LIB_URL_OldLaunchPrefsApp,
    (APTR)0xffffffff,

    (APTR)FUNCARRAY_32BIT_D0D1A0A1SR_NATIVE,
    (APTR)dispatch,
    (APTR)0xffffffff,

    (APTR)FUNCARRAY_32BIT_NATIVE,
    (APTR)LIB_URL_GetPrefsA,
    (APTR)LIB_URL_FreePrefsA,
    (APTR)LIB_URL_SetPrefsA,
    (APTR)LIB_URL_LaunchPrefsAppA,
    (APTR)LIB_URL_GetAttr,
    (APTR)0xffffffff,

    (APTR)FUNCARRAY_END
};

static const ULONG initTable[] =
{
    sizeof(struct Library),
    (ULONG)funcTable,
    NULL,
    (ULONG)initLib
};

const struct Resident romTag =
{
    RTC_MATCHWORD,
    (struct Resident *)&romTag,
    (struct Resident *)&romTag+1,
    RTF_AUTOINIT|RTF_PPC|RTF_EXTENDED,
    VERSION,
    NT_LIBRARY,
    0,
    (UBYTE *)lib_name,
    (UBYTE *)lib_ver,
    (APTR)initTable,
    REVISION,
    NULL
};

const ULONG __abox__ = 1;
#endif

/****************************************************************************/

#ifdef __MORPHOS__
static struct Library *initLib(struct Library *base,BPTR segList,struct ExecBase *sys)
#else
struct Library *SAVEDS ASM initLib(REG(a0,ULONG segList),REG(a6,struct ExecBase *sys),REG(d0, struct Library *base))
#endif
{
#define SysBase sys
    InitSemaphore(&lib_sem);
    InitSemaphore(&lib_prefsSem);
    InitSemaphore(&lib_memSem);
#undef SysBase

    SysBase     = sys;
    lib_segList = segList;

    return lib_base = base;
}

/****************************************************************************/

#ifdef __MORPHOS__
static struct Library *openLib(void)
{
    struct Library *base = (struct Library *)REG_A6;
#else
struct Library * SAVEDS ASM openLib(REG(a6,struct Library *base))
{
#endif

    struct Library *res;

    ObtainSemaphore(&lib_sem);

    base->lib_OpenCnt++;
    base->lib_Flags &= ~LIBF_DELEXP;

    if (!(lib_flags & BASEFLG_Init) && !initBase())
    {
        base->lib_OpenCnt--;
        res = NULL;
    }
    else res = base;

    ReleaseSemaphore(&lib_sem);

    return res;
}

/****************************************************************************/

#ifdef __MORPHOS__
static ULONG expungeLib(void)
{
    struct Library *base = (struct Library *)REG_A6;
#else
ULONG SAVEDS ASM expungeLib(REG(a6,struct Library *base))
{
#endif

    ULONG res;

    ObtainSemaphore(&lib_sem);

    if (!(base->lib_OpenCnt || lib_use))
    {
        Remove((struct Node *)base);
        FreeMem((UBYTE *)base-base->lib_NegSize,base->lib_NegSize+base->lib_PosSize);

        res = lib_segList;
    }
    else
    {
        base->lib_Flags |= LIBF_DELEXP;
        res = NULL;
    }

    ReleaseSemaphore(&lib_sem);

    return res;
}

/****************************************************************************/

#ifdef __MORPHOS__
static ULONG closeLib(void)
{
    struct Library *base = (struct Library *)REG_A6;
#else
ULONG SAVEDS ASM closeLib(REG(a6,struct Library *base))
{
#endif

    ULONG res = NULL;

    ObtainSemaphore(&lib_sem);

    if (!(--base->lib_OpenCnt || lib_use))
    {
        freeBase();

        if (base->lib_Flags & LIBF_DELEXP)
        {
            Remove((struct Node *)base);
            FreeMem((UBYTE *)base-base->lib_NegSize,base->lib_NegSize+base->lib_PosSize);

            res = lib_segList;
        }
    }

    ReleaseSemaphore(&lib_sem);

    return res;
}

/****************************************************************************/

#ifdef __MORPHOS__
static ULONG nil(void)
{
    return 0;
}
#endif

/****************************************************************************/

