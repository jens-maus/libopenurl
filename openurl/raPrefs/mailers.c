/*
**  RAOpenURL - Reaction preferences for openurl.library
**
**  Copyright (C) 2006 by OpenURL Team
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
** 
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
** 
**  Mailers utility functions
**
**  $Id: mailers.c,v 1.1 2008-09-02 23:24:26 abalabanb Exp $
**
*/

#include "mailers.h"

#include "gui_global.h"

#include <classes/window.h>
#include <libraries/openurl.h>

#include <reaction/reaction_macros.h>

#include <images/label.h>
#include <gadgets/space.h>

#include <gadgets/getfile.h>

#include <proto/exec.h>
#include <proto/label.h>
#include <proto/space.h>
#include <proto/layout.h>
#include <proto/window.h>
#include <proto/string.h>
#include <proto/getfile.h>
#include <proto/intuition.h>
#include <proto/listbrowser.h>

Object *edit_mail_win;
struct Window *edit_mail_window;

Object * make_edit_mail_win(void)
{
    return WindowObject,
        WA_ScreenTitle,        getString(MSG_App_ScreenTitle),
        WA_Title,              getString(MSG_Mailer_WinTitle),
        WA_DragBar,            TRUE,
        WA_CloseGadget,        TRUE,
        WA_SizeGadget,         TRUE,
        WA_DepthGadget,        TRUE,
        WA_Activate,           TRUE,
  //      WINDOW_AppPort,        AppPort,
        WINDOW_SharedPort,     AppPort,
        WINDOW_Position,       WPOS_CENTERSCREEN,
        WINDOW_LockHeight,     TRUE,
        WINDOW_Layout,         VLayoutObject,
            LAYOUT_SpaceOuter,  TRUE,
            LAYOUT_AddChild,  OBJ(OBJ_MAIL_ALIGN1) = VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_BevelStyle,  BVS_GROUP,
                LAYOUT_Label,       getString(MSG_Edit_Definitions),

                LAYOUT_AddChild,   OBJ(OBJ_MAIL_NAME_STR) = StringObject,
                    GA_ID,               OBJ_MAIL_NAME_STR,
                    GA_RelVerify,        TRUE,
                    GA_TabCycle,         TRUE,
             //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Name)),

                LAYOUT_AddChild,  HLayoutObject,
                    LAYOUT_SpaceInner,     FALSE,
                    LAYOUT_AddChild,       OBJ(OBJ_MAIL_PATH_GET) = GetFileObject,
                        GA_ID,                 OBJ_MAIL_PATH_GET,
                        GA_RelVerify,          TRUE,
          //              GETFILE_TitleText,     "Select Path To Browser",
                    End,  // GetFile
                    LAYOUT_AddChild,    OBJ(OBJ_MAIL_PATH_CHOOSE) = ButtonObject,
                        GA_ID,              OBJ_MAIL_PATH_CHOOSE,
                        GA_RelVerify,       TRUE,
                        GA_Image,           &chooser_image,
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
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_MAIL_AREXX_CHOOSE) = ButtonObject,
                        GA_ID,              OBJ_MAIL_AREXX_CHOOSE,
                        GA_RelVerify,       TRUE,
                        GA_Image,           &chooser_image,
                    End,  // Button
                    CHILD_WeightedWidth, 0,
                End,   // HLayout
                Label(getString(MSG_Edit_Port)),   // space before `ARexx` to give a better look
            End,   // VLayout


            LAYOUT_AddChild,  OBJ(OBJ_MAIL_ALIGN2) = VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_BevelStyle,  BVS_GROUP,
                LAYOUT_Label,       getString(MSG_Edit_ARexx),

                LAYOUT_AddChild,     OBJ(OBJ_MAIL_SHOW_STR) = StringObject,
                    GA_ID,                 OBJ_MAIL_SHOW_STR,
                    GA_RelVerify,          TRUE,
                    GA_TabCycle,           TRUE,
         //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Show)),

                LAYOUT_AddChild,   OBJ(OBJ_MAIL_FRONT_STR) = StringObject,
                    GA_ID,               OBJ_MAIL_FRONT_STR,
                    GA_RelVerify,        TRUE,
                    GA_TabCycle,         TRUE,
             //           STRINGA_Buffer,        buffer,
                End,  // String
                Label(getString(MSG_Edit_Screen)),

                LAYOUT_AddChild,    HLayoutObject,
                    LAYOUT_SpaceInner,  FALSE,
                    LAYOUT_AddChild,    OBJ(OBJ_MAIL_WRITE_STR) = StringObject,
                        GA_ID,              OBJ_MAIL_WRITE_STR,
                        GA_RelVerify,       TRUE,
                        GA_TabCycle,        TRUE,
             //           STRINGA_Buffer,        buffer,
                    End,  // String
                    LAYOUT_AddChild,    OBJ(OBJ_MAIL_WRITE_CHOOSE) = ButtonObject,
                            GA_ID,          OBJ_MAIL_WRITE_CHOOSE,
                            GA_RelVerify,   TRUE,
                            GA_Image,       &chooser_image,
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
                    LAYOUT_AddChild,     Button(getString(MSG_Edit_Use),OBJ_MAIL_USE),
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild, Button(getString(MSG_Edit_Cancel),OBJ_MAIL_CANCEL),
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

BOOL updateMailerList( struct List * list, struct MinList PrefsMailerList )
{
    struct URL_MailerNode   *   mn      = NULL,
                            *   newNode = NULL;
    struct Node             *   node    = NULL;

    // libération de la liste
    freeList( list );

    // ajout des nouvelles données
    for (mn = (struct URL_MailerNode *)PrefsMailerList.mlh_Head;
         mn->umn_Node.mln_Succ;
         mn = (struct URL_MailerNode *)mn->umn_Node.mln_Succ)
    {
        if (newNode = (struct URL_MailerNode*)IListBrowser->AllocListBrowserNode(3,
            LBNA_NodeSize,  sizeof(struct URL_MailerNode),
            LBNA_CheckBox,  TRUE,
            LBNA_Checked,   (mn->umn_Flags&UNF_DISABLED)?FALSE:TRUE,
            LBNA_Column,    1,
                LBNCA_Text, "",
            LBNA_Column, 2,
                LBNCA_Text, "",
            TAG_DONE))
        {
            IExec->CopyMem( mn, newNode, sizeof(struct URL_MailerNode ) );
            IListBrowser->SetListBrowserNodeAttrs( (struct Node*)newNode, LBNA_Column,    1,
                                                            LBNCA_Text,     newNode->umn_Name,
                                                            LBNA_Column,    2,
                                                            LBNCA_Text,     newNode->umn_Path,
                                                            TAG_END );

            IExec->AddTail(list, (struct Node*)newNode);
        }
        else
        {
            printf(" AllocListBrowserNode() failed\n");
            return(FALSE);
        }
    }

    return TRUE;
}

void updateMailerWindow( struct URL_MailerNode  * pMailer )
{
    if( pMailer )
    {
        IIntuition->SetGadgetAttrs(GAD(OBJ_MAIL_NAME_STR), edit_mail_window, NULL, STRINGA_TextVal, pMailer->umn_Name, TAG_DONE );
        IIntuition->SetGadgetAttrs(GAD(OBJ_MAIL_PATH_GET), edit_mail_window, NULL, GETFILE_File, pMailer->umn_Path, TAG_DONE );
        IIntuition->SetGadgetAttrs(GAD(OBJ_MAIL_AREXX_STR), edit_mail_window, NULL, STRINGA_TextVal, pMailer->umn_Port, TAG_DONE );
        IIntuition->SetGadgetAttrs(GAD(OBJ_MAIL_SHOW_STR), edit_mail_window, NULL, STRINGA_TextVal, pMailer->umn_ShowCmd, TAG_DONE );
        IIntuition->SetGadgetAttrs(GAD(OBJ_MAIL_FRONT_STR), edit_mail_window, NULL, STRINGA_TextVal, pMailer->umn_ToFrontCmd, TAG_DONE );
        IIntuition->SetGadgetAttrs(GAD(OBJ_MAIL_WRITE_STR), edit_mail_window, NULL, STRINGA_TextVal, pMailer->umn_WriteMailCmd, TAG_DONE );
    }
}


