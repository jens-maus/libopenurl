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

 $Id: Makefile 56 2009-05-18 07:28:47Z damato $

***************************************************************************/

#include "openurl.h"

#define CATCOMP_NUMBERS
#include "locale.h"

#include "SDI_hook.h"

#include "version.h"

#include <libraries/openurl.h>

/***********************************************************************/
/*
** If Urltext.mcc is present use it,
** otherwise falls back to a text object
*/

static Object *
ourltext(STRPTR url,STRPTR text)
{
    Object *o;

    o = UrltextObject,
        MUIA_Urltext_Text,           text,
        MUIA_Urltext_Url,            url,
        MUIA_Urltext_SetMax,         FALSE,
        MUIA_Urltext_NoOpenURLPrefs, TRUE,
    End;

    if (!o) o = TextObject, MUIA_Text_SetMax, FALSE, MUIA_Text_Contents, text ? text : url, End;

    return o;
}

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    TEXT  buf[64];
    STRPTR lver;
    Object *ok;

    if(!URL_GetAttr(URL_GetAttr_VerString,(ULONG *)((void *)&lver)))
      lver = "";

    snprintf(buf, sizeof(buf), "OpenURL\n%s", lver);

    if ((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Window_Title,          getString(MSG_About_WinTitle),
            MUIA_Window_ScreenTitle,    getString(MSG_App_ScreenTitle),
            MUIA_Window_IconifyGadget,  FALSE,
            MUIA_Window_MenuGadget,     FALSE,
            MUIA_Window_SnapshotGadget, FALSE,
            MUIA_Window_ConfigGadget,   FALSE,
            MUIA_Window_SizeGadget,     FALSE,
            MUIA_Window_CloseGadget,    FALSE,
            MUIA_Window_AllowTopMenus,  FALSE,

            WindowContents, VGroup,
                MUIA_Background, MUII_RequesterBack,

                Child, HGroup,
                    Child, HSpace(0),
                    Child, TextObject,
                        MUIA_Text_Contents, getString(MSG_About_Descr),
                    End,
                    Child, HSpace(0),
                End,

                Child, RectangleObject, MUIA_FixHeight, 4, End,

                Child, HGroup,
                    Child, HSpace(0),
                    Child, TextObject,
                        MUIA_Text_PreParse, "\33c",
                        MUIA_Text_Contents, buf,
                    End,
                    Child, HSpace(0),
                    Child, ourltext("http://sourceforge.net/projects/openurllib",NULL),
                End,

                Child, HGroup,
                    Child, HSpace(0),
                    Child, ourltext("http://sourceforge.net/projects/openurllib",NULL),
                    Child, HSpace(0),
                End,

                //Child, RectangleObject, MUIA_FixHeight, 4, End,

                Child, RectangleObject, MUIA_Rectangle_HBar, TRUE, End,

                Child, HGroup,
                    Child, RectangleObject, MUIA_Weight, 200, End,
                    Child, ok = obutton(MSG_About_OK,0),
                    Child, RectangleObject, MUIA_Weight, 200, End,
                End,
            End,
            TAG_MORE, msg->ops_AttrList)))
    {
        superset(cl,obj,MUIA_Window_ActiveObject,ok);

        DoMethod(ok,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,3,
            MUIM_Set,MUIA_Window_CloseRequest,TRUE);
    }

    return (ULONG)obj;
}

/***********************************************************************/

SDISPATCHER(dispatcher)
{
    switch(msg->MethodID)
    {
        case OM_NEW: return mNew(cl,obj,(APTR)msg);
        default:     return DoSuperMethodA(cl,obj,msg);
    }
}

/***********************************************************************/

ULONG
initAboutClass(void)
{
    return (ULONG)(g_aboutClass = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,0,ENTRY(dispatcher)));
}

/***********************************************************************/

void
disposeAboutClass(void)
{
    if (g_aboutClass) MUI_DeleteCustomClass(g_aboutClass);
}

/***********************************************************************/

