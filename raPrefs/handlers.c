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

#include "handlers.h"
#include "browsers.h"
#include "ftps.h"
#include "mailers.h"
#include "gui_global.h"
#include "OpenURL.h"
//#include "macros.h"
#include "utility.h"

#include "version.h"

#include <libraries/openurl.h>

#include <classes/window.h>

#include <gadgets/chooser.h>
#include <gadgets/getfile.h>
#include <gadgets/listbrowser.h>
#include <gadgets/chooser.h>

#include <reaction/reaction_macros.h>
#include "my_reaction_macros.h"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/listbrowser.h>
#include <proto/chooser.h>


extern struct Window *window;
extern struct Window *edit_brow_window;
extern struct Window *edit_mail_window;
extern struct Window *edit_ftp_window;

extern struct List list_Brow;
extern struct List list_Mail;
extern struct List list_FTPs;
extern struct List *popup_www_ftp, *popup_mail, *popup_arexxports;

extern Object *Objects[OBJ_NUM];
extern Object *win;
extern Object *edit_brow_win;
extern Object *edit_mail_win;
extern Object *edit_ftp_win;

extern BOOL showhints;


/// HandleInput_Main_Win
BOOL HandleInput_Main_Win(void)
{
    uint32 result      = 0;
    uint16 code        = 0;
    BOOL   done        = FALSE;

    while ((result = RA_HandleInput(win, &code)))
    {
        switch(result & WMHI_CLASSMASK)
        {
            case WMHI_CLOSEWINDOW:
                done = TRUE;
                break;
            case WMHI_MENUPICK:
//IDOS->Printf("[WMHI_MENUPICK] code=0x%08lx\n",code);
                done = HandleMenu(code);
                break;
            case WMHI_GADGETUP:
                switch (result & WMHI_GADGETMASK)
                {
                    case OBJ_CLICKTAB: // code = page clicked, starts from 0
//IDOS->Printf("[OBJ_CLICKTAB] %ld\n",code);
                        if(code == 3) break; // no effect on MISC page
                        gadset(GAD(OBJ_LBROWSER_BROW+(code*7)), window, LISTBROWSER_AutoFit, TRUE); // "jump" 7 to next OBJ_LBROWSER_
                    break;
                    case OBJ_LBROWSER_BROW:
                    {
                        uint32 retval = iget(OBJ(OBJ_LBROWSER_BROW), LISTBROWSER_RelEvent);

                        switch(retval)
                        {
                            case LBRE_CHECKED:
                            case LBRE_UNCHECKED:
                            {
                                struct URL_BrowserNode *bn;

                                bn = (struct URL_BrowserNode *)iget(OBJ(OBJ_LBROWSER_BROW), LISTBROWSER_SelectedNode);
                                if (retval == LBRE_UNCHECKED)
                                    SET_FLAG(bn->ubn_Flags,UNF_DISABLED);
                                else
                                    CLEAR_FLAG(bn->ubn_Flags,UNF_DISABLED);
                            }
                        }
                        if (retval != LBRE_DOUBLECLICK) break;
                        // we deliberately go on executing following case OBJ_EDIT_BROW
                        // because a double click mean the same as clicking edit button
                    }
                    case OBJ_EDIT_BROW:
                    {
                        struct URL_BrowserNode *bn;

                        bn = (struct URL_BrowserNode *)iget(OBJ(OBJ_LBROWSER_BROW), LISTBROWSER_SelectedNode);
                        if(bn == NULL) break;

                        updateBrowserWindow(bn);
                        edit_brow_window = RA_OpenWindow(edit_brow_win);
                        break;
                    }
                    case OBJ_ADD_BROW:
                        {
                         struct URL_BrowserNode *bnew;

                         bnew = (struct URL_BrowserNode*)IListBrowser->AllocListBrowserNode(3,
                                   LBNA_NodeSize, sizeof(struct URL_BrowserNode),
                                   LBNA_CheckBox, TRUE,
                                   LBNA_Checked,  TRUE,//isFlagClear(bnew->ubn_Flags, UNF_DISABLED),
                                   LBNA_Column, 1,
                                       LBNCA_Text, "",
                                   LBNA_Column, 2,
                                       LBNCA_Text, "",
                                 TAG_DONE);
                         if(bnew != NULL)
                         {
                          bnew->ubn_Flags = UNF_NEW|UNF_NTALLOC;

                          IExec->AddTail(&list_Brow, (struct Node *)bnew);

                          updateBrowserWindow(bnew);
                          edit_brow_window = RA_OpenWindow(edit_brow_win);
                         }
                        }
                        break;
                    case OBJ_CLONE_BROW:
                        {
                         struct URL_BrowserNode *bn, *bnew;

                         bn = (struct URL_BrowserNode *)iget(OBJ(OBJ_LBROWSER_BROW), LISTBROWSER_SelectedNode);
                         if(bn == NULL) break;

                         bnew = (struct URL_BrowserNode*)IListBrowser->AllocListBrowserNode(3,
                                   LBNA_NodeSize, sizeof(struct URL_BrowserNode),
                                   LBNA_CheckBox, TRUE,
                                   LBNA_Checked,  TRUE,//isFlagClear(bn->ubn_Flags, UNF_DISABLED),
                                   LBNA_Column, 1,
                                       LBNCA_Text, bn->ubn_Name,
                                   LBNA_Column, 2,
                                       LBNCA_Text, bn->ubn_Path,
                                 TAG_DONE);

                         IExec->CopyMem( bn, bnew, sizeof(struct URL_BrowserNode) );
                         bnew->ubn_Flags = UNF_NEW|UNF_NTALLOC;//|UNF_DISABLED;

                         gadset(GAD(OBJ_LBROWSER_BROW), window, LISTBROWSER_Labels, ~0);
                         IExec->AddTail(&list_Brow, (struct Node *)bnew);
                         //updateBrowserNode();
                         gadset(GAD(OBJ_LBROWSER_BROW), window, LISTBROWSER_Labels, &list_Brow);
                        }
                        break;
                    case OBJ_UP_BROW:
                        moveUpEntry(&list_Brow, OBJ_LBROWSER_BROW);
                        break;
                    case OBJ_DOWN_BROW:
                        moveDownEntry(&list_Brow, OBJ_LBROWSER_BROW);
                        break;
                    case OBJ_DELETE_BROW:
                        deleteEntry(&list_Brow, OBJ_LBROWSER_BROW);
                        break;

                    case OBJ_LBROWSER_MAIL:
                    {
                        uint32 retval = iget(OBJ(OBJ_LBROWSER_MAIL), LISTBROWSER_RelEvent);

                        switch(retval)
                        {
                            case LBRE_CHECKED:
                            case LBRE_UNCHECKED:
                            {
                                struct URL_MailerNode *mn;

                                mn = (struct URL_MailerNode *)iget(OBJ(OBJ_LBROWSER_MAIL), LISTBROWSER_SelectedNode);
                                if (retval == LBRE_UNCHECKED)
                                    SET_FLAG(mn->umn_Flags,UNF_DISABLED);
                                else
                                    CLEAR_FLAG(mn->umn_Flags,UNF_DISABLED);
                            }
                        }
                        if (retval != LBRE_DOUBLECLICK) break;
                        // we deliberately go on executing following case OBJ_EDIT_MAIL
                        // because a double click mean the same as clicking edit button
                    }
                    case OBJ_EDIT_MAIL:
                    {
                        struct URL_MailerNode *mn;

                        mn = (struct URL_MailerNode *)iget(OBJ(OBJ_LBROWSER_MAIL), LISTBROWSER_SelectedNode);
                        if(mn == NULL) break;

                        updateMailerWindow(mn);
                        edit_mail_window = RA_OpenWindow(edit_mail_win);
                        break;
                    }
                    case OBJ_ADD_MAIL:
                        {
                         struct URL_MailerNode *mnew;

                         mnew = (struct URL_MailerNode*)IListBrowser->AllocListBrowserNode(3,
                                   LBNA_NodeSize, sizeof(struct URL_MailerNode),
                                   LBNA_CheckBox, TRUE,
                                   LBNA_Checked,  TRUE,//isFlagClear(mnew->umn_Flags, UNF_DISABLED),
                                   LBNA_Column, 1,
                                       LBNCA_Text, "",
                                   LBNA_Column, 2,
                                       LBNCA_Text, "",
                                 TAG_DONE);
                         if(mnew != NULL)
                         {
                          mnew->umn_Flags = UNF_NEW|UNF_NTALLOC;//|UNF_DISABLED;

                          IExec->AddTail(&list_Mail, (struct Node *)mnew);

                          updateMailerWindow(mnew);
                          edit_mail_window = RA_OpenWindow(edit_mail_win);
                         }
                        }
                        break;
                    case OBJ_CLONE_MAIL:
                        {
                         struct URL_MailerNode *mn, *mnew;

                         mn = (struct URL_MailerNode *)iget(OBJ(OBJ_LBROWSER_MAIL), LISTBROWSER_SelectedNode);
                         if(mn == NULL) break;

                         mnew = (struct URL_MailerNode*)IListBrowser->AllocListBrowserNode(3,
                                   LBNA_NodeSize, sizeof(struct URL_MailerNode),
                                   LBNA_CheckBox, TRUE,
                                   LBNA_Checked,  TRUE,//isFlagClear(mn->umn_Flags, UNF_DISABLED),
                                   LBNA_Column, 1,
                                       LBNCA_Text, mn->umn_Name,
                                   LBNA_Column, 2,
                                       LBNCA_Text, mn->umn_Path,
                                 TAG_DONE);

                         IExec->CopyMem( mn, mnew, sizeof(struct URL_MailerNode) );
                         mnew->umn_Flags = UNF_NEW|UNF_NTALLOC;//|UNF_DISABLED;

                         gadset(GAD(OBJ_LBROWSER_MAIL), window, LISTBROWSER_Labels, ~0);
                         IExec->AddTail(&list_Mail, (struct Node *)mnew);
                         //updateMailerNode();
                         gadset(GAD(OBJ_LBROWSER_MAIL), window, LISTBROWSER_Labels, &list_Mail);
                        }
                        break;
                    case OBJ_UP_MAIL:
                        moveUpEntry(&list_Mail, OBJ_LBROWSER_MAIL);
                        break;
                    case OBJ_DOWN_MAIL:
                        moveDownEntry(&list_Mail, OBJ_LBROWSER_MAIL);
                        break;
                    case OBJ_DELETE_MAIL:
                        deleteEntry(&list_Mail, OBJ_LBROWSER_MAIL);
                        break;

                    case OBJ_LBROWSER_FTP:
                    {
                        uint32 retval = iget(OBJ(OBJ_LBROWSER_FTP), LISTBROWSER_RelEvent);

                        switch(retval)
                        {
                            case LBRE_CHECKED:
                            case LBRE_UNCHECKED:
                            {
                                struct URL_FTPNode *fn;

                                fn = (struct URL_FTPNode *)iget(OBJ(OBJ_LBROWSER_FTP), LISTBROWSER_SelectedNode);
                                if (retval == LBRE_UNCHECKED)
                                    SET_FLAG(fn->ufn_Flags,UNF_DISABLED);
                                else
                                    CLEAR_FLAG(fn->ufn_Flags,UNF_DISABLED);
                            }
                        }
                        if (retval != LBRE_DOUBLECLICK) break;
                        // we deliberately go on executing following case OBJ_EDIT_FTP
                        //  because a double click mean the same as clicking edit button
                    }
                    case OBJ_EDIT_FTP:
                    {
                        struct URL_FTPNode *fn;

                        fn = (struct URL_FTPNode *)iget(OBJ(OBJ_LBROWSER_FTP), LISTBROWSER_SelectedNode);
                        if(fn == NULL) break;

                        updateFTPWindow(fn);
                        edit_ftp_window = RA_OpenWindow(edit_ftp_win);
                        break;
                    }
                    case OBJ_ADD_FTP:
                        {
                         struct URL_FTPNode *fnew;

                         fnew = (struct URL_FTPNode*)IListBrowser->AllocListBrowserNode(3,
                                   LBNA_NodeSize, sizeof(struct URL_FTPNode),
                                   LBNA_CheckBox, TRUE,
                                   LBNA_Checked,  TRUE,//isFlagClear(fnew->ufn_Flags, UNF_DISABLED),
                                   LBNA_Column, 1,
                                       LBNCA_Text, "",
                                   LBNA_Column, 2,
                                       LBNCA_Text, "",
                                 TAG_DONE);
                         if(fnew != NULL)
                         {
                          fnew->ufn_Flags = UNF_NEW|UNF_NTALLOC;//|UNF_DISABLED;

                          IExec->AddTail(&list_FTPs, (struct Node *)fnew);

                          updateFTPWindow(fnew);
                          edit_ftp_window = RA_OpenWindow(edit_ftp_win);
                         }
                        }
                        break;
                    case OBJ_CLONE_FTP:
                        {
                         struct URL_FTPNode *fn, *fnew;

                         fn = (struct URL_FTPNode *)iget(OBJ(OBJ_LBROWSER_FTP), LISTBROWSER_SelectedNode);
                         if(fn == NULL) break;

                         fnew = (struct URL_FTPNode*)IListBrowser->AllocListBrowserNode(3,
                                   LBNA_NodeSize, sizeof(struct URL_FTPNode),
                                   LBNA_CheckBox, TRUE,
                                   LBNA_Checked,  TRUE,//isFlagClear(fn->ufn_Flags, UNF_DISABLED),
                                   LBNA_Column, 1,
                                       LBNCA_Text, fn->ufn_Name,
                                   LBNA_Column, 2,
                                       LBNCA_Text, fn->ufn_Path,
                                 TAG_DONE);

                         IExec->CopyMem( fn, fnew, sizeof(struct URL_FTPNode) );
                         fnew->ufn_Flags = UNF_NEW|UNF_NTALLOC;//|UNF_DISABLED;

                         gadset(GAD(OBJ_LBROWSER_FTP), window, LISTBROWSER_Labels, ~0);
                         IExec->AddTail(&list_FTPs, (struct Node *)fnew);
                         //updateFTPNode();
                         gadset(GAD(OBJ_LBROWSER_FTP), window, LISTBROWSER_Labels, &list_FTPs);
                        }
                        break;
                    case OBJ_UP_FTP:
                        moveUpEntry(&list_FTPs, OBJ_LBROWSER_FTP);
                        break;
                    case OBJ_DOWN_FTP:
                        moveDownEntry(&list_FTPs, OBJ_LBROWSER_FTP);
                        break;
                    case OBJ_DELETE_FTP:
                        deleteEntry(&list_FTPs, OBJ_LBROWSER_FTP);
                        break;

                    case OBJ_USE:
                    case OBJ_SAVE:
                    case OBJ_APPLY:
                        storePrefs((result & WMHI_GADGETMASK)==OBJ_SAVE);

                        if((result & WMHI_GADGETMASK)!=OBJ_APPLY)
                            done=TRUE;
                        break;
                    case OBJ_CANCEL:
                        done=TRUE;
                        break;
                }
                break;
            case WMHI_ICONIFY:
                if (RA_Iconify(win))
                {
                    window = NULL;
                    if (edit_brow_window)
                    {
                        RA_CloseWindow(edit_brow_win);
                        edit_brow_window = NULL;
                    }
                    if (edit_mail_window)
                    {
                        RA_CloseWindow(edit_mail_win);
                        edit_mail_window = NULL;
                    }
                    if (edit_ftp_window)
                    {
                        RA_CloseWindow(edit_ftp_win);
                        edit_ftp_window = NULL;
                    }
                }
                break;
            case WMHI_UNICONIFY:
                window = RA_OpenWindow(win);
                break;
        }
    }
    return (done);
}
///

/// HandleInput_Edit_Brow_Win
void HandleInput_Edit_Brow_Win()
{
    uint32 result      = 0;
    uint16 code        = 0;

    while ((result = RA_HandleInput(edit_brow_win, &code)))
    {
        switch(result & WMHI_CLASSMASK)
        {
            case WMHI_CLOSEWINDOW:
                RA_CloseWindow(edit_brow_win);
                edit_brow_window = NULL;
                break;
            /*case WMHI_MENUPICK:
//IDOS->Printf("[WMHI_MENUPICK] code=0x%08lx\n",code);
                ///done =/// HandleMenu(code);
                break;*/
            case WMHI_GADGETUP:
                switch (result & WMHI_GADGETMASK)
                {
                    case OBJ_BROW_USE:
                        gadset(GAD(OBJ_LBROWSER_BROW), window, LISTBROWSER_Labels, ~0);
                        updateBrowserNode();
                        gadset(GAD(OBJ_LBROWSER_BROW), window, LISTBROWSER_Labels, &list_Brow,
                                                               LISTBROWSER_AutoFit, TRUE);
                        // and we close the window
                    case OBJ_BROW_CANCEL:
                        RA_CloseWindow(edit_brow_win);
                        edit_brow_window = NULL;
                        break;
                    case OBJ_BROW_PATH_GET:
                        gfRequestFile(OBJ(OBJ_BROW_PATH_GET), edit_brow_window);
                        /*if (gfRequestFile(OBJ(OBJ_BROW_PATH_GET), edit_brow_window))
                        {
                        }*/
                        break;
                    case OBJ_BROW_PATH_CHOOSE:
                    case OBJ_BROW_OPEN_CHOOSE:
                    case OBJ_BROW_NEW_CHOOSE:
                        {
                         STRPTR option;
                         char res_txt[256];
                         uint32 obj_ID = result & WMHI_GADGETMASK,
                                attrib = STRINGA_TextVal; // attrib -> STRINGA_TextVal or GETFILE_File
                         struct Node *res = (struct Node*)iget(OBJ(obj_ID), CHOOSER_SelectedNode);

                         IChooser->GetChooserNodeAttrs(res, CNA_Text,&option, TAG_DONE);

                         if(obj_ID == OBJ_BROW_PATH_CHOOSE) attrib = GETFILE_File;

                         // obj_ID is object we clicked; obj_ID-1 is "previous" object (if correctly defined in gui_global.h)
                         IUtility->Strlcpy( res_txt, (STRPTR)iget(OBJ(obj_ID-1),attrib), sizeof(res_txt) );

                         IUtility->SNPrintf(res_txt, sizeof(res_txt), "%s%%%lc",res_txt,*(option+1)); // add "%<option>" to existing string
                         gadset(GAD(obj_ID-1), edit_brow_window, attrib,res_txt);
                        }
                        break;
                    case OBJ_BROW_AREXX_CHOOSE:
                        {
                         STRPTR res_txt;
                         struct Node *res = (struct Node*)iget(OBJ(OBJ_BROW_AREXX_CHOOSE), CHOOSER_SelectedNode);

                         IChooser->GetChooserNodeAttrs(res, CNA_Text,&res_txt, TAG_DONE);

                         gadset(GAD(OBJ_BROW_AREXX_STR), edit_brow_window, STRINGA_TextVal,res_txt); // replace selected entry/port
                        }
                        break;
                }
        }
    }
}
///

/// HandleInput_Edit_Mail_Win
void HandleInput_Edit_Mail_Win()
{
    uint32 result      = 0;
    uint16 code        = 0;

    while ((result = RA_HandleInput(edit_mail_win, &code)))
    {
        switch(result & WMHI_CLASSMASK)
        {
            case WMHI_CLOSEWINDOW:
                RA_CloseWindow(edit_mail_win);
                edit_mail_window = NULL;
                break;
            case WMHI_GADGETUP:
                switch (result & WMHI_GADGETMASK)
                {
                    case OBJ_MAIL_USE:
                        gadset(GAD(OBJ_LBROWSER_MAIL), window, LISTBROWSER_Labels, ~0);
                        updateMailerNode();
                        gadset(GAD(OBJ_LBROWSER_MAIL), window, LISTBROWSER_Labels, &list_Mail,
                                                               LISTBROWSER_AutoFit, TRUE);
                        // and we close the window
                    case OBJ_MAIL_CANCEL:
                        RA_CloseWindow(edit_mail_win);
                        edit_mail_window = NULL;
                        break;
                    case OBJ_MAIL_PATH_GET:
                        gfRequestFile(OBJ(OBJ_MAIL_PATH_GET), edit_mail_window);
                        /*if (gfRequestFile(OBJ(OBJ_MAIL_PATH_GET), edit_mail_window))
                        {
                        }*/
                        break;
                    case OBJ_MAIL_PATH_CHOOSE:  // set Attrs according to the button clicked on.
                    case OBJ_MAIL_WRITE_CHOOSE:
                        {
                         STRPTR option;
                         char res_txt[256];
                         uint32 obj_ID = result & WMHI_GADGETMASK,
                                attrib = STRINGA_TextVal; // attrib -> STRINGA_TextVal or GETFILE_File
                         struct Node *res = (struct Node*)iget(OBJ(obj_ID), CHOOSER_SelectedNode);

                         IChooser->GetChooserNodeAttrs(res, CNA_Text,&option, TAG_DONE);

                         if(obj_ID == OBJ_MAIL_PATH_CHOOSE) attrib = GETFILE_File;

                         // obj_ID is object we clicked; obj_ID-1 is "previous" object (if correctly defined in gui_global.h)
                         IUtility->Strlcpy( res_txt, (STRPTR)iget(OBJ(obj_ID-1),attrib), sizeof(res_txt) );

                         IUtility->SNPrintf(res_txt, sizeof(res_txt), "%s%%%lc",res_txt,*(option+1)); // add "%<option>" to existing string
                         gadset(GAD(obj_ID-1), edit_mail_window, attrib,res_txt);
                        }
                        break;
                    case OBJ_MAIL_AREXX_CHOOSE:
                        {
                         STRPTR res_txt;
                         struct Node *res = (struct Node*)iget(OBJ(OBJ_MAIL_AREXX_CHOOSE), CHOOSER_SelectedNode);

                         IChooser->GetChooserNodeAttrs(res, CNA_Text,&res_txt, TAG_DONE);

                         gadset(GAD(OBJ_MAIL_AREXX_STR), edit_mail_window, STRINGA_TextVal,res_txt); // replace selected entry/port
                        }
                        break;
                }
        }
    }
}
///

/// HandleInput_Edit_FTP_Win
void HandleInput_Edit_FTP_Win()
{
    uint32 result      = 0;
    uint16 code        = 0;

    while ((result = RA_HandleInput(edit_ftp_win, &code)))
    {
        switch(result & WMHI_CLASSMASK)
        {
            case WMHI_CLOSEWINDOW:
                RA_CloseWindow(edit_ftp_win);
                edit_ftp_window = NULL;
                break;
            case WMHI_GADGETUP:
                switch (result & WMHI_GADGETMASK)
                {
                    case OBJ_FTP_USE:
                        gadset(GAD(OBJ_LBROWSER_FTP), window, LISTBROWSER_Labels, ~0);
                        updateFTPNode();
                        gadset(GAD(OBJ_LBROWSER_FTP), window, LISTBROWSER_Labels, &list_FTPs,
                                                              LISTBROWSER_AutoFit, TRUE);
                        // and we close the window
                    case OBJ_FTP_CANCEL:
                        RA_CloseWindow(edit_ftp_win);
                        edit_ftp_window = NULL;
                        break;
                    case OBJ_FTP_PATH_GET:
                        gfRequestFile(OBJ(OBJ_FTP_PATH_GET), edit_ftp_window);
                        /*if (gfRequestFile(OBJ(OBJ_FTP_PATH_GET), edit_ftp_window))
                        {
                        }*/
                        break;
                    case OBJ_FTP_PATH_CHOOSE:  // set Attrs according to the button clicked on.
                    case OBJ_FTP_OPEN_CHOOSE:
                    case OBJ_FTP_NEW_CHOOSE:
                        {
                         STRPTR option;
                         char res_txt[256];
                         uint32 obj_ID = result & WMHI_GADGETMASK,
                                attrib = STRINGA_TextVal; // attrib -> STRINGA_TextVal or GETFILE_File

                         struct Node *res = (struct Node*)iget(OBJ(obj_ID), CHOOSER_SelectedNode);

                         IChooser->GetChooserNodeAttrs(res, CNA_Text,&option, TAG_DONE);

                         if(obj_ID == OBJ_FTP_PATH_CHOOSE) attrib = GETFILE_File;

                         // obj_ID is object we clicked; obj_ID-1 is "previous" object (if correctly defined in gui_global.h)
                         IUtility->Strlcpy( res_txt, (STRPTR)iget(OBJ(obj_ID-1),attrib), sizeof(res_txt) );

                         IUtility->SNPrintf(res_txt, sizeof(res_txt), "%s%%%lc",res_txt,*(option+1)); // add "%<option>" to existing string
                         gadset(GAD(obj_ID-1), edit_ftp_window, attrib,res_txt);
                        }
                        break;
                    case OBJ_FTP_AREXX_CHOOSE:
                        {
                         STRPTR res_txt;
                         struct Node *res = (struct Node*)iget(OBJ(OBJ_FTP_AREXX_CHOOSE), CHOOSER_SelectedNode);

                         IChooser->GetChooserNodeAttrs(res, CNA_Text,&res_txt, TAG_DONE);

                         gadset(GAD(OBJ_FTP_AREXX_STR), edit_ftp_window, STRINGA_TextVal,res_txt); // replace selected entry/port
                        }
                        break;
                }
        }
    }
}
///

/// HandleMenu
ULONG HandleMenu(UNUSED uint16 selection)
{
	ULONG closeme = FALSE;
	uint32 item;

#ifndef MENUCLASS
	struct MenuItem *MItem = NULL;

	while( selection && (MItem=IIntuition->ItemAddress(MenuStrip, selection)) )
	{
		item = (uint32)GTMENUITEM_USERDATA(MItem);
		selection = MItem->NextSelect; // "queue" next menu opt selection
//IDOS->Printf("0x%08lx\n",item);
#else
	item = NO_MENU_ID;
	while( (item=IIntuition->IDoMethod(menustripobj, MM_NEXTSELECT, 0, item)) != NO_MENU_ID )
	{
#endif

		switch(item)
		{
			case MSG_Menu_About:
			{
				char buf[512];

				IUtility->SNPrintf( buf, sizeof(buf),
				                    "%s\n\nOpenURL " LIB_REV_STRING " [" SYSTEMSHORT "/" CPU "] (" LIB_DATE ")\n" LIB_COPYRIGHT
				                    ,getString(MSG_About_Descr) );
				RA_Request((Object *)window,getString(MSG_About_WinTitle),getString(MSG_About_OK),buf,NULL);
			}
			break;
			//case MSG_Menu_Hide:
			case MSG_Menu_Iconify:
				if (RA_Iconify(win))
				{
					window = NULL;
					if (edit_brow_window)
					{
						RA_CloseWindow(edit_brow_win);
						edit_brow_window = NULL;
					}
					if (edit_mail_window)
					{
						RA_CloseWindow(edit_mail_win);
						edit_mail_window = NULL;
					}
					if (edit_ftp_window)
					{
						RA_CloseWindow(edit_ftp_win);
						edit_ftp_window = NULL;
					}
				}
			break;
			case MSG_Menu_Quit:
				closeme = TRUE;
			break;
			case MSG_Menu_Save:
			case MSG_Menu_Use:
				storePrefs(item==MSG_Menu_Save);
				closeme=TRUE;
			break;
			case MSG_Menu_LastSaved:
			case MSG_Menu_Restore:
			case MSG_Menu_Defaults:
				loadPrefs(item);
			break;
			case MSG_Menu_Showhints:
				showhints = !showhints;
				iset(win, WINDOW_GadgetHelp,showhints); // main window
				iset(edit_brow_win, WINDOW_GadgetHelp,showhints); // browser edit window
				iset(edit_mail_win, WINDOW_GadgetHelp,showhints); // mailer edit window
				iset(edit_ftp_win,  WINDOW_GadgetHelp,showhints); // ftp edit window
			break;
		}
	}

	return closeme;
}
///

void deleteEntry(struct List *l, uint32 objID)
{
	struct Node *n = (struct Node *)iget(OBJ(objID), LISTBROWSER_SelectedNode);
	if(l==NULL  ||  n==NULL) return;

	gadset(GAD(objID), window, LISTBROWSER_Labels,~0);
	IExec->Remove(n);
	IListBrowser->FreeListBrowserNode(n);

	gadset(GAD(objID), window, LISTBROWSER_Labels,l, LISTBROWSER_AutoFit,TRUE,
	                           LISTBROWSER_Selected,-1);
}

void moveUpEntry(struct List *l, uint32 objID)
{
	struct Node *n, *pred;
	int32 pos;

	n = (struct Node *)iget(OBJ(objID), LISTBROWSER_SelectedNode);
	pos = (int32)iget(OBJ(objID), LISTBROWSER_Selected); // starts at 0

	if(l==NULL  ||  n==NULL  ||  pos==0) return;

	gadset(GAD(objID), window, LISTBROWSER_Labels,~0);

	pred = IExec->GetPred(n);

	// Move the PREDecessor/above entry below the selected/active entry
	IExec->Remove(pred);
	if( IExec->GetSucc(n) ) IExec->Insert(l, pred, n);
	else IExec->AddTail(l, pred); // N is penultimate entry

	switch(objID)
	{
		case OBJ_LBROWSER_BROW: updateBrowserNode(); break;
		case OBJ_LBROWSER_MAIL: updateMailerNode(); break;
		case OBJ_LBROWSER_FTP : updateFTPNode(); break;
	}

	gadset(GAD(objID), window, LISTBROWSER_Labels,l, LISTBROWSER_AutoFit,TRUE,
	                           LISTBROWSER_Selected,pos-1);
}

void moveDownEntry(struct List *l, uint32 objID)
{
	struct Node *n, *succ;
	int32 pos, tot;

	n = (struct Node *)iget(OBJ(objID), LISTBROWSER_SelectedNode);
	pos = (int32)iget(OBJ(objID), LISTBROWSER_Selected); // starts at 0
	tot = (int32)iget(OBJ(objID), LISTBROWSER_TotalNodes) - 1;

	if(l==NULL  ||  n==NULL  ||  pos==tot) return;

	gadset(GAD(objID), window, LISTBROWSER_Labels,~0);

	succ = IExec->GetSucc(n);

	// Move the selected/active entry below the SUCCesor/below entry
	IExec->Remove(n);
	if( IExec->GetSucc(succ) ) IExec->Insert(l, n, succ);
	else IExec->AddTail(l, n); // SUCC is last entry

	switch(objID)
	{
		case OBJ_LBROWSER_BROW: updateBrowserNode(); break;
		case OBJ_LBROWSER_MAIL: updateMailerNode(); break;
		case OBJ_LBROWSER_FTP : updateFTPNode(); break;
	}

	gadset(GAD(objID), window, LISTBROWSER_Labels,l, LISTBROWSER_AutoFit,TRUE,
	                           LISTBROWSER_Selected,pos+1);
}

