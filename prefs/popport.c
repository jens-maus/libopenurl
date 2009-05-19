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

#include "openurl.h"

#include <libraries/openurl.h>
#include <exec/execbase.h>

#include "SDI_hook.h"
#include "macros.h"

/**************************************************************************/
/*
** Public ports list
*/

static struct MUI_CustomClass *listClass = NULL;
#define listObject NewObject(listClass->mcc_Class,NULL

static ULONG
mListNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    return (ULONG)DoSuperNew(cl,obj,
      MUIA_Frame,              MUIV_Frame_InputList,
      MUIA_Background,         MUII_ListBack,
      MUIA_List_AutoVisible,   TRUE,
      MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
      MUIA_List_DestructHook,  MUIV_List_DestructHook_String,
      TAG_MORE,(ULONG)msg->ops_AttrList);
}

/**************************************************************************/

static ULONG
mListSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct Node *mstate;

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    DoSuperMethod(cl,obj,MUIM_List_Clear);

    // this Forbid() will very likely be broken by any memory allocation during the
    // MUIM_List_InsertSingle invocation. Better replace this by a more secure approach.
    Forbid();

    for (mstate = SysBase->PortList.lh_Head; mstate->ln_Succ; mstate = mstate->ln_Succ)
        DoSuperMethod(cl,obj,MUIM_List_InsertSingle,(ULONG)mstate->ln_Name,MUIV_List_Insert_Sorted);

    Permit();

    return TRUE;
}

/**************************************************************************/

SDISPATCHER(listDispatcher)
{
    switch (msg->MethodID)
    {
        case OM_NEW:     return mListNew(cl,obj,(APTR)msg);

        case MUIM_Setup: return mListSetup(cl,obj,(APTR)msg);

        default:         return DoSuperMethodA(cl,obj,msg);
    }
}

/**************************************************************************/

static ULONG
initListClass(void)
{
    return (ULONG)(listClass = MUI_CreateCustomClass(NULL,MUIC_List,NULL,0,ENTRY(listDispatcher)));
}

/**************************************************************************/

static void
disposeListClass(void)
{
    if (listClass) MUI_DeleteCustomClass(listClass);
}

/**************************************************************************/

HOOKPROTONH(windowFun, void, Object *pop, Object *win)
{
  set(win,MUIA_Window_DefaultObject,pop);
}
MakeStaticHook(windowHook, windowFun);

/***********************************************************************/

HOOKPROTONH(openFun, ULONG, Object *list, Object *str)
{
    STRPTR s, x;
    int   i;

    s = (STRPTR)xget(str,MUIA_String_Contents);

    for (i = 0; ;i++)
    {
        DoMethod(list,MUIM_List_GetEntry,i,(ULONG)&x);
        if (!x)
        {
            set(list,MUIA_List_Active,MUIV_List_Active_Off);
            break;
        }
        else
            if (!stricmp(x,s))
            {
                set(list,MUIA_List_Active,i);
                break;
            }
    }

    return TRUE;
}
MakeStaticHook(openHook, openFun);

/***********************************************************************/

HOOKPROTONH(closeFun, void, Object *list, Object *str)
{
    STRPTR port;

    DoMethod(list,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,(ULONG)&port);
    if (port)
    {
        TEXT buf[PORT_LEN], *dot, *digit;

        dot = strrchr(port,'.');

        if (dot)
        {
            dot++;

            for (digit = dot; *digit; digit++)
                if (!isdigit(*digit))
                {
                    dot = NULL;
                    break;
                }

                if (dot)
                {
                    stccpy(buf,port,dot-port);
                    port = buf;
                }
        }
    }

    set(str,MUIA_String_Contents,port);
}
MakeStaticHook(closeHook, closeFun);

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *lv;

    if (obj = (Object *)DoSuperNew(cl,obj,

            MUIA_Popstring_String, ostring(GetTagData(MUIA_Popport_Len,64,msg->ops_AttrList),GetTagData(MUIA_Popport_Key,0,msg->ops_AttrList),0),
            MUIA_Popstring_Button, opopbutton(MUII_PopUp,0),

            MUIA_Popobject_Object, lv = ListviewObject,
                MUIA_Listview_List, listObject, End,
            End,
            MUIA_Popobject_WindowHook, &windowHook,
            MUIA_Popobject_StrObjHook, &openHook,
            MUIA_Popobject_ObjStrHook, &closeHook,

            TAG_MORE,(ULONG)msg->ops_AttrList))
    {
        DoMethod(lv,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,(ULONG)obj,2,MUIM_Popstring_Close,TRUE);
    }

    return (ULONG)obj;
}

/***********************************************************************/

SDISPATCHER(dispatcher)
{
    switch (msg->MethodID)
    {
        case OM_NEW: return mNew(cl,obj,(APTR)msg);
        default:     return DoSuperMethodA(cl,obj,msg);
    }
}

/***********************************************************************/

ULONG
initPopportClass(void)
{
    if (initListClass())
    {
        if (g_popportClass = MUI_CreateCustomClass(NULL,MUIC_Popobject,NULL,0,ENTRY(dispatcher)))
            return TRUE;

        disposeListClass();
    }

    return FALSE;
}

/**************************************************************************/

void
disposePopportClass(void)
{
    disposeListClass();
    if (g_popportClass) MUI_DeleteCustomClass(g_popportClass);
}

/**************************************************************************/

