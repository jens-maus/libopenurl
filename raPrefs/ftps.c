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

#include "ftps.h"

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
extern Object *edit_ftp_win;

extern struct List *popup_www_ftp, *popup_arexxports;

extern struct MsgPort *AppPort;
//extern struct Hook idcmphook;
extern BOOL showhints;

struct Window *edit_ftp_window;


Object * make_edit_ftp_win(void)
{
    return WindowObject,
        WA_ScreenTitle,        "OpenURL " LIB_REV_STRING " (" LIB_DATE ")",//getString(MSG_App_ScreenTitle),
        WA_Title,              getString(MSG_FTP_WinTitle),
        WA_DragBar,            TRUE,
        WA_CloseGadget,        TRUE,
        WA_SizeGadget,         TRUE,
        WA_DepthGadget,        TRUE,
        WA_Activate,           TRUE,
   //     WINDOW_AppPort,        AppPort,
        WINDOW_SharedPort,     AppPort,
        //WINDOW_IDCMPHook,      &idcmphook,
        //WINDOW_IDCMPHookBits,  IDCMP_IDCMPUPDATE,
        WINDOW_GadgetHelp,     showhints,
        WINDOW_Position,       WPOS_CENTERSCREEN,
        WINDOW_LockHeight,     TRUE,
        WINDOW_Layout,         VLayoutObject,
            LAYOUT_SpaceOuter,  TRUE,
            LAYOUT_AddChild,  OBJ(OBJ_FTP_ALIGN_T) = VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_BevelStyle,  BVS_GROUP,
                LAYOUT_Label,       getString(MSG_Edit_Definitions),

                LAYOUT_AddChild,   OBJ(OBJ_FTP_NAME_STR) = StringObject,
                    GA_ID,               OBJ_FTP_NAME_STR,
                    GA_RelVerify,        TRUE,
                    GA_TabCycle,         TRUE,
                    GA_HintInfo, getString(MSG_Edit_Name_Help),
             //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Name)),

                LAYOUT_AddChild,  HLayoutObject,
                    LAYOUT_SpaceInner,     FALSE,
                    LAYOUT_AddChild,       OBJ(OBJ_FTP_PATH_GET) = GetFileObject,
                        GA_ID,                 OBJ_FTP_PATH_GET,
                        GA_RelVerify,          TRUE,
                        GA_HintInfo, getString(MSG_Edit_Path_Help),
                        GETFILE_TitleText,     getString(MSG_ASL_FTP),//"Select Path To Browser",
                    End,  // GetFile
                    LAYOUT_AddChild,    OBJ(OBJ_FTP_PATH_CHOOSE) = ChooserObject,///ButtonObject,
                        GA_ID,              OBJ_FTP_PATH_CHOOSE,
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
                    LAYOUT_AddChild,    OBJ(OBJ_FTP_AREXX_STR) = StringObject,
                        GA_ID,               OBJ_FTP_AREXX_STR,
                        GA_RelVerify,        TRUE,
                        GA_TabCycle,         TRUE,
                        GA_HintInfo, getString(MSG_Edit_Port_Help),
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_FTP_AREXX_CHOOSE) = ChooserObject,//ButtonObject,
                        GA_ID,              OBJ_FTP_AREXX_CHOOSE,
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

//                LAYOUT_AddChild,  HLayoutObject,
                    LAYOUT_AddChild,  OBJ(OBJ_FTP_REMOVE) = CheckBoxObject,
                        GA_ID,               OBJ_FTP_REMOVE,
                        GA_RelVerify,        TRUE,
                        GA_Selected,         FALSE,//TRUE,
                        GA_HintInfo, getString(MSG_FTP_RemoveURLQualifier_Help),
                        GA_Text,             getString(MSG_FTP_RemoveURLQualifier),
                    End,  // CheckBox
//                End,   // HLayout
//                Label(getString(MSG_FTP_RemoveURLQualifier)),
            End,   // VLayout


            LAYOUT_AddChild, OBJ(OBJ_FTP_ALIGN_B) = VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_BevelStyle,  BVS_GROUP,
                LAYOUT_Label,       getString(MSG_Edit_ARexx),
                LAYOUT_AlignLabels, OBJ(OBJ_FTP_ALIGN_T),  // align with the 4 labels above

                LAYOUT_AddChild,     OBJ(OBJ_FTP_SHOW_STR) = StringObject,
                    GA_ID,                 OBJ_FTP_SHOW_STR,
                    GA_RelVerify,          TRUE,
                    GA_TabCycle,           TRUE,
                    GA_HintInfo, getString(MSG_Edit_Show_Help),
         //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Show)),

                LAYOUT_AddChild,   OBJ(OBJ_FTP_FRONT_STR) = StringObject,
                    GA_ID,               OBJ_FTP_FRONT_STR,
                    GA_RelVerify,        TRUE,
                    GA_TabCycle,         TRUE,
                    GA_HintInfo, getString(MSG_Edit_Screen_Help),
             //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Screen)),

                LAYOUT_AddChild,    HLayoutObject,
                    LAYOUT_SpaceInner,  FALSE,
                    LAYOUT_AddChild,    OBJ(OBJ_FTP_OPEN_STR) = StringObject,
                        GA_ID,              OBJ_FTP_OPEN_STR,
                        GA_RelVerify,       TRUE,
                        GA_TabCycle,        TRUE,
                        GA_HintInfo, getString(MSG_Edit_OpenURL_Help),
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_FTP_OPEN_CHOOSE) = ChooserObject,//ButtonObject,
                            GA_ID,          OBJ_FTP_OPEN_CHOOSE,
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
                    LAYOUT_AddChild,    OBJ(OBJ_FTP_NEW_STR) = StringObject,
                        GA_ID,               OBJ_FTP_NEW_STR,
                        GA_RelVerify,        TRUE,
                        GA_TabCycle,         TRUE,
                        GA_HintInfo, getString(MSG_Edit_NewWin_Help),
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_FTP_NEW_CHOOSE) = ChooserObject,//ButtonObject,
                            GA_ID,          OBJ_FTP_NEW_CHOOSE,
                            GA_RelVerify,   TRUE,
                            //GA_Image,       &chooser_image,
                            GA_HintInfo, getString(MSG_Edit_NewWin_Help),
                            CHOOSER_DropDown, TRUE,
                            CHOOSER_Labels,   popup_www_ftp,
                        End,  // Button
                        CHILD_WeightedWidth, 0,
                End,   // HLayout
                Label(getString(MSG_Edit_NewWin)),
            End,   // VLayout

            LAYOUT_AddChild,    HLayoutObject,
                LAYOUT_AddChild,    SpaceObject,
                    SPACE_MinWidth,    2,
                End,  // Space
                CHILD_WeightedWidth,   0,

                LAYOUT_AddChild,     HLayoutObject,
                    LAYOUT_EvenSize,     TRUE,
                    LAYOUT_AddChild,     ButtonH(getString(MSG_Edit_Use),OBJ_FTP_USE,getString(MSG_Edit_Use_Help)),
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild, ButtonH(getString(MSG_Edit_Cancel),OBJ_FTP_CANCEL,getString(MSG_Edit_Cancel_Help)),
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

BOOL updateFTPList(struct List * list, struct MinList PrefsFTPList)
{
    struct URL_FTPNode  *   fn      = NULL,
                        *   newNode = NULL;

    // libération de la liste
    freeList(list);

    // ajout des nouvelles données
    for (fn = (struct URL_FTPNode *)PrefsFTPList.mlh_Head;
         fn->ufn_Node.mln_Succ;
         fn = (struct URL_FTPNode *)fn->ufn_Node.mln_Succ)
    {
        if((newNode = (struct URL_FTPNode*)IListBrowser->AllocListBrowserNode(3,
            LBNA_NodeSize,  sizeof(struct URL_FTPNode),
            LBNA_CheckBox,  TRUE,
            LBNA_Checked,   isFlagClear(fn->ufn_Flags, UNF_DISABLED),
            LBNA_Column, 1,
            LBNCA_Text, "",
            LBNA_Column, 2,
            LBNCA_Text, "",
            TAG_DONE)) != NULL)
        {
            IExec->CopyMem(fn, newNode, sizeof(struct URL_FTPNode));
            IListBrowser->SetListBrowserNodeAttrs((struct Node*)newNode, LBNA_Column,    1,
                                                            LBNCA_Text,     newNode->ufn_Name,
                                                            LBNA_Column,    2,
                                                            LBNCA_Text,     newNode->ufn_Path,
                                                            TAG_END);
            IExec->AddTail(list, (struct Node*)newNode);
        }
        else
        {
            IDOS->Printf(" AllocListBrowserNode() failed\n");
            return(FALSE);
        }
    }

    return TRUE;
}

void updateFTPWindow(struct URL_FTPNode  * pFTP)
{
    iset(edit_ftp_win, WINDOW_UserData, pFTP);

    if(pFTP != NULL)
    {
        //iset(edit_ftp_win,  WINDOW_UserData, pFTP);
        gadset(GAD(OBJ_FTP_NAME_STR), edit_ftp_window, STRINGA_TextVal, pFTP->ufn_Name);
        gadset(GAD(OBJ_FTP_PATH_GET), edit_ftp_window, GETFILE_File, pFTP->ufn_Path);
        gadset(GAD(OBJ_FTP_AREXX_STR), edit_ftp_window, STRINGA_TextVal, pFTP->ufn_Port);
        gadset(GAD(OBJ_FTP_REMOVE), edit_ftp_window, GA_Selected, isFlagSet(pFTP->ufn_Flags, UFNF_REMOVEFTP));
        gadset(GAD(OBJ_FTP_SHOW_STR), edit_ftp_window, STRINGA_TextVal, pFTP->ufn_ShowCmd);
        gadset(GAD(OBJ_FTP_FRONT_STR), edit_ftp_window, STRINGA_TextVal, pFTP->ufn_ToFrontCmd);
        gadset(GAD(OBJ_FTP_OPEN_STR), edit_ftp_window, STRINGA_TextVal, pFTP->ufn_OpenURLCmd);
        gadset(GAD(OBJ_FTP_NEW_STR), edit_ftp_window, STRINGA_TextVal, pFTP->ufn_OpenURLWCmd);
    }
}

void updateFTPNode(void)
{
    struct URL_FTPNode *pFTP;

    if((pFTP = (struct URL_FTPNode *)iget(edit_ftp_win, WINDOW_UserData)) != NULL)
    {
        STRPTR strValue;

        strValue = (STRPTR)iget(OBJ(OBJ_FTP_NAME_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pFTP->ufn_Name, strValue, sizeof(pFTP->ufn_Name));
        strValue = (STRPTR)iget(OBJ(OBJ_FTP_PATH_GET), GETFILE_File);
        IUtility->Strlcpy(pFTP->ufn_Path, strValue, sizeof(pFTP->ufn_Path));
        strValue = (STRPTR)iget(OBJ(OBJ_FTP_AREXX_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pFTP->ufn_Port, strValue, sizeof(pFTP->ufn_Port));
        if( iget(OBJ(OBJ_FTP_REMOVE),GA_Selected) ) SET_FLAG(pFTP->ufn_Flags, UFNF_REMOVEFTP);
        else CLEAR_FLAG(pFTP->ufn_Flags, UFNF_REMOVEFTP);
        strValue = (STRPTR)iget(OBJ(OBJ_FTP_SHOW_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pFTP->ufn_ShowCmd, strValue, sizeof(pFTP->ufn_ShowCmd));
        strValue = (STRPTR)iget(OBJ(OBJ_FTP_FRONT_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pFTP->ufn_ToFrontCmd, strValue, sizeof(pFTP->ufn_ToFrontCmd));
        strValue = (STRPTR)iget(OBJ(OBJ_FTP_OPEN_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pFTP->ufn_OpenURLCmd, strValue, sizeof(pFTP->ufn_OpenURLCmd));
        strValue = (STRPTR)iget(OBJ(OBJ_FTP_NEW_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pFTP->ufn_OpenURLWCmd, strValue, sizeof(pFTP->ufn_OpenURLWCmd));

        // now update the ListBrowser attributes
        IListBrowser->SetListBrowserNodeAttrs( (struct Node*)pFTP, 
                                              LBNA_Column, 1,
                                                  LBNCA_Text, pFTP->ufn_Name,
                                              LBNA_Column, 2,
                                                  LBNCA_Text, pFTP->ufn_Path,
                                             TAG_END);
    }
}
