/***************************************************************************

 openurl.library - universal URL display and browser launcher library
 Copyright (C) 1998-2005 by Troels Walsted Hansen, et al.
 Copyright (C) 2005-2022 openurl.library Open Source Team

 This library is free software; it has been placed in the public domain
 and you can freely redistribute it and/or modify it. Please note, however,
 that some components may be under the LGPL or GPL license.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 openurl.library project: http://github.com/jens-maus/libopenurl

 $Id$

***************************************************************************/

#include "mailers.h"

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
extern Object *edit_mail_win;

extern struct List *popup_mail, *popup_arexxports;

extern struct MsgPort *AppPort;
//extern struct Hook idcmphook;
extern BOOL showhints;

struct Window *edit_mail_window;


Object * make_edit_mail_win(void)
{
    return WindowObject,
        WA_ScreenTitle,        "OpenURL " LIB_REV_STRING " (" LIB_DATE ")",//getString(MSG_App_ScreenTitle),
        WA_Title,              getString(MSG_Mailer_WinTitle),
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
        WINDOW_Layout,         VLayoutObject,
            //LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
            LAYOUT_SpaceOuter,  TRUE,

            LAYOUT_AddChild,  OBJ(OBJ_MAIL_ALIGN_T) = VLayoutObject,
                LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_BevelStyle,  BVS_GROUP,
                LAYOUT_Label,       getString(MSG_Edit_Definitions),
 
                LAYOUT_AddChild,   OBJ(OBJ_MAIL_NAME_STR) = StringObject,
                    GA_ID,               OBJ_MAIL_NAME_STR,
                    GA_RelVerify,        TRUE,
                    GA_TabCycle,         TRUE,
                    GA_HintInfo, getString(MSG_Edit_Name_Help),
             //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Name)),

                LAYOUT_AddChild,  HLayoutObject,
                    LAYOUT_SpaceInner,     FALSE,
                    LAYOUT_AddChild,       OBJ(OBJ_MAIL_PATH_GET) = GetFileObject,
                        GA_ID,                 OBJ_MAIL_PATH_GET,
                        GA_RelVerify,          TRUE,
                        GA_HintInfo, getString(MSG_Edit_Path_Help),
                        GETFILE_TitleText,     getString(MSG_ASL_Mailer),//"Select Path To Browser",
                    End,  // GetFile
                    LAYOUT_AddChild,    OBJ(OBJ_MAIL_PATH_CHOOSE) = ChooserObject,//ButtonObject,
                        GA_ID,              OBJ_MAIL_PATH_CHOOSE,
                        GA_RelVerify,       TRUE,
                        //GA_Image,           &chooser_image,
                        GA_HintInfo, getString(MSG_Edit_Path_Help),
                        CHOOSER_DropDown, TRUE,
                        CHOOSER_Labels,   popup_mail,
                    End,  // Button
                    CHILD_WeightedWidth, 0,
                End,   // HLayout
                Label(getString(MSG_Edit_Path)),

                LAYOUT_AddChild,  HLayoutObject,
                    LAYOUT_SpaceInner,  FALSE,
                    LAYOUT_AddChild,    OBJ(OBJ_MAIL_AREXX_STR) = StringObject,
                        GA_ID,               OBJ_MAIL_AREXX_STR,
                        GA_RelVerify,        TRUE,
                        GA_TabCycle,         TRUE,
                        GA_HintInfo, getString(MSG_Edit_Port_Help),
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_MAIL_AREXX_CHOOSE) = ChooserObject,//ButtonObject,
                        GA_ID,              OBJ_MAIL_AREXX_CHOOSE,
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
                Label(getString(MSG_Edit_Port)),   // space before `ARexx` to give a better look
            End,   // VLayout


            LAYOUT_AddChild,  OBJ(OBJ_MAIL_ALIGN_B) = VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_BevelStyle,  BVS_GROUP,
                LAYOUT_Label,       getString(MSG_Edit_ARexx),
                LAYOUT_AlignLabels, OBJ(OBJ_MAIL_ALIGN_T),  // align with the 4 labels above

                LAYOUT_AddChild,     OBJ(OBJ_MAIL_SHOW_STR) = StringObject,
                    GA_ID,                 OBJ_MAIL_SHOW_STR,
                    GA_RelVerify,          TRUE,
                    GA_TabCycle,           TRUE,
                    GA_HintInfo, getString(MSG_Edit_Screen_Help),
         //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Show)),

                LAYOUT_AddChild,   OBJ(OBJ_MAIL_FRONT_STR) = StringObject,
                    GA_ID,               OBJ_MAIL_FRONT_STR,
                    GA_RelVerify,        TRUE,
                    GA_TabCycle,         TRUE,
                    GA_HintInfo, getString(MSG_Edit_Screen_Help),
             //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Screen)),

                LAYOUT_AddChild,    HLayoutObject,
                    LAYOUT_SpaceInner,  FALSE,
                    LAYOUT_AddChild,    OBJ(OBJ_MAIL_WRITE_STR) = StringObject,
                        GA_ID,              OBJ_MAIL_WRITE_STR,
                        GA_RelVerify,       TRUE,
                        GA_TabCycle,        TRUE,
                        GA_HintInfo, getString(MSG_Mailer_Write_Help),
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_MAIL_WRITE_CHOOSE) = ChooserObject,//ButtonObject,
                            GA_ID,          OBJ_MAIL_WRITE_CHOOSE,
                            GA_RelVerify,   TRUE,
                            //GA_Image,       &chooser_image,
                            GA_HintInfo, getString(MSG_Mailer_Write_Help),
                            CHOOSER_DropDown, TRUE,
                            CHOOSER_Labels,   popup_mail,
                        End,  // Button
                        CHILD_WeightedWidth, 0,
                    End,   // HLayout
                Label(getString(MSG_Mailer_Write)),

            End,   // VLayout

            LAYOUT_AddChild,    HLayoutObject,
                LAYOUT_AddChild,    SpaceObject,
                    SPACE_MinWidth,    2,
                End,  // Space
                CHILD_WeightedWidth,   0,

                LAYOUT_AddChild,     HLayoutObject,
                    LAYOUT_EvenSize,     TRUE,
                    LAYOUT_AddChild,     ButtonH(getString(MSG_Edit_Use),OBJ_MAIL_USE,getString(MSG_Edit_Use_Help)),
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild, ButtonH(getString(MSG_Edit_Cancel),OBJ_MAIL_CANCEL,getString(MSG_Edit_Cancel_Help)),
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

BOOL updateMailerList(struct List * list, struct MinList PrefsMailerList)
{
    struct URL_MailerNode   *   mn      = NULL,
                            *   newNode = NULL;

    // lib�ration de la liste
    freeList(list);

    // ajout des nouvelles donn�es
    for (mn = (struct URL_MailerNode *)PrefsMailerList.mlh_Head;
         mn->umn_Node.mln_Succ;
         mn = (struct URL_MailerNode *)mn->umn_Node.mln_Succ)
    {
        if((newNode = (struct URL_MailerNode*)IListBrowser->AllocListBrowserNode(3,
            LBNA_NodeSize,  sizeof(struct URL_MailerNode),
            LBNA_CheckBox,  TRUE,
            LBNA_Checked,   isFlagClear(mn->umn_Flags, UNF_DISABLED),
            LBNA_Column,    1,
            LBNCA_Text, "",
            LBNA_Column, 2,
            LBNCA_Text, "",
            TAG_DONE)) != NULL)
        {
            IExec->CopyMem(mn, newNode, sizeof(struct URL_MailerNode));
            IListBrowser->SetListBrowserNodeAttrs((struct Node*)newNode, LBNA_Column,    1,
                                                            LBNCA_Text,     newNode->umn_Name,
                                                            LBNA_Column,    2,
                                                            LBNCA_Text,     newNode->umn_Path,
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

void updateMailerWindow(struct URL_MailerNode  * pMailer)
{
    iset(edit_mail_win, WINDOW_UserData, pMailer);

    if(pMailer != NULL)
    {
        //iset(edit_mail_win,  WINDOW_UserData, pMailer);
        gadset(GAD(OBJ_MAIL_NAME_STR), edit_mail_window, STRINGA_TextVal, pMailer->umn_Name);
        gadset(GAD(OBJ_MAIL_PATH_GET), edit_mail_window, GETFILE_File, pMailer->umn_Path);
        gadset(GAD(OBJ_MAIL_AREXX_STR), edit_mail_window, STRINGA_TextVal, pMailer->umn_Port);
        gadset(GAD(OBJ_MAIL_SHOW_STR), edit_mail_window, STRINGA_TextVal, pMailer->umn_ShowCmd);
        gadset(GAD(OBJ_MAIL_FRONT_STR), edit_mail_window, STRINGA_TextVal, pMailer->umn_ToFrontCmd);
        gadset(GAD(OBJ_MAIL_WRITE_STR), edit_mail_window, STRINGA_TextVal, pMailer->umn_WriteMailCmd);
    }
}

void updateMailerNode()
{
    struct URL_MailerNode *pMailer;

    if((pMailer = (struct URL_MailerNode *)iget(edit_mail_win, WINDOW_UserData)) != NULL)
    {
        STRPTR strValue;

        strValue = (STRPTR)iget(OBJ(OBJ_MAIL_NAME_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pMailer->umn_Name, strValue, sizeof(pMailer->umn_Name));
        strValue = (STRPTR)iget(OBJ(OBJ_MAIL_PATH_GET), GETFILE_File);
        IUtility->Strlcpy(pMailer->umn_Path, strValue, sizeof(pMailer->umn_Path));
        strValue = (STRPTR)iget(OBJ(OBJ_MAIL_AREXX_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pMailer->umn_Port, strValue, sizeof(pMailer->umn_Port));
        strValue = (STRPTR)iget(OBJ(OBJ_MAIL_SHOW_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pMailer->umn_ShowCmd, strValue, sizeof(pMailer->umn_ShowCmd));
        strValue = (STRPTR)iget(OBJ(OBJ_MAIL_FRONT_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pMailer->umn_ToFrontCmd, strValue, sizeof(pMailer->umn_ToFrontCmd));
        strValue = (STRPTR)iget(OBJ(OBJ_MAIL_WRITE_STR), STRINGA_TextVal);
        IUtility->Strlcpy(pMailer->umn_WriteMailCmd, strValue, sizeof(pMailer->umn_WriteMailCmd));

        // now update the ListBrowser attributes
        IListBrowser->SetListBrowserNodeAttrs( (struct Node*)pMailer,
                                              LBNA_Column, 1,
                                                  LBNCA_Text, pMailer->umn_Name,
                                              LBNA_Column, 2,
                                                  LBNCA_Text, pMailer->umn_Path,
                                             TAG_END);
    }
}
