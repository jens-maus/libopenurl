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

 $Id: version.h 56 2009-05-18 07:28:47Z damato $

***************************************************************************/

#include "openurl.h"

#define CATCOMP_NUMBERS
#include "locale.h"
#include "macros.h"

#include <libraries/openurl.h>
#include <stdio.h>

#include "SDI_hook.h"

/**************************************************************************/
/*
** Little lamp class for enabled/disabled
*/

static struct MUI_CustomClass *lampClass = NULL;
#define lampObject NewObject(lampClass->mcc_Class,NULL

struct lampData
{
    LONG               enabled;
    LONG               disabled;
    LONG               detail;
    WORD               delta;

    ULONG              flags;
};

enum
{
    FLG_LampSetup    = 1<<0,
    FLG_LampDisabled = 1<<1,
};

/***********************************************************************/

static ULONG
mLampNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    return (ULONG)DoSuperNew(cl,obj,
        MUIA_Font, MUIV_Font_List,
        TAG_MORE,  msg->ops_AttrList);
}

/**************************************************************************/

static ULONG
mLampSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct lampData *data = INST_DATA(cl,obj);
    struct TagItem  *tag;

    if((tag = FindTagItem(MUIA_Lamp_Disabled,msg->ops_AttrList)))
    {
        if (tag->ti_Data) data->flags |= FLG_LampDisabled;
        else data->flags &= ~FLG_LampDisabled;

        /* Of course, we don't redraw here */
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mLampSetup(struct IClass *cl,Object *obj,struct MUIP_Setup *msg)
{
    struct lampData *data = INST_DATA(cl,obj);
    if (!DoSuperMethodA(cl,obj,(APTR)msg)) return FALSE;

    if (MUIMasterBase->lib_Version<20)
	{
    	data->enabled  = MUI_ObtainPen(muiRenderInfo(obj),(APTR)"r00000000,ffffffff,00000000",0);
	    data->disabled = MUI_ObtainPen(muiRenderInfo(obj),(APTR)"rffffffff,00000000,00000000",0);
	    data->detail   = MUI_ObtainPen(muiRenderInfo(obj),(APTR)"r00000000,00000000,00000000",0);
    }
	else
    {
    	data->enabled  = MUI_ObtainPen(muiRenderInfo(obj),(APTR)"r02ff00",0);
   		data->disabled = MUI_ObtainPen(muiRenderInfo(obj),(APTR)"rff0000",0);
		data->detail   = MUI_ObtainPen(muiRenderInfo(obj),(APTR)"r000000",0);
	}

    data->flags |= FLG_LampSetup;
    return TRUE;
}

/***********************************************************************/

static ULONG
mLampCleanup(struct IClass *cl,Object *obj,struct MUIP_Setup *msg)
{
    struct lampData *data = INST_DATA(cl,obj);

    if (data->flags & FLG_LampSetup)
    {
        MUI_ReleasePen(muiRenderInfo(obj),data->enabled);
        MUI_ReleasePen(muiRenderInfo(obj),data->disabled);
        MUI_ReleasePen(muiRenderInfo(obj),data->detail);

        data->flags &= ~FLG_LampSetup;
    }

    return DoSuperMethodA(cl,obj,(APTR)msg);
}

/***********************************************************************/

static ULONG
mLampAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    struct lampData          *data = INST_DATA(cl,obj);
    struct RastPort          rp;
    struct TextExtent        te;
    UWORD                    w, h, d;

    DoSuperMethodA(cl,obj,(APTR)msg);

    CopyMem(&_screen(obj)->RastPort,&rp,sizeof(rp));

    /* Don't ask or modify ! */

    SetFont(&rp,_font(obj));
    TextExtent(&rp,"  ",2,&te);

    w = te.te_Width;
    h = te.te_Height;

    if (w>=h) d = w;
    else d = h;

    data->delta = te.te_Extent.MinY;

    msg->MinMaxInfo->MinWidth  += d;
    msg->MinMaxInfo->MinHeight += h;
    msg->MinMaxInfo->DefWidth  += d;
    msg->MinMaxInfo->DefHeight += h;
    msg->MinMaxInfo->MaxWidth  += d;
    msg->MinMaxInfo->MaxHeight += h;

    return 0;
}

/***********************************************************************/

static ULONG
mLampDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
    struct lampData *data = INST_DATA(cl,obj);
    ULONG           res;

    res = DoSuperMethodA(cl,obj,(APTR)msg);

    if (msg->flags & (MADF_DRAWOBJECT|MADF_DRAWUPDATE))
    {
        WORD l, t, r, b;

        /* Don't ask or modify ! */

        l = _mleft(obj);
        r = _mright(obj);
        t = _mtop(obj)+(_mheight(obj)+data->delta)/2-1;
        b = t-data->delta;

        if (r-l>2)
        {
            l += 1;
            r -= 1;
        }

        if (b-t>2)
        {
            t += 1;
            b -= 1;
        }

        SetAPen(_rp(obj),MUIPEN((data->flags & FLG_LampDisabled) ? data->disabled : data->enabled));
        RectFill(_rp(obj),l,t,r,b);

        SetAPen(_rp(obj),MUIPEN(data->detail));
        Move(_rp(obj),l,t);
        Draw(_rp(obj),r,t);
        Draw(_rp(obj),r,b);
        Draw(_rp(obj),l,b);
        Draw(_rp(obj),l,t);
    }

    return res;
}

/***********************************************************************/

SDISPATCHER(lampDispatcher)
{
    switch(msg->MethodID)
    {
        case OM_NEW:         return mLampNew(cl,obj,(APTR)msg);
        case OM_SET:         return mLampSets(cl,obj,(APTR)msg);

        case MUIM_Setup:     return mLampSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup:   return mLampCleanup(cl,obj,(APTR)msg);
        case MUIM_AskMinMax: return mLampAskMinMax(cl,obj,(APTR)msg);
        case MUIM_Draw:      return mLampDraw(cl,obj,(APTR)msg);

        default:             return DoSuperMethodA(cl,obj,msg);
    }
}

/***********************************************************************/

static ULONG
initLampClass(void)
{
    return (ULONG)(lampClass = MUI_CreateCustomClass(NULL,MUIC_Rectangle,NULL,sizeof(struct lampData),ENTRY(lampDispatcher)));
}

/**************************************************************************/

static void
disposeLampClass(void)
{
    if (lampClass) MUI_DeleteCustomClass(lampClass);
}

/**************************************************************************/
/*
** List of clients with lamps
*/

static struct MUI_CustomClass *listClass = NULL;
#define listObject NewObject(listClass->mcc_Class,NULL

struct listData
{
    Object *olamp;
    APTR   lamp;
    TEXT   col0buf[NAME_LEN+16];

    ULONG  nameOfs;
    ULONG  pathOfs;
    ULONG  nodeSize;

    TEXT  format[64];

    ULONG  flags;
};

enum
{
    FLG_ListSetup = 1<<0,
};

/* Used for Import/Export */
struct listIO
{
    ULONG len;
    TEXT format[64];
};

/**************************************************************************/

HOOKPROTO(conFun, struct URL_Node *, APTR pool, struct URL_Node *node)
{
    struct listData *data = hook->h_Data;
    struct URL_Node *new;

    if (node->Flags & UNF_NTALLOC)
    {
        new = node;
        node->Flags &= ~UNF_NTALLOC;
    }
    else if((new = AllocPooled(pool,data->nodeSize)))
      CopyMem(node,new,data->nodeSize);

    return new;
}
MakeStaticHook(conHook, conFun);

/**************************************************************************/

HOOKPROTO(destFun, void, APTR pool, struct URL_Node *node)
{
    struct listData *data = hook->h_Data;

    FreePooled(pool,node,data->nodeSize);
}
MakeStaticHook(destHook, destFun);

/**************************************************************************/

HOOKPROTO(dispFun, void, STRPTR *array, struct URL_Node *node)
{
    struct listData *data = hook->h_Data;

    if (node)
    {
        if (data->lamp)
        {
            set(data->olamp,MUIA_Lamp_Disabled,node->Flags & UNF_DISABLED);
            sprintf(data->col0buf,"\33O[%08lx]",(ULONG)data->lamp);
            *array++ = data->col0buf;
        }
        else *array++ = (node->Flags & UNF_DISABLED) ? " " : ">";

        *array++ = (STRPTR)node+data->nameOfs;
        *array   = (STRPTR)node+data->pathOfs;
    }
    else
    {
        *array++ = " ";
        *array++ = getString(MSG_Edit_ListName);
        *array   = getString(MSG_Edit_ListPath);
    }
}
MakeStaticHook(dispHook, dispFun);

/**************************************************************************/

static ULONG
mListNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if (obj = (Object *)DoSuperNew(cl,obj,
            InputListFrame,
            MUIA_List_Title,         TRUE,
            MUIA_List_Format,        "C=0,C=1,C=2",
            MUIA_List_DragSortable,  TRUE,
            MUIA_List_Pool,          g_pool,
            MUIA_List_ConstructHook, &conHook,
            MUIA_List_DestructHook,  &destHook,
            MUIA_List_DisplayHook,   &dispHook,
            MUIA_List_DragSortable,  TRUE,
            MUIA_List_ShowDropMarks, TRUE,
            TAG_MORE, msg->ops_AttrList))
    {
        struct listData *data = INST_DATA(cl,obj);

        data->nameOfs  = GetTagData(MUIA_AppList_NodeNameOffset,0,msg->ops_AttrList);
        data->pathOfs  = GetTagData(MUIA_AppList_NodePathOffset,0,msg->ops_AttrList);
        data->nodeSize = GetTagData(MUIA_AppList_NodeSize,0,msg->ops_AttrList);

        strlcpy(data->format,"C=0,C=1,C=2", sizeof(data->format));

        if (lampClass) data->olamp = lampObject, End;

        conHook.h_Data = data;
        destHook.h_Data = data;
        dispHook.h_Data = data;
    }

    return (ULONG)obj;
}

/**************************************************************************/

static ULONG
mListDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct listData *data = INST_DATA(cl,obj);

    if (data->olamp) MUI_DisposeObject(data->olamp);

    return DoSuperMethodA(cl,obj,msg);
}

/**************************************************************************/

static ULONG
mListSetup(struct IClass *cl,Object *obj,struct MUIP_Setup *msg)
{
    struct listData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,(APTR)msg)) return FALSE;

    /* After thinking about that hard, I decided to use the lamp in >=8 color screen */
    if (data->olamp && (GetBitMapAttr(_screen(obj)->RastPort.BitMap,BMA_DEPTH)>3))
        data->lamp = (APTR)DoSuperMethod(cl,obj,MUIM_List_CreateImage,(ULONG)data->olamp,0);

    data->flags |= FLG_ListSetup;

    return TRUE;
}

/***********************************************************************/

static ULONG
mListCleanup(struct IClass *cl,Object *obj,struct MUIP_Setup *msg)
{
    struct listData *data = INST_DATA(cl,obj);

    if (data->flags & FLG_ListSetup)
    {
        if (data->lamp)
        {
            DoSuperMethod(cl,obj,MUIM_List_DeleteImage,(ULONG)data->lamp);
            data->lamp = NULL;
        }

        data->flags &= ~FLG_ListSetup;
    }

    return DoSuperMethodA(cl,obj,(APTR)msg);
}

/***********************************************************************/
/*
** Import format
*/

static ULONG
mListImport(struct IClass *cl,Object *obj,struct MUIP_Import *msg)
{
    register ULONG id;

    if (MUIMasterBase->lib_Version<20) return 0;

    if (id = (muiNotifyData(obj)->mnd_ObjectID))
    {
        register struct listIO *io;

        if (io = (struct listIO *)DoMethod(msg->dataspace,MUIM_Dataspace_Find,id))
        {
            struct listData *data = INST_DATA(cl,obj);

            stccpy(data->format,io->format,sizeof(data->format));
            set(obj,MUIA_List_Format,data->format);
        }
    }

    return 0;
}

/***********************************************************************/
/*
** Export format
*/

static ULONG
mListExport(UNUSED struct IClass *cl,Object *obj,struct MUIP_Import *msg)
{
    register ULONG id;

    if (MUIMasterBase->lib_Version<20) return 0;

    if (id = (muiNotifyData(obj)->mnd_ObjectID))
    {
        struct listIO io;
        STRPTR        f;

        get(obj,MUIA_List_Format,&f);
        io.len = strlen(f)+1;
        stccpy(io.format,f,sizeof(io.format));

        DoMethod(msg->dataspace,MUIM_Dataspace_Add,(ULONG)&io,sizeof(ULONG)+io.len,id);
    }

    return 0;
}

/**************************************************************************/
/*
** Check if format changed
*/

static ULONG
mListCheckSave(struct IClass *cl,Object *obj,UNUSED Msg msg)
{
    struct listData *data = INST_DATA(cl,obj);
    UBYTE           *f;

    get(obj,MUIA_List_Format,&f);

    return (ULONG)strcmp((STRPTR)f,(STRPTR)&data->format);
}

/**************************************************************************/

SDISPATCHER(listDispatcher)
{
    switch (msg->MethodID)
    {
        case OM_NEW:                  return mListNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:              return mListDispose(cl,obj,(APTR)msg);

        case MUIM_Setup:              return mListSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup:            return mListCleanup(cl,obj,(APTR)msg);
        case MUIM_Import:             return mListImport(cl,obj,(APTR)msg);
        case MUIM_Export:             return mListExport(cl,obj,(APTR)msg);

        case MUIM_App_CheckSave:      return mListCheckSave(cl,obj,(APTR)msg);

        default:                      return DoSuperMethodA(cl,obj,msg);
    }
}

/**************************************************************************/

static ULONG
initListClass(void)
{
    return (ULONG)(listClass = MUI_CreateCustomClass(NULL,MUIC_List,NULL,sizeof(struct listData),ENTRY(listDispatcher)));
}

/**************************************************************************/

static void
disposeListClass(void)
{
    if (listClass) MUI_DeleteCustomClass(listClass);
}

/**************************************************************************/

struct data
{
    Object         *appList;
    Object         *add;
    Object         *edit;
    Object         *clone;
    Object         *delete;
    Object         *disable;
    Object         *up;
    Object         *down;

    ULONG          nameOfs;
    ULONG          pathOfs;
    ULONG          nodeSize;
    struct IClass  *editClass;
    ULONG          editAttr;
    ULONG          listAttr;

    STRPTR         newNodeName;
};

/**************************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object        *appl, *addb, *editb, *cloneb, *deleteb, *disableb, *upb, *downb;
    struct IClass *editWinClass;
    STRPTR         nodeName, helpNode;
    ULONG         nameOfs, pathOfs, nodeSize, editWinAttr, listAttr, help, id;

    /* What we are  */
    switch (GetTagData(MUIA_AppList_Type,0,msg->ops_AttrList))
    {
        case MUIV_AppList_Type_Browser:
            nameOfs            = sizeof(struct MinNode) + sizeof(ULONG);
            pathOfs            = sizeof(struct MinNode) + sizeof(ULONG) + NAME_LEN;
            nodeSize           = sizeof(struct URL_BrowserNode);
            editWinClass       = g_browserEditWinClass->mcc_Class;
            editWinAttr        = MUIA_BrowserEditWin_Browser;
            listAttr           = MUIA_BrowserEditWin_ListObj;
            nodeName           = getString(MSG_Browser_NewBrowser);
            helpNode           = "BROWSER";
            help               = MSG_Browser_List_Help;
            id                 = MAKE_ID('B','L','S','T');
            break;

        case MUIV_AppList_Type_Mailer:
            nameOfs            = sizeof(struct MinNode) + sizeof(ULONG);
            pathOfs            = sizeof(struct MinNode) + sizeof(ULONG) + NAME_LEN;
            nodeSize           = sizeof(struct URL_MailerNode);
            editWinClass       = g_mailerEditWinClass->mcc_Class;
            editWinAttr        = MUIA_MailerEditWin_Mailer;
            listAttr           = MUIA_MailerEditWin_ListObj;
            nodeName           = getString(MSG_Mailer_NewMailer);
            helpNode           = "MAILERS";
            help               = MSG_Mailer_List_Help;
            id                 = MAKE_ID('M','L','S','T');
            break;

        case MUIV_AppList_Type_FTP:
            nameOfs            = sizeof(struct MinNode) + sizeof(ULONG);
            pathOfs            = sizeof(struct MinNode) + sizeof(ULONG) + NAME_LEN;
            nodeSize           = sizeof(struct URL_FTPNode);
            editWinClass       = g_FTPEditWinClass->mcc_Class;
            editWinAttr        = MUIA_FTPEditWin_FTP;
            listAttr           = MUIA_FTPEditWin_ListObj;
            nodeName           = getString(MSG_FTP_NewFTP);
            helpNode           = "FTPS";
            help               = MSG_FTP_List_Help;
            id                 = MAKE_ID('F','L','S','T');
            break;

        default:
            return 0;
    }

    if (obj = (Object *)DoSuperNew(cl,obj,
                MUIA_HelpNode,           helpNode,
                MUIA_ShortHelp,          getString(help),
                MUIA_Group_Horiz,        TRUE,
                MUIA_Group_HorizSpacing, 2,

                Child, ListviewObject,
                    MUIA_CycleChain,               TRUE,
                    MUIA_Listview_DefClickColumn,  1,
                    MUIA_Listview_DragType,        MUIV_Listview_DragType_Immediate,
                    MUIA_Listview_List, appl = listObject,
                        MUIA_ObjectID,               id,
                        MUIA_AppList_NodeNameOffset, nameOfs,
                        MUIA_AppList_NodePathOffset, pathOfs,
                        MUIA_AppList_NodeSize,       nodeSize,
                    End,
                End,

                Child, VGroup,
                    //VirtualFrame,
                    //MUIA_Background, MUII_GroupBack,
                    MUIA_Group_VertSpacing, 2,
                    MUIA_Weight,     		0,

                    Child, addb    = obutton(MSG_AppList_Add,MSG_AppList_Add_Help),
                    Child, editb   = obutton(MSG_AppList_Edit,MSG_AppList_Edit_Help),
                    Child, cloneb  = obutton(MSG_AppList_Clone,MSG_AppList_Clone_Help),
                    Child, deleteb = obutton(MSG_AppList_Delete,MSG_AppList_Delete_Help),
                    Child, HGroup,
                        MUIA_Group_HorizSpacing, 1,
                        Child, upb      = oibutton(IBT_Up,MSG_AppList_MoveUp_Help),
                        Child, downb    = oibutton(IBT_Down,MSG_AppList_MoveDown_Help),
                        Child, disableb = otbutton(MSG_AppList_Disable,MSG_AppList_Disable_Help),
                    End,
                    Child, VSpace(0),
                End,

            TAG_MORE, msg->ops_AttrList))
    {
        struct data *data = INST_DATA(cl,obj);

        /* init instance data */
        data->appList  = appl;
        data->add      = addb;
        data->edit     = editb;
        data->clone    = cloneb;
        data->delete   = deleteb;
        data->disable  = disableb;
        data->up       = upb;
        data->down     = downb;

        data->nameOfs  = nameOfs;
        data->pathOfs  = pathOfs;
        data->nodeSize = nodeSize;

        data->editClass   = editWinClass;
        data->editAttr    = editWinAttr;
        data->listAttr    = listAttr;
        data->newNodeName = nodeName;

        /* listview */
        DoMethod(appl,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_AppList_ActiveChanged);
        DoMethod(appl,MUIM_Notify,MUIA_Listview_DoubleClick,MUIV_EveryTime,(ULONG)obj,2,MUIM_AppList_Edit,TRUE);

        /* buttons */
        DoMethod(obj,MUIM_MultiSet,MUIA_Disabled,TRUE,
            (ULONG)editb,
            (ULONG)cloneb,
            (ULONG)deleteb,
            (ULONG)disableb,
            (ULONG)upb,
            (ULONG)downb,
            NULL);

        /* list buttons */
        DoMethod(addb,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,1,MUIM_AppList_Add);
        DoMethod(editb,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,1,MUIM_AppList_Edit,FALSE);
        DoMethod(cloneb,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,1,MUIM_AppList_Clone);
        DoMethod(deleteb,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,1,MUIM_AppList_Delete);
        DoMethod(disableb,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,2,MUIM_AppList_Disable,MUIV_TriggerValue);
        DoMethod(upb,MUIM_Notify,MUIA_Timer,MUIV_EveryTime,(ULONG)obj,2,MUIM_AppList_Move,TRUE);
        DoMethod(downb,MUIM_Notify,MUIA_Timer,MUIV_EveryTime,(ULONG)obj,2,MUIM_AppList_Move,FALSE);
    }

    return (ULONG)obj;
}

/**************************************************************************/
/*
** I hate this: it will be removed asap!
*/

static ULONG
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct data *data = INST_DATA(cl,obj);

    switch (msg->opg_AttrID)
    {
        case MUIA_AppList_ListObj: *msg->opg_Storage = (ULONG)data->appList; return TRUE;
        default: return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/**************************************************************************/

static ULONG
mAdd(struct IClass *cl,Object *obj,UNUSED Msg msg)
{
    struct data     *data = INST_DATA(cl,obj);
    struct URL_Node *node;

    if (!(node = AllocPooled(g_pool,data->nodeSize))) return FALSE;

    memset(node,0,data->nodeSize);
    strcpy((STRPTR)node+data->nameOfs,data->newNodeName);

    node->Flags = UNF_NEW|UNF_NTALLOC;

    DoMethod(data->appList,MUIM_List_InsertSingle,(ULONG)node,MUIV_List_Insert_Bottom);

    set(data->appList,MUIA_List_Active,xget(data->appList,MUIA_List_InsertPosition));

    DoMethod(obj,MUIM_AppList_Edit,FALSE);

    return TRUE;
}

/**************************************************************************/

static ULONG
mEdit(struct IClass *cl,Object *obj,struct MUIP_AppList_Edit *msg)
{
    struct data     *data = INST_DATA(cl,obj);
    struct URL_Node *node;

    DoMethod(data->appList,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,(ULONG)&node);
    if (node)
    {
        if (msg->check && (xget(data->appList,MUIA_Listview_ClickColumn)==0))
        {
            set(data->appList,MUIA_Listview_ClickColumn,1);

            if (node->Flags & UNF_DISABLED) node->Flags &= ~UNF_DISABLED;
            else node->Flags |= UNF_DISABLED;

            DoMethod(data->appList,MUIM_List_Redraw,xget(data->appList,MUIA_List_Active));
            set(data->disable,MUIA_Selected,node->Flags & UNF_DISABLED);
        }
        else DoMethod(_app(obj),MUIM_App_OpenWin,(ULONG)data->editClass,data->editAttr,(ULONG)node,data->listAttr,(ULONG)data->appList,TAG_END);
    }

    return TRUE;
}

/**************************************************************************/

static ULONG
mClone(struct IClass *cl,Object *obj,UNUSED Msg msg)
{
    struct data     *data = INST_DATA(cl,obj);
    struct URL_Node *node;
    ULONG           active;

    get(data->appList,MUIA_List_Active,&active);
    DoMethod(data->appList,MUIM_List_GetEntry,active,(ULONG)&node);
    if (node)
    {
        struct URL_Node *new;

        if (!(new = AllocPooled(g_pool,data->nodeSize))) return FALSE;
        CopyMem(node,new,data->nodeSize);
        new->Flags |= UNF_NEW|UNF_NTALLOC;

        DoMethod(data->appList,MUIM_List_InsertSingle,(ULONG)new,MUIV_List_Insert_Bottom);
        set(data->appList,MUIA_List_Active,MUIV_List_Active_Bottom);

        DoMethod(obj,MUIM_AppList_Edit,FALSE);
    }

    return TRUE;
}

/**************************************************************************/

static ULONG
mDelete(struct IClass *cl,Object *obj,UNUSED Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    UBYTE       *node;
    ULONG       active;

    get(data->appList,MUIA_List_Active,&active);
    DoMethod(data->appList,MUIM_List_GetEntry,active,(ULONG)&node);
    if (node)
    {
        DoMethod(_app(obj),MUIM_App_CloseWin,data->editAttr,(ULONG)node);
        DoMethod(data->appList,MUIM_List_Remove,active);
    }

    return TRUE;
}

/**************************************************************************/

static ULONG
mActiveChanged(struct IClass *cl,Object *obj,UNUSED Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    LONG    a;

    a = (LONG)xget(data->appList,MUIA_List_Active);
    if (a>=0)
    {
        struct URL_Node *node;
        LONG       n;

        DoMethod(obj,MUIM_MultiSet,MUIA_Disabled,FALSE,
        (ULONG)data->edit,
            (ULONG)data->clone,
            (ULONG)data->delete,
            (ULONG)data->disable,
            NULL);

        DoMethod(data->appList,MUIM_List_GetEntry,a,(ULONG)&node);
        set(data->disable,MUIA_Selected,node->Flags & UNF_DISABLED);

        if (a==0) SetAttrs(data->up,MUIA_Selected,FALSE,MUIA_Disabled,TRUE,TAG_DONE);
        else set(data->up,MUIA_Disabled,FALSE);

        n = xget(data->appList,MUIA_List_Entries);
        if (n-1<=a) SetAttrs(data->down,MUIA_Selected,FALSE,MUIA_Disabled,TRUE,TAG_DONE);
        else set(data->down,MUIA_Disabled,FALSE);
    }
    else
    {
        set(data->disable,MUIA_Selected,FALSE);

        DoMethod(obj,MUIM_MultiSet,MUIA_Disabled,TRUE,
            (ULONG)data->edit,
            (ULONG)data->clone,
            (ULONG)data->delete,
            (ULONG)data->disable,
            (ULONG)data->up,
            (ULONG)data->down,
            NULL);
    }

    return TRUE;
}

/**************************************************************************/

static ULONG
mDisable(struct IClass *cl,Object *obj,struct MUIP_AppList_Disable *msg)
{
    struct data     *data = INST_DATA(cl,obj);
    struct URL_Node *node;

    DoMethod(data->appList,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,(ULONG)&node);
    if (node)
    {
        if (!BOOLSAME(msg->disable,node->Flags & UNF_DISABLED))
        {
            if (msg->disable) node->Flags |= UNF_DISABLED;
            else node->Flags &= ~UNF_DISABLED;

            DoMethod(data->appList,MUIM_List_Redraw,xget(data->appList,MUIA_List_Active));
        }
    }

    return TRUE;
}

/**************************************************************************/

static ULONG
mMove(struct IClass *cl,Object *obj,struct MUIP_AppList_Move *msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoMethod(data->appList,MUIM_List_Exchange,MUIV_List_Exchange_Active,msg->up ? MUIV_List_Exchange_Previous : MUIV_List_Exchange_Next);
    set(data->appList,MUIA_List_Active,msg->up ? MUIV_List_Active_Up : MUIV_List_Active_Down);

    return 0;
}

/**************************************************************************/
/*
** Forward to the list
*/

static ULONG
mCheckSave(struct IClass *cl,Object *obj,UNUSED Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    return DoMethod(data->appList,MUIM_App_CheckSave);
}

/**************************************************************************/

SDISPATCHER(dispatcher)
{
    switch (msg->MethodID)
    {
        case OM_NEW:                     return mNew(cl,obj,(APTR)msg);
        case OM_GET:                     return mGet(cl,obj,(APTR)msg);

        case MUIM_AppList_Add:           return mAdd(cl,obj,(APTR)msg);
        case MUIM_AppList_Edit:          return mEdit(cl,obj,(APTR)msg);
        case MUIM_AppList_Clone:         return mClone(cl,obj,(APTR)msg);
        case MUIM_AppList_Delete:        return mDelete(cl,obj,(APTR)msg);
        case MUIM_AppList_ActiveChanged: return mActiveChanged(cl,obj,(APTR)msg);
        case MUIM_AppList_Disable:       return mDisable(cl,obj,(APTR)msg);
        case MUIM_AppList_Move:          return mMove(cl,obj,(APTR)msg);
        case MUIM_App_CheckSave:    return mCheckSave(cl,obj,(APTR)msg);

        default:                         return DoSuperMethodA(cl,obj,msg);
    }
}

/**************************************************************************/

ULONG
initAppListClass(void)
{
    if (initListClass())
    {
        if (g_appListClass = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct data),ENTRY(dispatcher)))
        {
            initLampClass();

            return TRUE;
        }

        disposeListClass();
    }

    return FALSE;
}

/**************************************************************************/

void
disposeAppListClass(void)
{
    disposeLampClass();
    disposeListClass();
    if (g_appListClass) MUI_DeleteCustomClass(g_appListClass);
}

/**************************************************************************/

