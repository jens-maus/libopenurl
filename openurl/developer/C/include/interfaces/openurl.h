#ifndef OPENURL_INTERFACE_DEF_H
#define OPENURL_INTERFACE_DEF_H

/*
**  $VER: openurl.h 7.3 (28.12.2007)
**
**  This file defines interface C structure for OS4.
**
**  Written by Troels Walsted Hansen <troels@thule.no>
**  Placed in the public domain.
**
**  Versions 3.1 to 7.1 were developed by:
**  - Alfonso Ranieri <alforan@tin.it>
**  - Stefan Kost <ensonic@sonicpulse.de>
**
**  Copyright (C) 2006-2007 OpenURL Team
*/ 

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef EXEC_EXEC_H
#include <exec/exec.h>
#endif
#ifndef EXEC_INTERFACES_H
#include <exec/interfaces.h>
#endif

#include <rexx/storage.h>

struct OpenURLIFace
{
	struct InterfaceData Data;

	ULONG APICALL (*Obtain)(struct OpenURLIFace *Self);
	ULONG APICALL (*Release)(struct OpenURLIFace *Self);
	void APICALL (*Expunge)(struct OpenURLIFace *Self);
	struct Interface * APICALL (*Clone)(struct OpenURLIFace *Self);
	ULONG APICALL (*URL_OpenA)(struct OpenURLIFace *Self, STRPTR url, struct TagItem * tags);
	ULONG APICALL (*URL_Open)(struct OpenURLIFace *Self, STRPTR url, ...);
	void APICALL (*Reserved1)(struct OpenURLIFace *Self);
	void APICALL (*Reserved2)(struct OpenURLIFace *Self);
	void APICALL (*Reserved3)(struct OpenURLIFace *Self);
	void APICALL (*Reserved4)(struct OpenURLIFace *Self);
	void APICALL (*Reserved5)(struct OpenURLIFace *Self);
	void APICALL (*Reserved6)(struct OpenURLIFace * Self, struct RexxMsg *msg, STRPTR *resPtr);
	struct URL_Prefs * APICALL (*URL_GetPrefsA)(struct OpenURLIFace *Self, struct TagItem * tags);
	struct URL_Prefs * APICALL (*URL_GetPrefs)(struct OpenURLIFace *Self, ...);
	void APICALL (*URL_FreePrefsA)(struct OpenURLIFace *Self, struct URL_Prefs * prefs, struct TagItem * tags);
	void APICALL (*URL_FreePrefs)(struct OpenURLIFace *Self, struct URL_Prefs * prefs, ...);
	ULONG APICALL (*URL_SetPrefsA)(struct OpenURLIFace *Self, struct URL_Prefs * up, struct TagItem * tags);
	ULONG APICALL (*URL_SetPrefs)(struct OpenURLIFace *Self, struct URL_Prefs * up, ...);
	ULONG APICALL (*URL_LaunchPrefsAppA)(struct OpenURLIFace *Self, struct TagItem * tags);
	ULONG APICALL (*URL_LaunchPrefsApp)(struct OpenURLIFace *Self, ...);
	ULONG APICALL (*URL_GetAttr)(struct OpenURLIFace *Self, ULONG attr, ULONG * storage);
};

#endif /* OPENURL_INTERFACE_DEF_H */
