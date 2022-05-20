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

#include <exec/exec.h>
#include <intuition/intuition.h>
#include <intuition/icclass.h>
#include <dos/dos.h>
#include <workbench/icon.h>

//#include <proto/gadtools.h>

#include <images/label.h>
//#include <images/glyph.h>
#include <images/bitmap.h>

#include <classes/window.h>
#include <classes/requester.h>

#include <gadgets/layout.h>
#include <gadgets/space.h>
#include <gadgets/button.h>
#include <gadgets/clicktab.h>
//#include <gadgets/texteditor.h>
//#include <gadgets/scroller.h>
#include <gadgets/checkbox.h>
#include <gadgets/listbrowser.h>
#include <gadgets/string.h>
#include <gadgets/getfile.h>
#include <gadgets/chooser.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/icon.h>

/*#include <proto/window.h>
#include <proto/requester.h>
#include <proto/layout.h>
#include <proto/space.h>
#include <proto/button.h>
#include <proto/clicktab.h>
//#include <proto/texteditor.h>
#include <proto/scroller.h>
#include <proto/checkbox.h>
#include <proto/listbrowser.h>
#include <proto/string.h>
#include <proto/getfile.h>
#include <proto/chooser.h>
#include <proto/label.h>
#include <proto/bitmap.h>*/
#include <proto/chooser.h>
#include <proto/layout.h>
#include <proto/listbrowser.h>

#include <reaction/reaction_macros.h>
#include "my_reaction_macros.h"

#include <libraries/openurl.h>
#include <proto/openurl.h>

#include "OpenURL.h"
#include "gui_global.h"
#include "browsers.h"
#include "ftps.h"
#include "handlers.h"
#include "mailers.h"
#include "utility.h"
//#include "macros.h"

#include "version.h"

static const char USED_VAR version[] = "$VER: OpenURL-Prefs " LIB_REV_STRING " [" SYSTEMSHORT "/" CPU "] (" LIB_DATE ") " LIB_COPYRIGHT;

struct ClassLibrary *WindowBase; Class *WindowClass;
struct Library *LayoutBase; Class *LayoutClass;
struct LayoutIFace *ILayout = NULL;
struct ClassLibrary *LabelBase; Class *LabelClass;
struct ClassLibrary *SpaceBase; Class *SpaceClass;
struct ClassLibrary *ButtonBase; Class *ButtonClass;
struct ClassLibrary *StringBase; Class *StringClass;
struct ClassLibrary *BitMapBase; Class *BitMapClass;
struct ClassLibrary *ClickTabBase; Class *ClickTabClass;
struct Library *ChooserBase; Class *ChooserClass;
struct ChooserIFace *IChooser = NULL;
struct ClassLibrary *GetFileBase; Class *GetFileClass;
struct ClassLibrary *CheckBoxBase; Class *CheckBoxClass;
struct Library *ListBrowserBase; Class *ListBrowserClass;
struct ListBrowserIFace *IListBrowser = NULL;
struct ClassLibrary *RequesterBase; Class *RequesterClass;

struct Library *UtilityBase = NULL;
struct Library *IntuitionBase = NULL;
struct Library *IconBase = NULL;
//struct Library *AslBase = NULL;
struct Library * OpenURLBase = NULL;

struct UtilityIFace *IUtility = NULL;
struct IntuitionIFace *IIntuition = NULL;
struct IconIFace *IIcon = NULL;
//struct AslIFace *IAsl = NULL;
struct OpenURLIFace *IOpenURL = NULL;


struct MsgPort *AppPort;
//struct Hook idcmphook;

struct Window *window;

struct List list_Brow;
struct List list_Mail;
struct List list_FTPs;
struct ColumnInfo *LB_ColInfo;

struct List *popup_www_ftp, *popup_mail, *popup_arexxports;

Object *win;
Object *Objects[OBJ_NUM];
Object *edit_brow_win, *edit_mail_win, *edit_ftp_win;

int32 mimgsize; // Menu images size
BOOL showhints; // Show hints on gadgets

static STRPTR PageLabels[] =
{
    (STRPTR)MSG_Win_Labels_Browsers,
    (STRPTR)MSG_Win_Labels_Mailers,
    (STRPTR)MSG_Win_Labels_FTPs,
    (STRPTR)MSG_Win_Labels_Misc,
    NULL
};

/*uint16 pageData[13] =
{
    // Plane 0
    0x3FF8,0x2008,0x2008,0x3FF8,0x2008,0x2008,0x2008,0x2008,
    0x2008,0x2008,0x2008,0x2008,0x3FF8
};

struct Image chooser_image =
{
    0, 0,           // LeftEdge, TopEdge
    16, 13, 8,      // Width, Height, Depth
    pageData,       // ImageData
    0x0001, 0x0000, // PlanePick, PlaneOnOff
    NULL            // NextImage
};*/

/**
**/
/*struct TagItem lst2btn[] = {
    { LISTBROWSER_SelectedNode, GA_ReadOnly },
    { TAG_END, 0 }
};*/


#ifdef MENUCLASS
	Object *menustripobj;

struct Image *MenuImage(CONST_STRPTR name, struct Screen *screen)
{
	struct Image *i = NULL;
	APTR prev_win;
	BPTR dir, prev_dir;
	STRPTR name_s, name_g;
	uint32 len, totlen;

	len = IUtility->Strlen(name);
	totlen = 2*len + 6; // [name_LENGHT + 3] + [name_LENGTH + 3] -> [name_s] + [name_g]

	name_s = IExec->AllocVecTags(totlen, TAG_END);
	if(name_s)
	{
		name_g = name_s + len + 3;

		IUtility->Strlcpy(name_s, name, totlen);
		IUtility->Strlcat(name_s, "_s", totlen);

		IUtility->Strlcpy(name_g, name, totlen);
		IUtility->Strlcat(name_g, "_g", totlen);

		prev_win = IDOS->SetProcWindow( (APTR)-1 ); // Disable requesters
		dir = IDOS->Lock("TBIMAGES:", SHARED_LOCK);
		IDOS->SetProcWindow(prev_win);              // Re-enable requesters
		if(dir != ZERO)
		{
			prev_dir = IDOS->SetCurrentDir(dir);

			i = (struct Image *)BitMapObject,
			                     (mimgsize!=24)? IA_Scalable : TAG_IGNORE, TRUE,
			                     //IA_Width,24, IA_Height,24+2,
			                     BITMAP_SourceFile,         name,
			                     BITMAP_SelectSourceFile,   name_s,
			                     BITMAP_DisabledSourceFile, name_g,
			                     BITMAP_Screen, screen,
			                     BITMAP_Masking, TRUE,
			                    TAG_END);
			if(i)
				IIntuition->SetAttrs( (Object *)i, IA_Height,mimgsize+2, IA_Width,mimgsize, TAG_END);
				//IIntuition->SetAttrs( (Object *)i, IA_Height,24+2, IA_Width,24, TAG_END);

			IDOS->SetCurrentDir(prev_dir);
			IDOS->UnLock(dir);
		}

		IExec->FreeVec(name_s);
	}

	return (i);
}

void CreateMainMenu(struct Screen *scr)
{
    menustripobj = IIntuition->NewObject(NULL, "menuclass",

     MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
      MA_Type,T_MENU, MA_Label,getString(MSG_Menu_Project),//"Project",
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
        MA_Type,T_ITEM, MA_Label,getString(MSG_Menu_Iconify),//"Iconify",
        MA_ID,    MSG_Menu_Iconify,// was MSG_Menu_Hide,
        //MA_Key,   "I",
        MA_Image, MenuImage("iconify",scr),
      TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
        MA_Type,T_ITEM, MA_Label,getString(MSG_Menu_About),//"About",
        MA_ID,    MSG_Menu_About,
        //MA_Key,   "?",
        MA_Image, MenuImage("info",scr),
      TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass", MA_Type,T_ITEM, MA_Separator,TRUE, TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
        MA_Type,T_ITEM, MA_Label,getString(MSG_Menu_Quit),//"Quit",
        MA_ID,    MSG_Menu_Quit,
        //MA_Key,   "Q",
        MA_Image, MenuImage("quit",scr),
      TAG_END),
     TAG_END),
     MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
      MA_Type,T_MENU, MA_Label,getString(MSG_Menu_Prefs),//"Preferences",
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
        MA_Type,T_ITEM, MA_Label,getString(MSG_Menu_Save),//"Save settings",
        MA_ID,    MSG_Menu_Save,
        //MA_Key,   "S",
        MA_Image, MenuImage("save",scr),
      TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
        MA_Type,T_ITEM, MA_Label,getString(MSG_Menu_Use),//"Use settings",
        MA_ID,    MSG_Menu_Use,
        //MA_Key,   "U",
        MA_Image, MenuImage("use",scr),
      TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass", MA_Type,T_ITEM, MA_Separator,TRUE, TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
        MA_Type,T_ITEM, MA_Label,getString(MSG_Menu_Defaults),//"Default settings",
        MA_ID,    MSG_Menu_Defaults,
        //MA_Key,   "D",
        MA_Image, MenuImage("restore",scr),
      TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
        MA_Type,T_ITEM, MA_Label,getString(MSG_Menu_LastSaved),//"Last saved",
        MA_ID,    MSG_Menu_LastSaved,
        //MA_Key,   "L",
        MA_Image, MenuImage("undo_reverttosaved",scr),
      TAG_END),
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
        MA_Type,T_ITEM, MA_Label,getString(MSG_Menu_Restore),//"Restore",
        MA_ID,    MSG_Menu_Restore,
        //MA_Key,   "R",
        MA_Image, MenuImage("undo",scr),
      TAG_END),
     TAG_END),
     MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
      MA_Type,T_MENU, MA_Label,getString(MSG_Menu_Help),//"Help",
      MA_AddChild, IIntuition->NewObject(NULL, "menuclass",
        MA_Type,T_ITEM, MA_Label,getString(MSG_Menu_Showhints),//"Show hints?",
        MA_ID,    MSG_Menu_Showhints,
        //MA_Key,   "H",
        MA_Image, MenuImage("helpbubble",scr),
        MA_Toggle,   TRUE,
        MA_Selected, showhints,
      TAG_END),
     TAG_END),

    TAG_END);
}
#else
	#define MyItem(i) Item((STRPTR)i,0,i)

	struct NewMenu menu[] =
	{
	    Title((STRPTR)MSG_Menu_Project),
	        MyItem(MSG_Menu_About),
	        ItemBar,
	        //MyItem(MSG_Menu_Hide),
	        MyItem(MSG_Menu_Iconify),
	        ItemBar,
	        MyItem(MSG_Menu_Quit),
	    Title((STRPTR)MSG_Menu_Prefs),
	        MyItem(MSG_Menu_Save),
	        MyItem(MSG_Menu_Use),
	        ItemBar,
	        MyItem(MSG_Menu_LastSaved),
	        MyItem(MSG_Menu_Restore),
	        MyItem(MSG_Menu_Defaults),
	    EndMenu
	};
	struct Menu *MenuStrip;
#endif


Object *make_window(void)
{
    Object
        *page1 = NULL,
        *page2 = NULL,
        *page3 = NULL,
        *page4 = NULL;

    struct DiskObject *iconify = NULL;

    // reset icon X/Y positions so it iconifies properly on Workbench
    if( (iconify=IIcon->GetIconTags("PROGDIR:OpenURL", ICONGETA_FailIfUnavailable,FALSE, TAG_END)) )
    {
     iconify->do_CurrentX = NO_ICON_POSITION;
     iconify->do_CurrentY = NO_ICON_POSITION;
    }

/*
    OBJ(OBJ_HIDDEN_CHOOSER) = ChooserObject,
        GA_ID,                  OBJ_HIDDEN_CHOOSER,
        GA_RelVerify,           TRUE,
          //CHOOSER_Labels,         &chooserlist,
        CHOOSER_DropDown,       TRUE,
        CHOOSER_AutoFit,        TRUE,
        CHOOSER_Hidden,         TRUE,
        ICA_TARGET,             ICTARGET_IDCMP,
    End;  // Chooser
*/

    page1 = VLayoutObject,
        LAYOUT_AddChild,    HLayoutObject,

//            LAYOUT_AddChild,    VLayoutObject,
//                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_AddChild, OBJ(OBJ_LBROWSER_BROW) = ListBrowserObject,
                    GA_ID,                      OBJ_LBROWSER_BROW,
           //         GA_Immediate,               TRUE,
                    GA_RelVerify,               TRUE,
                    GA_HintInfo, getString(MSG_Browser_List_Help),
                    LISTBROWSER_AutoFit,        TRUE,
                    LISTBROWSER_HorizontalProp, TRUE,
                    LISTBROWSER_ShowSelected,   TRUE,
                    LISTBROWSER_Labels,         &list_Brow,
                    LISTBROWSER_ColumnInfo,     LB_ColInfo,//&BrowsColInfo,
                    LISTBROWSER_ColumnTitles,   TRUE,
                End,  // ListBrowser
//            End,   // VLayout

            LAYOUT_AddChild,    VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                SPACE,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Add),OBJ_ADD_BROW,getString(MSG_AppList_Add_Help)),
                CHILD_WeightedHeight,   0,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Edit),OBJ_EDIT_BROW,getString(MSG_AppList_Edit_Help)),
                CHILD_WeightedHeight,   0,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Clone),OBJ_CLONE_BROW,getString(MSG_AppList_Clone_Help)),
                CHILD_WeightedHeight,   0,

                LAYOUT_AddChild,     HLayoutObject,
                    LAYOUT_AddChild,    OBJ(OBJ_UP_BROW) = ButtonObject,
                        GA_ID,              OBJ_UP_BROW,
                        GA_HintInfo, getString(MSG_AppList_MoveUp_Help),
                        BUTTON_AutoButton,  BAG_UPARROW,
                    End,  // Button
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild,    OBJ(OBJ_DOWN_BROW) = ButtonObject,
                        GA_ID,              OBJ_DOWN_BROW,
                        GA_HintInfo, getString(MSG_AppList_MoveDown_Help),
                        BUTTON_AutoButton,  BAG_DNARROW,
                    End,  // Button
                    CHILD_WeightedWidth,   0,
                End,   // HLayout
                CHILD_WeightedHeight,   0,

                SPACE,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Delete),OBJ_DELETE_BROW,getString(MSG_AppList_Delete_Help)),
                CHILD_WeightedHeight,   0,
                SPACE,
            End,   // VLayout
            CHILD_WeightedWidth,   0,

        End,   // HLayout
    End;  // VLayout         // *** end of page 1 ***


    page2 = VLayoutObject,
        LAYOUT_AddChild,    HLayoutObject,

//            LAYOUT_AddChild,    VLayoutObject,
//                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_AddChild, OBJ(OBJ_LBROWSER_MAIL) = ListBrowserObject,
                    GA_ID,                      OBJ_LBROWSER_MAIL,
           //         GA_Immediate,               TRUE,
                    GA_RelVerify,               TRUE,
                    GA_HintInfo, getString(MSG_Mailer_List_Help),
                    LISTBROWSER_AutoFit,        TRUE,
                    LISTBROWSER_HorizontalProp, TRUE,
                    LISTBROWSER_ShowSelected,   TRUE,
                    LISTBROWSER_Labels,         &list_Mail,
                    LISTBROWSER_ColumnInfo,     LB_ColInfo,//&MailColInfo,
                    LISTBROWSER_ColumnTitles,   TRUE,
                End,  // ListBrowser
//            End,   // VLayout

            LAYOUT_AddChild,    VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                SPACE,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Add),OBJ_ADD_MAIL,getString(MSG_AppList_Add_Help)),
                CHILD_WeightedHeight,   0,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Edit),OBJ_EDIT_MAIL,getString(MSG_AppList_Edit_Help)),
                CHILD_WeightedHeight,   0,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Clone),OBJ_CLONE_MAIL,getString(MSG_AppList_Clone_Help)),
                CHILD_WeightedHeight,   0,

                LAYOUT_AddChild,     HLayoutObject,
                    LAYOUT_AddChild,    OBJ(OBJ_UP_MAIL) = ButtonObject,
                        GA_ID,              OBJ_UP_MAIL,
                        GA_HintInfo, getString(MSG_AppList_MoveUp_Help),
                        BUTTON_AutoButton,  BAG_UPARROW,
                    End,  // Button
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild,    OBJ(OBJ_DOWN_MAIL) = ButtonObject,
                        GA_ID,              OBJ_DOWN_MAIL,
                        GA_HintInfo, getString(MSG_AppList_MoveDown_Help),
                        BUTTON_AutoButton,  BAG_DNARROW,
                    End,  // Button
                    CHILD_WeightedWidth,   0,
                End,   // HLayout
                CHILD_WeightedHeight,   0,

                SPACE,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Delete),OBJ_DELETE_MAIL,getString(MSG_AppList_Delete_Help)),
                CHILD_WeightedHeight,   0,
                SPACE,
            End,   // VLayout
            CHILD_WeightedWidth,   0,

        End,   // HLayout
    End;  // VLayout         // *** end of page 2 ***


    page3 = VLayoutObject,
        LAYOUT_AddChild,    HLayoutObject,

//            LAYOUT_AddChild,    VLayoutObject,
//                LAYOUT_SpaceOuter,  TRUE,
                LAYOUT_AddChild, OBJ(OBJ_LBROWSER_FTP) = ListBrowserObject,
                    GA_ID,                      OBJ_LBROWSER_FTP,
                    GA_RelVerify,               TRUE,
                    GA_HintInfo, getString(MSG_FTP_List_Help),
                    LISTBROWSER_AutoFit,        TRUE,
                    LISTBROWSER_HorizontalProp, TRUE,
                    LISTBROWSER_ShowSelected,   TRUE,
                    LISTBROWSER_Labels,         &list_FTPs,
                    LISTBROWSER_ColumnInfo,     LB_ColInfo,//&FTPsColInfo,
                    LISTBROWSER_ColumnTitles,   TRUE,
                End,  // ListBrowser
//            End,   // VLayout

            LAYOUT_AddChild,    VLayoutObject,
                LAYOUT_SpaceOuter,  TRUE,
                SPACE,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Add),OBJ_ADD_FTP,getString(MSG_AppList_Add_Help)),
                CHILD_WeightedHeight,   0,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Edit),OBJ_EDIT_FTP,getString(MSG_AppList_Edit_Help)),
                CHILD_WeightedHeight,   0,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Clone),OBJ_CLONE_FTP,getString(MSG_AppList_Clone_Help)),
                CHILD_WeightedHeight,   0,

                LAYOUT_AddChild,     HLayoutObject,
                    LAYOUT_AddChild,    OBJ(OBJ_UP_FTP) = ButtonObject,
                        GA_ID,              OBJ_UP_FTP,
                        GA_HintInfo, getString(MSG_AppList_MoveUp_Help),
                        BUTTON_AutoButton,  BAG_UPARROW,
                    End,  // Button
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild,    OBJ(OBJ_DOWN_FTP) = ButtonObject,
                        GA_ID,              OBJ_DOWN_FTP,
                        GA_HintInfo, getString(MSG_AppList_MoveDown_Help),
                        BUTTON_AutoButton,  BAG_DNARROW,
                    End,  // Button
                    CHILD_WeightedWidth,   0,
                End,   // HLayout
                CHILD_WeightedHeight,   0,

                SPACE,
                LAYOUT_AddChild, ButtonH(getString(MSG_AppList_Delete),OBJ_DELETE_FTP,getString(MSG_AppList_Delete_Help)),
                CHILD_WeightedHeight,   0,
                SPACE,
            End,   // VLayout
            CHILD_WeightedWidth,   0,

        End,   // HLayout
    End;  // VLayout     // *** end of page 3 ***


    page4 = VLayoutObject,

        LAYOUT_AddChild,  VLayoutObject,
            LAYOUT_BevelStyle,  BVS_GROUP,
            LAYOUT_Label,       getString(MSG_Misc_Defaults),

            LAYOUT_AddChild, OBJ(OBJ_UNICONIFY) = CheckBoxObject,
                GA_ID,               OBJ_UNICONIFY,
                GA_RelVerify,        TRUE,
                GA_Selected,         TRUE,
                GA_Text,             getString(MSG_Misc_Show),
                GA_HintInfo, getString(MSG_Misc_Show_Help),
            End,  // CheckBox

            LAYOUT_AddChild, OBJ(OBJ_BRING) = CheckBoxObject,
                GA_ID,               OBJ_BRING,
                GA_RelVerify,        TRUE,
                GA_Selected,         TRUE,
                GA_Text,             getString(MSG_Misc_Bring),
                GA_HintInfo, getString(MSG_Misc_Bring_Help),
            End,  // CheckBox

            LAYOUT_AddChild, OBJ(OBJ_OPEN) = CheckBoxObject,
                GA_ID,               OBJ_OPEN,
                GA_RelVerify,        TRUE,
                GA_Selected,         FALSE,
                GA_Text,             getString(MSG_Misc_Open),
                GA_HintInfo, getString(MSG_Misc_Open_Help),
            End,  // CheckBox

            LAYOUT_AddChild, OBJ(OBJ_LAUNCH) = CheckBoxObject,
                GA_ID,               OBJ_LAUNCH,
                GA_RelVerify,        TRUE,
                GA_Selected,         TRUE,
                GA_Text,             getString(MSG_Misc_Launch),
                GA_HintInfo, getString(MSG_Misc_Launch_Help),
            End,  // CheckBox
        End,  // VLayout

        LAYOUT_AddChild,  VLayoutObject,
            LAYOUT_BevelStyle,  BVS_GROUP,
            LAYOUT_Label,       getString(MSG_Misc_Options),

            LAYOUT_AddChild, OBJ(OBJ_PREPEND) = CheckBoxObject,
                GA_ID,               OBJ_PREPEND,
                GA_RelVerify,        TRUE,
                GA_Selected,         TRUE,
                GA_Text,             getString(MSG_Misc_Prepend),
                GA_HintInfo, getString(MSG_Misc_Prepend_Help),
            End,  // CheckBox

            LAYOUT_AddChild, OBJ(OBJ_SEND_MAILTO) = CheckBoxObject,
                GA_ID,               OBJ_SEND_MAILTO,
                GA_RelVerify,        TRUE,
                GA_Selected,         TRUE,
                GA_Text,             getString(MSG_Misc_UseMailer),
                GA_HintInfo, getString(MSG_Misc_UseMailer_Help),
            End,  // CheckBox

            LAYOUT_AddChild, OBJ(OBJ_SEND_FTP) = CheckBoxObject,
                GA_ID,               OBJ_SEND_FTP,
                GA_RelVerify,        TRUE,
                GA_Selected,         FALSE,
                GA_Text,             getString(MSG_Misc_UseFTP),
                GA_HintInfo, getString(MSG_Misc_UseFTP_Help),
            End,  // CheckBox

        End,  // VLayout
    End;  // VLayout      // *** end of page 4 ***


    OBJ(OBJ_CLICKTAB) = ClickTabObject,
        GA_Text,      PageLabels,
        GA_ID,        OBJ_CLICKTAB,
        GA_RelVerify, TRUE,
        CLICKTAB_Current,   0,  // page to open with
        CLICKTAB_PageGroup, PageObject,
            PAGE_Add, page1,
            PAGE_Add, page2,
            PAGE_Add, page3,
            PAGE_Add, page4,
        PageEnd,
    ClickTabEnd;

    return WindowObject,
        WA_ScreenTitle,        "OpenURL " LIB_REV_STRING " (" LIB_DATE ")",//getString(MSG_App_ScreenTitle),
        WA_Title,              getString(MSG_Win_WinTitle),
        WA_DragBar,            TRUE,
        WA_CloseGadget,        TRUE,
        WA_SizeGadget,         TRUE,
        WA_DepthGadget,        TRUE,
        WA_Activate,           TRUE,
        WINDOW_IconifyGadget,  TRUE,
        WINDOW_Icon,           iconify,//IIcon->GetDiskObject("PROGDIR:OpenURL"),
        //WINDOW_IconTitle,      getString(MSG_Win_WinTitle),
        WINDOW_AppPort,        AppPort,
        WINDOW_SharedPort,     AppPort,
        WINDOW_GadgetHelp,     showhints,
        WINDOW_Position,       WPOS_CENTERSCREEN,
#ifdef MENUCLASS
        WA_MenuStrip, menustripobj,
#else
        WINDOW_NewMenu, menu,
#endif
        WINDOW_Layout,         VLayoutObject,

            LAYOUT_AddChild,       OBJ(OBJ_CLICKTAB),

            LAYOUT_AddChild,        HLayoutObject,
                LAYOUT_BevelStyle,  BVS_SBAR_VERT,
            End,   // HLayout
            CHILD_WeightedHeight,   0,

            LAYOUT_AddChild,        HLayoutObject,
                LAYOUT_AddChild,    SpaceObject,
                    SPACE_MinWidth,    2,
                End,  // Space
                CHILD_WeightedWidth,   0,

                LAYOUT_AddChild,        HLayoutObject,
                    LAYOUT_EvenSize,    TRUE,
                    LAYOUT_AddChild, ButtonH(getString(MSG_Win_Save),OBJ_SAVE,getString(MSG_Win_Save_Help)),
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild, ButtonH(getString(MSG_Win_Use),OBJ_USE,getString(MSG_Win_Use_Help)),
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild, ButtonH(getString(MSG_Win_Apply),OBJ_APPLY,getString(MSG_Win_Apply_Help)),
                    CHILD_WeightedWidth,   0,

                    LAYOUT_AddChild, ButtonH(getString(MSG_Win_Cancel),OBJ_CANCEL,getString(MSG_Win_Cancel_Help)),
                    CHILD_WeightedWidth,   0,
                End,   // HLayout
                CHILD_WeightedHeight,   0,

                LAYOUT_AddChild,    SpaceObject,
                    SPACE_MinWidth,    2,
                End,  // Space
                CHILD_WeightedWidth,   0,

            End,   // HLayout
            CHILD_MinWidth, 350,  // sets a more attractive size for the whole Layout
            CHILD_WeightedHeight,   0,

        End,   // VLayout
    WindowEnd;
}


void createChooserList(struct List *popup_list, CONST_STRPTR popup_s, uint32 *popup_n)
{
	uint16 i;
	char temp_buf[64];
	struct Node *n;

	for(i=0; popup_s[i]!='\0'; ++i)
	{
		IUtility->SNPrintf(temp_buf, sizeof(temp_buf), "%%%lc - %s",popup_s[i],getString(popup_n[i]));
//IDOS->Printf("createChooserList: [%ld]'%s'\n",i,temp_buf);
		n = IChooser->AllocChooserNode(CNA_CopyText,TRUE, CNA_Text,temp_buf, TAG_DONE);

		IExec->AddTail(popup_list, n);
	}
}

void freeChooserList(struct List *list)
{
	struct Node *node, *nextnode;

	node = IExec->GetHead(list);
	while(node)
	{
//DBUG("freeChooserList 0x%08lx\n",node);
		nextnode = IExec->GetSucc(node);
		IChooser->FreeChooserNode(node);
		node = nextnode;
	}

	IExec->FreeSysObject(ASOT_LIST, list);
	list = NULL;
}


void createChooserARexxPorts(struct List *list)
{
	struct Node *n, *nd;
	struct Hook *prhook = NULL;
	//int32 count = 0;

	IExec->Forbid();
	for(nd=((struct ExecBase *)SysBase)->PortList.lh_Head; nd->ln_Succ; nd=nd->ln_Succ)
	{
//IDOS->Printf("portname: '%s'\n",nd->ln_Name);
		n = IChooser->AllocChooserNode(CNA_CopyText,TRUE, CNA_Text,nd->ln_Name, TAG_DONE);
		IExec->AddTail(list, n);
	}
	IExec->Permit();

	// Sort list by name
	prhook = (struct Hook *)IExec->AllocSysObjectTags(ASOT_HOOK,
	                                                  ASOHOOK_Entry, CompareNameNodes,
	                                                 TAG_DONE);
	sort_list(list, prhook);
	IExec->FreeSysObject(ASOT_HOOK, prhook);
}


int main(void)
{
	struct DiskObject *icon = NULL;

	if( !open_libs() )
	{
		close_libs();
		IDOS->PutErrStr("Can't open requiered libraries.\n");
		return(RETURN_FAIL);
	}

	if( !(OpenURLBase=IExec->OpenLibrary(OPENURLNAME, OPENURLVER)) )
	{
		close_libs();
		IDOS->PutErrStr("Can't open 'openurl.library'.\n");
		return(RETURN_FAIL);
	}
	if( !(IOpenURL=(struct OpenURLIFace*)IExec->GetInterface(OpenURLBase, "main", 1L, NULL)) )
	{
		IExec->CloseLibrary(OpenURLBase);
		close_libs();
		IDOS->PutErrStr("Can't get 'openurl.library' IFace.\n");
		return(RETURN_FAIL);
	}

	initStrings();

	localizeStrings(PageLabels);

	mimgsize = 24;
	showhints = FALSE;
	/* tooltypes - START */
	if( (icon=IIcon->GetDiskObjectNew("PROGDIR:OpenURL")) )
	{
		mimgsize = CFGInteger(icon, "MENUIMAGESIZE", 24);
		if(mimgsize < 1) mimgsize = 24;

		showhints = CFGBoolean(icon, "SHOWHINTS", FALSE);
	}
	/* tooltypes - END */

#ifndef MENUCLASS
	localizeNewMenu(menu);
#endif

    /*if(!(OpenURLBase = IExec->OpenLibrary(OPENURLNAME, OPENURLVER)))
         return -1;
    if(!(IOpenURL = (struct OpenURLIFace*)IExec->GetInterface(OpenURLBase, "main", 1L, NULL)))
        return -1;*/

	//RA_SetUpHook(idcmphook, IDCMPFunc, NULL);

	if((AppPort = IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE)) != NULL)
	{
		char syms[] = "asbfup";		// browser & ftp (aka &syms[4])
		uint32 names[] = {
			MSG_Mailer_PopAddress,
			MSG_Mailer_Popsubject,
			MSG_Mailer_PopBodyText,
			MSG_Mailer_PopBodyFile,
			MSG_Edit_PopURL,		//<- browser & ftp (aka names+4)
			MSG_Edit_PopScreen
		};

#ifdef MENUCLASS
		struct Screen *screen = IIntuition->LockPubScreen(NULL);
		CreateMainMenu(screen);
		IIntuition->UnlockPubScreen(NULL, screen);
#endif

		popup_www_ftp = IExec->AllocSysObjectTags(ASOT_LIST, TAG_DONE);
		popup_mail = IExec->AllocSysObjectTags(ASOT_LIST, TAG_DONE);
		createChooserList(popup_www_ftp, &syms[4], names+4); // "only" last 2 entries
		createChooserList(popup_mail, syms, names);

		popup_arexxports = IExec->AllocSysObjectTags(ASOT_LIST, TAG_DONE);
		createChooserARexxPorts(popup_arexxports);

		IExec->NewList(&list_Brow);
		IExec->NewList(&list_Mail);
		IExec->NewList(&list_FTPs);

		//Localize listbrowser's column titles
		LB_ColInfo = IListBrowser->AllocLBColumnInfo(3,
		                              LBCIA_Column, 0,
		                                   LBCIA_Title, getString(MSG_Edit_ListUse),
		                              LBCIA_Column, 1,
		                                   LBCIA_Title, getString(MSG_Edit_ListName),
		                                   LBCIA_DraggableSeparator, TRUE,
		                              LBCIA_Column, 2,
		                                   LBCIA_Title, getString(MSG_Edit_ListPath),
		                           TAG_DONE);

		win = make_window();

		edit_brow_win = make_edit_brow_win();
		edit_mail_win = make_edit_mail_win();
		edit_ftp_win = make_edit_ftp_win();

#ifndef MENUCLASS
		MenuStrip = IGadTools->CreateMenusA(menu, NULL);
#endif

		loadPrefs(URL_GetPrefs_Mode_InUse);

		// Set up inter-group label alignment
		iset( OBJ(OBJ_BROW_ALIGN_T), LAYOUT_AlignLabels,OBJ(OBJ_BROW_ALIGN_B) );
		iset( OBJ(OBJ_MAIL_ALIGN_T), LAYOUT_AlignLabels,OBJ(OBJ_MAIL_ALIGN_B) );
		iset( OBJ(OBJ_FTP_ALIGN_T), LAYOUT_AlignLabels,OBJ(OBJ_FTP_ALIGN_B) );

    /*iset(OBJ(OBJ_LBROWSER_FTP), ICA_TARGET, OBJ(OBJ_EDIT_FTP),
                                  ICA_MAP,    lst2btn);
      iset(OBJ(OBJ_LBROWSER_MAIL), ICA_TARGET, OBJ(OBJ_EDIT_MAIL),
                                   ICA_MAP,    lst2btn);
      iset(OBJ(OBJ_LBROWSER_BROW), ICA_TARGET, OBJ(OBJ_EDIT_BROW),
                                   ICA_MAP,    lst2btn);*/

		if((window = RA_OpenWindow(win)) != NULL)
		{
			uint32 sigmask;
			BOOL done = FALSE;

			sigmask = iget(win, WINDOW_SigMask);
			while (!done)
			{
				uint32 siggot = IExec->Wait(sigmask|SIGBREAKF_CTRL_C);

				if(siggot & SIGBREAKF_CTRL_C)
				{
					done = TRUE;
					break;
				}

				if (siggot & sigmask)
				{
					done = HandleInput_Main_Win();
					HandleInput_Edit_Brow_Win();
					HandleInput_Edit_Mail_Win();
					HandleInput_Edit_FTP_Win();
				}
			}
		}

		IIntuition->DisposeObject(edit_ftp_win);
		IIntuition->DisposeObject(edit_mail_win);
		IIntuition->DisposeObject(edit_brow_win);

		IIntuition->DisposeObject(win);

		freeChooserList(popup_arexxports);
		freeChooserList(popup_mail);
		freeChooserList(popup_www_ftp);

		// The hidden chooser isn't attached to anything,
		// so we must dispose of it ourselves...
		//IIntuition->DisposeObject(OBJ(OBJ_HIDDEN_CHOOSER));

#ifdef MENUCLASS
		if(menustripobj)
		{
			IIntuition->DisposeObject(menustripobj);
			menustripobj = NULL;
		}
#else
		IGadTools->FreeMenus(MenuStrip);
#endif

		IListBrowser->FreeLBColumnInfo(LB_ColInfo);

		IListBrowser->FreeListBrowserList(&list_FTPs);
		IListBrowser->FreeListBrowserList(&list_Mail);
		IListBrowser->FreeListBrowserList(&list_Brow);
 
		IExec->FreeSysObject(ASOT_PORT, AppPort);
	}

	IExec->DropInterface((struct Interface*)IOpenURL);
	IExec->CloseLibrary(OpenURLBase);

	uninitStrings();

	close_libs();

	return 0;
}

/*uint32 IDCMPFunc(UNUSED struct Hook *hook, UNUSED Object *wobj, struct IntuiMessage *Msg)
{
	//struct Window *w = Msg->IDCMPWindow;
	struct Node *nod_sel;

	if (Msg->Class == IDCMP_IDCMPUPDATE)
	{
		if (IUtility->GetTagData(GA_ID, 0, Msg->IAddress) == OBJ_HIDDEN_CHOOSER)
		{
			STRPTR label;
uint32 id_sel;
			nod_sel = (struct Node *)IUtility->GetTagData(CHOOSER_SelectedNode, -1, Msg->IAddress);
			id_sel = IUtility->GetTagData(GA_ID, 0, Msg->IAddress);
			IChooser->GetChooserNodeAttrs(nod_sel, CNA_Text,&label, TAG_END);
IDOS->Printf("chooser picked = 0x%08lx '%s' (0x%08lx)\n",nod_sel,label,id_sel);
			//find out which button was clicked, to replace OBJ_STRING with the correct String gad
			//gadset(GAD(), w, NULL, GA_Text,hidden_strings[active], TAG_END);
		}
	}

	return 0;
}*/

ULONG loadPrefs(ULONG mode)
{
    struct URL_Prefs       *p;
    ULONG                  error = 0;

    /* get the openurl.library prefs */
    switch(mode)
    {
        case URL_GetPrefs_Mode_InUse: mode = URL_GetPrefs_Mode_InUse;   break;
        case MSG_Menu_LastSaved     : mode = URL_GetPrefs_Mode_Envarc;  break;
        case MSG_Menu_Restore       : mode = URL_GetPrefs_Mode_Env;     break;
        case MSG_Menu_Defaults      : mode = URL_GetPrefs_Mode_Default; break;
        default: return FALSE;
    }

    p = IOpenURL->URL_GetPrefs(URL_GetPrefs_Mode,mode, TAG_DONE);
    if (!p) error = MSG_Err_NoPrefs;
    else if (p->up_Version!=PREFS_VERSION) error = MSG_Err_BadPrefs;

    if (error)
    {
        RA_Request(NULL,0,getString(MSG_ErrReqTitle),
                          getString(MSG_ErrReqGadget),
                          getString(error),
                          p ? p->up_Version : 0);

        if (p) IOpenURL->URL_FreePrefsA(p,NULL);

        return FALSE;
    }

    /* Browsers */
		gadset(GAD(OBJ_LBROWSER_BROW), window, LISTBROWSER_Labels, ~0);
		updateBrowserList(&list_Brow, p->up_BrowserList);
		gadset(GAD(OBJ_LBROWSER_BROW), window, LISTBROWSER_Labels, &list_Brow, LISTBROWSER_AutoFit, TRUE);

    /* Mailers */
    gadset(GAD(OBJ_LBROWSER_MAIL), window, LISTBROWSER_Labels, ~0, TAG_DONE);
    updateMailerList(&list_Mail, p->up_MailerList);
    gadset(GAD(OBJ_LBROWSER_MAIL), window, LISTBROWSER_Labels, &list_Mail, LISTBROWSER_AutoFit, TRUE);

    /* FTPs */
		gadset(GAD(OBJ_LBROWSER_FTP), window, LISTBROWSER_Labels, ~0, TAG_DONE);
		updateFTPList(&list_FTPs, p->up_FTPList);
		gadset(GAD(OBJ_LBROWSER_FTP), window, LISTBROWSER_Labels, &list_FTPs, LISTBROWSER_AutoFit, TRUE);

    /* Miscellaneous */
    gadset( GAD(OBJ_PREPEND),     window, GA_Selected, isFlagSet(p->up_Flags,UPF_PREPENDHTTP) );
    gadset( GAD(OBJ_SEND_MAILTO), window, GA_Selected, isFlagSet(p->up_Flags,UPF_DOMAILTO) );
    gadset( GAD(OBJ_SEND_FTP),    window, GA_Selected, isFlagSet(p->up_Flags,UPF_DOFTP) );

    gadset(GAD(OBJ_UNICONIFY), window, GA_Selected, p->up_DefShow);
    gadset(GAD(OBJ_BRING),     window, GA_Selected, p->up_DefBringToFront);
    gadset(GAD(OBJ_OPEN),      window, GA_Selected, p->up_DefNewWindow);
    gadset(GAD(OBJ_LAUNCH),    window, GA_Selected, p->up_DefLaunch);

    /* free the preferences */
    IOpenURL->URL_FreePrefsA(p,NULL);

    return TRUE;
}

ULONG storePrefs(BOOL bStorePrefs)
{
    struct URL_Prefs up;

    /* Copy settings from gadgets into structure */
    up.up_Version = PREFS_VERSION;

    /* Browsers */
    IExec->CopyMem(&list_Brow, &up.up_BrowserList, sizeof(struct MinList));

    /* Mailers */
    IExec->CopyMem(&list_Mail, &up.up_MailerList, sizeof(struct MinList));

    /* FTPs */
    IExec->CopyMem(&list_FTPs, &up.up_FTPList, sizeof(struct MinList));

    /* Miscellaneous */
    if(iget(OBJ(OBJ_PREPEND), GA_Selected))
        SET_FLAG(up.up_Flags, UPF_PREPENDHTTP);
    else
        CLEAR_FLAG(up.up_Flags, UPF_PREPENDHTTP);

    if(iget(OBJ(OBJ_SEND_MAILTO), GA_Selected))
        SET_FLAG(up.up_Flags, UPF_DOMAILTO);
    else
        CLEAR_FLAG(up.up_Flags, UPF_DOMAILTO);

    if(iget(OBJ(OBJ_SEND_FTP), GA_Selected))
        SET_FLAG(up.up_Flags, UPF_DOFTP);
    else
        CLEAR_FLAG(up.up_Flags, UPF_DOFTP);

    if(iget(OBJ(OBJ_SEND_MAILTO), GA_Selected))
        SET_FLAG(up.up_Flags, UPF_DOMAILTO);
    else
        CLEAR_FLAG(up.up_Flags, UPF_DOMAILTO);

    if(iget(OBJ(OBJ_SEND_FTP), GA_Selected))
        SET_FLAG(up.up_Flags, UPF_DOFTP);
    else
        CLEAR_FLAG(up.up_Flags, UPF_DOFTP);

    up.up_DefShow = iget(OBJ(OBJ_UNICONIFY), GA_Selected);
    up.up_DefBringToFront = iget(OBJ(OBJ_BRING), GA_Selected);
    up.up_DefNewWindow = iget(OBJ(OBJ_OPEN), GA_Selected);
    up.up_DefLaunch = iget(OBJ(OBJ_LAUNCH), GA_Selected);

    /* Save to disk */
    if (!IOpenURL->URL_SetPrefs(&up,URL_SetPrefs_Save,bStorePrefs,TAG_DONE))
        RA_Request((Object *)window,getString(MSG_ErrReqTitle),getString(MSG_ErrReqGadget),getString(MSG_Err_FailedSave),NULL);

    return TRUE;
}


BOOL myOpenLibrary(CONST_STRPTR libname, uint32 libversion, CONST_STRPTR ifacename, uint32 ifaceversion, struct Library **base, struct Interface **iface)
{
	if( !((*base)=IExec->OpenLibrary(libname, libversion)) )
	{
		IExec->DebugPrintF("Unable to open '%s' V%ld\n",libname,libversion);

		return FALSE;
	}

	if( !((*iface)=IExec->GetInterface((*base), ifacename, ifaceversion, NULL)) )
	{
		IExec->DebugPrintF("Unable to open interface %s V%ld of '%s'\n",ifacename,ifaceversion,libname);

		return FALSE;
	}

	return TRUE;
}

BOOL open_libs(void)
{
	BOOL res;

	res = myOpenLibrary("dos.library",50L, "main",1, &DOSBase, (struct Interface **)&IDOS);
	res &= myOpenLibrary("intuition.library",50L, "main",1, &IntuitionBase, (struct Interface **)&IIntuition);
	res &= myOpenLibrary("utility.library",50L, "main",1, &UtilityBase, (struct Interface **)&IUtility);
	res &= myOpenLibrary("icon.library",50L, "main",1, &IconBase, (struct Interface **)&IIcon);
	//res &= myOpenLibrary("asl.library",50L, "main",1, &AslBase, (struct Interface **)&IAsl);

	WindowBase = IIntuition->OpenClass("window.class", 0, &WindowClass);
	LabelBase  = IIntuition->OpenClass("images/label.image", 0, &LabelClass);
	BitMapBase = IIntuition->OpenClass("images/bitmap.image", 0, &BitMapClass);
	LayoutBase = (struct Library*)IIntuition->OpenClass("gadgets/layout.gadget", 0, &LayoutClass);
	SpaceBase   = IIntuition->OpenClass("gadgets/space.gadget",  0, &SpaceClass);
	ButtonBase  = IIntuition->OpenClass("gadgets/button.gadget", 0, &ButtonClass);
	StringBase  = IIntuition->OpenClass("gadgets/string.gadget", 0, &StringClass);
	GetFileBase = IIntuition->OpenClass("gadgets/getfile.gadget", 0, &GetFileClass);
	ChooserBase = (struct Library*)IIntuition->OpenClass("gadgets/chooser.gadget", 0, &ChooserClass);
	ClickTabBase  = IIntuition->OpenClass("gadgets/clicktab.gadget", 0, &ClickTabClass);
	CheckBoxBase  = IIntuition->OpenClass("gadgets/checkbox.gadget", 0, &CheckBoxClass);
	RequesterBase = IIntuition->OpenClass("requester.class", 0, &RequesterClass);
	ListBrowserBase = (struct Library*)IIntuition->OpenClass("gadgets/listbrowser.gadget", 41, &ListBrowserClass);

	if(!ListBrowserBase || !CheckBoxBase || !ClickTabBase || !ChooserBase || !GetFileBase
	   || !StringBase || !ButtonBase || !SpaceBase || !RequesterBase || !LayoutBase
	   || !BitMapBase || !LabelBase || !WindowBase) return(FALSE);

	ILayout      = (struct LayoutIFace *)IExec->GetInterface( (struct Library*)LayoutBase, "main", 1, NULL );
	IListBrowser = (struct ListBrowserIFace *)IExec->GetInterface( (struct Library*)ListBrowserBase, "main", 1, NULL );
	IChooser     = (struct ChooserIFace *)IExec->GetInterface( (struct Library*)ChooserBase, "main", 1, NULL );

	return res;
}

void close_libs(void)
{
	if(IListBrowser   ) IExec->DropInterface( (struct Interface *)IListBrowser );
	if(ListBrowserBase) IIntuition->CloseClass( (struct ClassLibrary*)ListBrowserBase );

	if(BitMapBase ) IIntuition->CloseClass(BitMapBase);
	if(GetFileBase) IIntuition->CloseClass(GetFileBase);

	if(IChooser   ) IExec->DropInterface( (struct Interface *)IChooser );
	if(ChooserBase) IIntuition->CloseClass( (struct ClassLibrary*)ChooserBase );

	if(ClickTabBase) IIntuition->CloseClass(ClickTabBase);
	if(CheckBoxBase) IIntuition->CloseClass(CheckBoxBase);
	if(StringBase  ) IIntuition->CloseClass(StringBase);
	if(ButtonBase  ) IIntuition->CloseClass(ButtonBase);
	if(SpaceBase   ) IIntuition->CloseClass(SpaceBase);
	if(LabelBase   ) IIntuition->CloseClass(LabelBase);

	if(ILayout   ) IExec->DropInterface( (struct Interface *)ILayout );
	if(LayoutBase) IIntuition->CloseClass( (struct ClassLibrary*)LayoutBase );

	if(RequesterBase) IIntuition->CloseClass(RequesterBase);
	if(WindowBase   ) IIntuition->CloseClass(WindowBase);

	//IExec->DropInterface( (struct Interface *)IAsl );
	//IExec->CloseLibrary(AslBase);
	IExec->DropInterface( (struct Interface *)IIcon );
	IExec->CloseLibrary(IconBase);
	IExec->DropInterface( (struct Interface *)IUtility );
	IExec->CloseLibrary(UtilityBase);
	IExec->DropInterface( (struct Interface *)IIntuition );
	IExec->CloseLibrary(IntuitionBase);
	IExec->DropInterface( (struct Interface *)IDOS );
	IExec->CloseLibrary(DOSBase);
}

/* sort_list() by Fredrik 'salas00' Wikstrom */
void sort_list(struct List *list, struct Hook *cmphook)
{
	struct Node *curr, *next;
	int32 j, i = 0, n = 0;

	for( curr=IExec->GetHead(list); curr; curr=IExec->GetSucc(curr) ) ++n;

	for(; i<n; ++i)
	{
		curr = IExec->GetHead(list);
		for(j=0; j<n-1-i; ++j)
		{
			next = IExec->GetSucc(curr);

			if( (int)IUtility->CallHookPkt(cmphook,curr,next) > 0 )
			{
				IExec->Remove(curr);

				IExec->Insert(list, curr, next);
			}
			else { curr = next; }
		}
	}
}


int CompareNameNodes(UNUSED struct Hook *hook, struct Node *node1, struct Node *node2)
{
	STRPTR name1, name2;

	IChooser->GetChooserNodeAttrs(node1, CNA_Text,&name1, TAG_DONE);
	IChooser->GetChooserNodeAttrs(node2, CNA_Text,&name2, TAG_DONE);

	return( IUtility->Stricmp(name1,name2) );
}
