/***************************************************************************

 openurl.library - universal URL display and browser launcher library
 Copyright (C) 1998-2005 by Troels Walsted Hansen, et al.
 Copyright (C) 2005-2021 openurl.library Open Source Team

 This library is free software; it has been placed in the public domain
 and you can freely redistribute it and/or modify it. Please note, however,
 that some components may be under the LGPL or GPL license.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 openurl.library project: http://github.com/jens-maus/libopenurl

 $Id$

***************************************************************************/

#include "browsers.h"

#include "gui_global.h"
#include "utility.h"
//#include "macros.h"
#include "version.h"

#include <classes/window.h>
#include <libraries/openurl.h>

#include <reaction/reaction_macros.h>
#include "my_reaction_macros.h"

#include <images/label.h>
#include <gadgets/layout.h>
#include <gadgets/space.h>
#include <gadgets/listbrowser.h>
#include <gadgets/string.h>
#include <gadgets/getfile.h>
#include <gadgets/chooser.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
/*#include <proto/label.h>
#include <proto/space.h>
#include <proto/layout.h>
#include <proto/window.h>
#include <proto/string.h>
#include <proto/getfile.h>*/
#include <proto/listbrowser.h>


extern Class *ChooserClass, *StringClass, *GetFileClass, *LabelClass, *CheckBoxClass,
             *RequesterClass, *ButtonClass, *SpaceClass, *LayoutClass, *StringClass,
             *GetFileClass, *WindowClass;

extern Object *Objects[OBJ_NUM];
extern Object *edit_brow_win;
//extern Object *menustripobj;

extern struct List *popup_www_ftp, *popup_arexxports;

extern struct MsgPort *AppPort;
//extern struct Hook idcmphook;
extern BOOL showhints;

struct Window *edit_brow_window;


Object * make_edit_brow_win(void)
{
    return WindowObject,
        WA_ScreenTitle,        "OpenURL " LIB_REV_STRING " (" LIB_DATE ")",//getString(MSG_App_ScreenTitle),
        WA_Title,              getString(MSG_Browser_WinTitle),
        WA_DragBar,            TRUE,
        WA_CloseGadget,        TRUE,
        WA_SizeGadget,         TRUE,
        WA_DepthGadget,        TRUE,
        WA_Activate,           TRUE,
  //      WINDOW_AppPort,        AppPort,
        WINDOW_SharedPort,     AppPort,
        //WINDOW_IDCMPHook,      &idcmphook,
        //WINDOW_IDCMPHookBits,  IDCMP_IDCMPUPDATE,
        WINDOW_GadgetHelp,     showhints,
        WINDOW_Position,       WPOS_CENTERSCREEN,
        WINDOW_LockHeight,     TRUE,
/*#ifdef MENUCLASS
        WA_MenuStrip, menustripobj,
#else
        WINDOW_NewMenu, menu,
#endif*/
        WINDOW_Layout,         VLayoutObject,
            LAYOUT_SpaceOuter,  TRUE,
            LAYOUT_AddChild,  OBJ(OBJ_BROW_ALIGN_T) = VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_BevelStyle,  BVS_GROUP,
                LAYOUT_Label,       getString(MSG_Edit_Definitions),

                LAYOUT_AddChild,   OBJ(OBJ_BROW_NAME_STR) = StringObject,
                    GA_ID,               OBJ_BROW_NAME_STR,
                    GA_RelVerify,        TRUE,
                    GA_TabCycle,         TRUE,
                    GA_HintInfo, getString(MSG_Edit_Name_Help),
             //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Name)),

                LAYOUT_AddChild,  HLayoutObject,
                    LAYOUT_SpaceInner,     FALSE,
                    LAYOUT_AddChild,       OBJ(OBJ_BROW_PATH_GET) = GetFileObject,
                        GA_ID,                 OBJ_BROW_PATH_GET,
                        GA_RelVerify,          TRUE,
                        GA_HintInfo, getString(MSG_Edit_Path_Help),
                        GETFILE_TitleText,     getString(MSG_ASL_Browser),//"Select Path To Browser",
                    End,  // GetFile
                    LAYOUT_AddChild,    OBJ(OBJ_BROW_PATH_CHOOSE) = ChooserObject,//ButtonObject,
                        GA_ID,              OBJ_BROW_PATH_CHOOSE,
                        GA_RelVerify,       TRUE,
                        //GA_Image,           &chooser_image,
                        GA_HintInfo, getString(MSG_Edit_Path_Help),
                        CHOOSER_DropDown, TRUE,
                        CHOOSER_Labels,   popup_www_ftp,
                    End,  // Button
                    CHILD_WeightedWidth, 0,
                End,   // HLayout
                Label(getString(MSG_Edit_Path)),

                LAYOUT_AddChild,  HLayoutObject,
                    LAYOUT_SpaceInner,  FALSE,
                    LAYOUT_AddChild,    OBJ(OBJ_BROW_AREXX_STR) = StringObject,
                        GA_ID,               OBJ_BROW_AREXX_STR,
                        GA_RelVerify,        TRUE,
                        GA_TabCycle,         TRUE,
                        GA_HintInfo, getString(MSG_Edit_Port_Help),
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_BROW_AREXX_CHOOSE) = ChooserObject,//ButtonObject,
                        GA_ID,              OBJ_BROW_AREXX_CHOOSE,
                        GA_RelVerify,       TRUE,
                        //GA_Image,           &chooser_image,
                        GA_HintInfo, getString(MSG_Edit_Port_Help),
                        CHOOSER_DropDown,      TRUE,
                        CHOOSER_Labels,        popup_arexxports,
                        CHOOSER_MaxLabels,     99,
                        CHOOSER_Justification, CHJ_CENTER,
                    End,  // Button
                    CHILD_WeightedWidth, 0,
                End,   // HLayout
                Label(getString(MSG_Edit_Port)),
            End,   // VLayout


            LAYOUT_AddChild,  OBJ(OBJ_BROW_ALIGN_B) = VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_BevelStyle,  BVS_GROUP,
                LAYOUT_Label,       getString(MSG_Edit_ARexx),
                LAYOUT_AlignLabels, OBJ(OBJ_BROW_ALIGN_T),  // align with the 4 labels above

                LAYOUT_AddChild,     OBJ(OBJ_BROW_SHOW_STR) = StringObject,
                    GA_ID,                 OBJ_BROW_SHOW_STR,
                    GA_RelVerify,          TRUE,
                    GA_TabCycle,           TRUE,
                    GA_HintInfo, getString(MSG_Edit_Show_Help),
         //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Show)),

                LAYOUT_AddChild,   OBJ(OBJ_BROW_FRONT_STR) = StringObject,
                    GA_ID,               OBJ_BROW_FRONT_STR,
                    GA_RelVerify,        TRUE,
                    GA_TabCycle,         TRUE,
                    GA_HintInfo, getString(MSG_Edit_Screen_Help),
             //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Screen)),

                LAYOUT_AddChild,    HLayoutObject,
                    LAYOUT_SpaceInner,  FALSE,
                    LAYOUT_AddChild,    OBJ(OBJ_BROW_OPEN_STR) = StringObject,
                        GA_ID,              OBJ_BROW_OPEN_STR,
                        GA_RelVerify,       TRUE,
                        GA_TabCycle,        TRUE,
                        GA_HintInfo, getString(MSG_Edit_OpenURL_Help),
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_BROW_OPEN_CHOOSE) = ChooserObject,//ButtonObject,
                            GA_ID,          OBJ_BROW_OPEN_CHOOSE,
                            GA_RelVerify,   TRUE,
                            //GA_Image,       &chooser_image,
                            GA_HintInfo, getString(MSG_Edit_OpenURL_Help),
                            CHOOSER_DropDown, TRUE,
                            CHOOSER_Labels,   popup_www_ftp,
                        End,  // Button
                        CHILD_WeightedWidth, 0,
                    End,   // HLayout
                Label(getString(MSG_Edit_OpenURL)),

                LAYOUT_AddChild,    HLayoutObject,
                    LAYOUT_SpaceInner,  FALSE,
                    LAYOUT_AddChild,    OBJ(OBJ_BROW_NEW_STR) = StringObject,
                        GA_ID,               OBJ_BROW_NEW_STR,
                        GA_RelVerify,        TRUE,
                        GA_TabCycle,         TRUE,
                        GA_HintInfo, getString(MSG_Edit_NewWin_Help),
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_BROW_NEW_CHOOSE) = ChooserObject,//ButtonObject,
                            GA_ID,          OBJ_BROW_NEW_CHOOSE,
                            GA_RelVerify,   TRUE,
                            //GA_Image,       &chooser_image,
                            GA_HintInfo, getString(MSG_Edit_NewWin_Help),
                            CHOOSER_DropDown, TRUE,
                            CHOOSER_Labels,   popup_www_ftp,
                        End,  // Button
                        CHILD_WeightedWidth, 0,
                End,   // HLayout
                Label(getString(MSG_Edit_NewWin)),  // space before `New` to give a better look
            End,   // VLayout


            LAYOUT_AddChild,    HLayoutObject,
                LAYOUT_AddChild,    SpaceObject,
                    SPACE_MinWidth,    2,
                End,  // Space
                CHILD_WeightedWidth,   0,

                LAYOUT_AddChild,     HLayoutObject,
                    LAYOUT_EvenSize,     TRUE,
                    LAYOUT_AddChild,     ButtonH(getString(MSG_Edit_Use),OBJ_BROW_USE,getString(MSG_Edit_Use_Help)),
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild, ButtonH(getString(MSG_Edit_Cancel),OBJ_BROW_CANCEL,getString(MSG_Edit_Cancel_Help)),
                    CHILD_WeightedWidth,   0,
                End,   // HLayout

                LAYOUT_AddChild,    SpaceObject,
                    SPACE_MinWidth,    2,
                End,  // Space
                CHILD_WeightedWidth,   0,
            End,   // HLayout
            CHILD_MinWidth, 300,  // sets a more attractive size for the whole Layout

        End,   // VLayout
    WindowEnd;
}

BOOL updateBrowserList(struct List * list, struct MinList PrefsBrowserList)
{
    struct URL_BrowserNode  *   bn      = NULL,
                            *   newNode = NULL;

    // libération de la liste
    freeList(list);

    // ajout des nouvelles données
    for (bn = (struct URL_BrowserNode *)PrefsBrowserList.mlh_Head;
         bn->ubn_Node.mln_Succ;
         bn = (struct URL_BrowserNode *)bn->ubn_Node.mln_Succ)
    {
        if((newNode = (struct URL_BrowserNode*)IListBrowser->AllocListBrowserNode(3,
            LBNA_NodeSize,  sizeof(struct URL_BrowserNode),
            LBNA_CheckBox,  TRUE,
            LBNA_Checked,   isFlagClear(bn->ubn_Flags, UNF_DISABLED),
            LBNA_Column, 1,
            LBNCA_Text, "",
            LBNA_Column, 2,
            LBNCA_Text, "",
            TAG_DONE)) != NULL)
        {
            IExec->CopyMem(bn, newNode, sizeof(struct URL_BrowserNode));
            IListBrowser->SetListBrowserNodeAttrs((struct Node*)newNode, LBNA_Column,    1,
                                                            LBNCA_Text,     newNode->ubn_Name,
                                                            LBNA_Column,    2,
                                                            LBNCA_Text,     newNode->ubn_Path,
                                                            TAG_END);
            IExec->AddTail(list, (struct Node *)newNode);
        }
        else
        {
            IDOS->Printf(" AllocListBrowserNode() failed\n");
            return(FALSE);
        }
    }

    return TRUE;
}

void updateBrowserWindow(struct URL_BrowserNode  * pBrowser)
{
    iset(edit_brow_win, WINDOW_UserData, pBrowser);

    if(pBrowser != NULL)
    {
        //iset(edit_brow_win, WINDOW_UserData, pBrowser);
        gadset(GAD(OBJ_BROW_NAME_STR),  edit_brow_window, STRINGA_TextVal,  pBrowser->ubn_Name);
        gadset(GAD(OBJ_BROW_PATH_GET),  edit_brow_window, GETFILE_FullFile, pBrowser->ubn_Path);
        gadset(GAD(OBJ_BROW_AREXX_STR), edit_brow_window, STRINGA_TextVal,  pBrowser->ubn_Port);
        gadset(GAD(OBJ_BROW_SHOW_STR),  edit_brow_window, STRINGA_TextVal,  pBrowser->ubn_ShowCmd);
        gadset(GAD(OBJ_BROW_FRONT_STR), edit_brow_window, STRINGA_TextVal,  pBrowser->ubn_ToFrontCmd);
        gadset(GAD(OBJ_BROW_OPEN_STR),  edit_brow_window, STRINGA_TextVal,  pBrowser->ubn_OpenURLCmd);
        gadset(GAD(OBJ_BROW_NEW_STR),   edit_brow_window, STRINGA_TextVal,  pBrowser->ubn_OpenURLWCmd);
    }
    else IDOS->Printf("No browser node\n");
}

void updateBrowserNode(void)
{
    struct URL_BrowserNode *pBrowser;

    if((pBrowser = (struct URL_BrowserNode *)iget(edit_brow_win, WINDOW_UserData)) != NULL)
    {
        STRPTR strValue;

        strValue = (STRPTR)iget(OBJ(OBJ_BROW_NAME_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pBrowser->ubn_Name, strValue, sizeof(pBrowser->ubn_Name));
        strValue = (STRPTR)iget(OBJ(OBJ_BROW_PATH_GET), GETFILE_FullFile);//GETFILE_File);
        IUtility->Strlcpy(pBrowser->ubn_Path, strValue, sizeof(pBrowser->ubn_Path));
        strValue = (STRPTR)iget(OBJ(OBJ_BROW_AREXX_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pBrowser->ubn_Port, strValue, sizeof(pBrowser->ubn_Port));
        strValue = (STRPTR)iget(OBJ(OBJ_BROW_SHOW_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pBrowser->ubn_ShowCmd, strValue, sizeof(pBrowser->ubn_ShowCmd));
        strValue = (STRPTR)iget(OBJ(OBJ_BROW_FRONT_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pBrowser->ubn_ToFrontCmd, strValue, sizeof(pBrowser->ubn_ToFrontCmd));
        strValue = (STRPTR)iget(OBJ(OBJ_BROW_OPEN_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pBrowser->ubn_OpenURLCmd, strValue, sizeof(pBrowser->ubn_OpenURLCmd));
        strValue = (STRPTR)iget(OBJ(OBJ_BROW_NEW_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pBrowser->ubn_OpenURLWCmd, strValue, sizeof(pBrowser->ubn_OpenURLWCmd));

        // now update the ListBrowser attributes
        IListBrowser->SetListBrowserNodeAttrs( (struct Node*)pBrowser,
                                              LBNA_Column, 1,
                                                  LBNCA_Text, pBrowser->ubn_Name,
                                              LBNA_Column, 2,
                                                  LBNCA_Text, pBrowser->ubn_Path,
                                             TAG_END);
    }
}
