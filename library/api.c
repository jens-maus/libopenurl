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

 $Id$

***************************************************************************/

#include "lib.h"

#include <dos/dostags.h>
#include <exec/execbase.h>

#include "version.h"
#include "debug.h"

#include <stdio.h>

/**************************************************************************/

ULONG LIBFUNC
URL_OpenA(REG(a0,STRPTR URL), REG(a1,struct TagItem *attrs))
{
    struct List portList;
    TEXT       buf[256];
    STRPTR     pubScreenName = NULL, fullURL = NULL;
    ULONG       res, show, toFront, newWindow, launch, httpPrepend = FALSE;

    NewList(&portList);

    ObtainSemaphore(&OpenURLBase->prefsSem);

    /* parse arguments */
    pubScreenName = (STRPTR)GetTagData(URL_PubScreenName,(ULONG)"Workbench",attrs);
    show          = GetTagData(URL_Show, OpenURLBase->prefs->up_DefShow,attrs);
    toFront       = GetTagData(URL_BringToFront, OpenURLBase->prefs->up_DefBringToFront,attrs);
    newWindow     = GetTagData(URL_NewWindow, OpenURLBase->prefs->up_DefNewWindow,attrs);
    launch        = GetTagData(URL_Launch, OpenURLBase->prefs->up_DefLaunch,attrs);

    /* make a copy of the global list of named ports */
    Forbid();
    res = copyList(&portList, &((struct ExecBase *)SysBase)->PortList,sizeof(struct Node));
    Permit();

    if(!res)
      goto done;

    /* prepend "http://" if URL has no method */
    if(OpenURLBase->prefs->up_Flags & UPF_PREPENDHTTP)
    {
        TEXT *colon;

        colon = strchr((STRPTR)URL,':');

        if (!colon) httpPrepend = TRUE;
        else
        {
            TEXT *p;

            for (p = (STRPTR)URL; p<colon; p++)
            {
                if (!isalnum(*p) && (*p!='+') && (*p!='-'))
                {
                    httpPrepend = TRUE;
                    break;
                }
            }
        }
    }

    if (httpPrepend)
    {
        ULONG len = strlen((STRPTR)URL)+8;

        if (len>sizeof(buf))
        {
            if(!(fullURL = allocArbitrateVecPooled(strlen((STRPTR)URL)+8)))
              goto done;
        }
        else fullURL = buf;

        snprintf(fullURL, len, "http://%s", URL);
    }
    else
    	fullURL = URL;

    /* Be case insensitive - Piru */
    if((OpenURLBase->prefs->up_Flags & UPF_DOMAILTO) && !Strnicmp((STRPTR)URL,"mailto:",7))
        res = sendToMailer(fullURL,&portList,show,toFront,launch,pubScreenName);
    else
        if ((OpenURLBase->prefs->up_Flags & UPF_DOFTP) && !Strnicmp((STRPTR)URL,"ftp://",6))
            res = sendToFTP(fullURL,&portList,show,toFront,newWindow,launch,pubScreenName);
        else res = sendToBrowser(fullURL,&portList,show,toFront,newWindow,launch,pubScreenName);

done:
    ReleaseSemaphore(&OpenURLBase->prefsSem);
    freeList(&portList);
    if(httpPrepend && fullURL && fullURL!=buf)
      freeArbitrateVecPooled(fullURL);

    return res;
}

/**************************************************************************/

struct URL_Prefs * LIBFUNC
URL_GetPrefsA(REG(a0,struct TagItem *attrs))
{
    struct URL_Prefs *p;
    ULONG            mode;

    mode = GetTagData(URL_GetPrefs_Mode,URL_GetPrefs_Mode_Env,attrs);

    if (mode==URL_GetPrefs_Mode_Default)
    {
        if(!(p = allocArbitrateVecPooled(sizeof(struct URL_Prefs))))
          return NULL;

        setDefaultPrefs(p);

        return p;
    }

    if ((mode==URL_GetPrefs_Mode_Env) || (mode==URL_GetPrefs_Mode_Envarc))
    {
        mode = (mode==URL_GetPrefs_Mode_Env) ? LOADPREFS_ENV : LOADPREFS_ENVARC;

        if (!(p = allocArbitrateVecPooled(sizeof(struct URL_Prefs))))
            return NULL;

        if (loadPrefs(p,mode))
            return p;

        if (GetTagData(URL_GetPrefs_FallBack,TRUE,attrs))
        {

            if ((mode==LOADPREFS_ENV) && loadPrefs(p,LOADPREFS_ENVARC))
                return p;

            setDefaultPrefs(p);

            return p;
        }

        URL_FreePrefsA(p,NULL);

        return NULL;
    }

    ObtainSemaphoreShared(&OpenURLBase->prefsSem);
    p = copyPrefs(OpenURLBase->prefs);
    ReleaseSemaphore(&OpenURLBase->prefsSem);

    return p;
}

/**************************************************************************/

struct URL_Prefs * LIBFUNC
URL_OldGetPrefs(void)
{
  return URL_GetPrefsA(NULL);
}

/**************************************************************************/

void LIBFUNC
URL_FreePrefsA(REG(a0,struct URL_Prefs *p),UNUSED REG(a1,struct TagItem *attrs))
{
    if (p)
    {
        freeList((struct List *)&p->up_BrowserList);
        freeList((struct List *)&p->up_MailerList);
        freeList((struct List *)&p->up_FTPList);
        freeArbitrateVecPooled(p);
    }
}

/**************************************************************************/

void LIBFUNC
URL_OldFreePrefs(REG(a0,struct URL_Prefs *p))
{
  URL_FreePrefsA(p,NULL);
}

/**************************************************************************/

ULONG LIBFUNC
URL_SetPrefsA(REG(a0,struct URL_Prefs *p),REG(a1,struct TagItem *attrs))
{
    ULONG res = FALSE;

    if (p->up_Version==PREFS_VERSION)
    {
        struct URL_Prefs *newp;

        ObtainSemaphore(&OpenURLBase->prefsSem);

        if((newp = copyPrefs(p)))
        {
            newp->up_Version = PREFS_VERSION;
            newp->up_Flags &= ~UPF_ISDEFAULTS;

            URL_FreePrefsA(OpenURLBase->prefs,NULL);
            OpenURLBase->prefs = newp;

            if((res = savePrefs((STRPTR)DEF_ENV, OpenURLBase->prefs)))
            {
                if (GetTagData(URL_SetPrefs_Save,FALSE,attrs))
                {
                    res = savePrefs((STRPTR)DEF_ENVARC, OpenURLBase->prefs);
                }
            }
        }

        ReleaseSemaphore(&OpenURLBase->prefsSem);
    }

    return res;
}

/**************************************************************************/

ULONG LIBFUNC
URL_OldSetPrefs(REG(a0,struct URL_Prefs *p),REG(d0,ULONG save))
{
    struct TagItem stags[] = { { URL_SetPrefs_Save, 0         },
                               { TAG_DONE,          TAG_DONE  } };

    stags[0].ti_Data = save;

    return URL_SetPrefsA(p,stags);
}

/**************************************************************************/

struct URL_Prefs * LIBFUNC
URL_OldGetDefaultPrefs(void)
{
    struct TagItem gtags[] = { { URL_GetPrefs_Mode, URL_GetPrefs_Mode_Default },
                               { TAG_DONE,          TAG_DONE                  } };

    return URL_GetPrefsA(gtags);
}

/**************************************************************************/

ULONG LIBFUNC
URL_LaunchPrefsAppA(REG(a0,UNUSED struct TagItem *attrs))
{
    BPTR in;

    if((in  = Open("NIL:",MODE_OLDFILE)))
    {
        BPTR out;

        if((out = Open("NIL:",MODE_OLDFILE)))
        {
            TEXT name[256] = "";

            LONG len = GetVar("AppPaths/OpenURL",name+1,sizeof(name)-1,GVF_GLOBAL_ONLY);
            if (len<=0)
            {
                // Ok let's try to be backward compatible
                if(GetVar("OpenURL_Prefs_Path",name,sizeof(name),GVF_GLOBAL_ONLY)<=0)
                {
                    strcpy(name,"\"Sys:Prefs/Open URL\"");
                }
            }
            else
            {
                name[0]='\"';
                strcpy(name+1+len,"/Open URL\"");
                name[len+11]='\0';
            }

            #if defined(__amigaos4__)
            #define NP_STACKSIZE 48000
            #else
            #define NP_STACKSIZE 16000
            #endif
            #if !defined(__MORPHOS__)
            #define NP_PPCStackSize TAG_IGNORE
            #endif

            SystemTags( name,   SYS_Input,       (ULONG)in,
                                SYS_Output,      (ULONG)out,
                                SYS_Error,       NULL,
                                NP_StackSize,    NP_STACKSIZE,
                                SYS_Asynch,      TRUE,
                                NP_PPCStackSize, 32000,
                                TAG_END);

            return TRUE;
        }

        Close(in);
    }

    return FALSE;
}

/**************************************************************************/

ULONG LIBFUNC
URL_OldLaunchPrefsApp(void)
{
    return URL_LaunchPrefsAppA(NULL);
}

/**************************************************************************/

ULONG LIBFUNC
URL_GetAttr(REG(d0,ULONG attr),REG(a0,ULONG *storage))
{
    switch (attr)
    {
        case URL_GetAttr_Version:          *storage = LIB_VERSION;    return TRUE;
        case URL_GetAttr_Revision:         *storage = LIB_REVISION;   return TRUE;
        case URL_GetAttr_VerString:        *storage = (ULONG)"$VER: openurl.library " LIB_REV_STRING " [" SYSTEMSHORT "/" CPU "] (" LIB_DATE ") " LIB_COPYRIGHT;; return TRUE;

        case URL_GetAttr_PrefsVer:         *storage = PREFS_VERSION;  return TRUE;

        case URL_GetAttr_HandlerVersion:   *storage = 0;              return TRUE;
        case URL_GetAttr_HandlerRevision:  *storage = 0;              return TRUE;
        case URL_GetAttr_HandlerVerString: *storage = (ULONG)"";      return TRUE;

        default: return FALSE;
    }
}

/**************************************************************************/

LONG LIBFUNC
dispatch(REG(a0, struct RexxMsg *msg), REG(a1, STRPTR *resPtr))
{
    TEXT  *fun = msg->rm_Args[0];
    ULONG  res, na = msg->rm_Action & RXARGMASK;

    if (!stricmp(fun,"OPENURL"))
    {
        if (na<1) return 17;
        else
        {
            struct TagItem tags[MAXRMARG+1];
            STRPTR url;
            int            i, j;

            for (i = na, j = 0, url = NULL; i>0; i--)
            {
                TEXT *arg = msg->rm_Args[i];
                Tag   tag;

                if (!arg || !*arg) continue;

                if (!stricmp(arg,"SHOW") || !stricmp(arg,"NOSHOW")) tag = URL_Show;
                else if (!stricmp(arg,"TOFRONT") || !stricmp(arg,"NOTOFRONT")) tag  = URL_BringToFront;
                     else if (!stricmp(arg,"NEWWIN") || !stricmp(arg,"NONEWWIN")) tag = URL_NewWindow;
                          else if (!stricmp(arg,"LAUNCH") || !stricmp(arg,"NOLAUNCH")) tag  = URL_Launch;
                               else
                               {
                                   url = arg;
                                   continue;
                               }

                tags[j].ti_Tag  = tag;
                tags[j++].ti_Data = strnicmp(arg,"NO",2);
            }

            tags[j].ti_Tag = TAG_END;

            res = url && URL_OpenA(url,tags);
        }
    }
    else
    {
        if (!stricmp(fun,"OPENURLPREFS"))
        {
            if (na!=0) return 17;

            res = URL_LaunchPrefsAppA(NULL);
        }
        else return 1;
    }

    return (*resPtr = (STRPTR)CreateArgstring(res ? "1" : "0",1)) ? 0 : 3;
}

/**************************************************************************/

