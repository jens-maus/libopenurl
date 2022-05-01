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

#ifndef OPENURL_H
#define OPENURL_H


uint32 IDCMPFunc(UNUSED struct Hook *hook, UNUSED Object *wobj, struct IntuiMessage *Msg);
ULONG loadPrefs(ULONG mode);
ULONG storePrefs(BOOL bStorePrefs);
void updateFTPWindow(struct URL_FTPNode *pFTP);
BOOL open_libs(void);
void close_libs(void);
void createChooserList(struct List *, CONST_STRPTR popup_s, uint32 *popup_n);
void freeChooserList(struct List *);
void createChooserARexxPorts(struct List *);
void sort_list(struct List *, struct Hook *);
int CompareNameNodes(UNUSED struct Hook *, struct Node *, struct Node *);


#endif // OPENURL_H
