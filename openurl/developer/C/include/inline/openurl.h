#ifndef _INLINE_OPENURL_H
#define _INLINE_OPENURL_H

#ifndef CLIB_OPENURL_PROTOS_H
#define CLIB_OPENURL_PROTOS_H
#endif

#include <exec/types.h>

#ifndef OPENURL_BASE_NAME
#define OPENURL_BASE_NAME OpenURLBase
#endif

#define URL_OpenA(url, tags) \
	((ULONG (*)(STRPTR __asm("a0"), struct TagItem * __asm("a1"), struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 30))(url, tags, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_Open(url, tags...) \
	({ULONG _tags[] = {tags}; URL_OpenA((url), (struct TagItem *) _tags);})
#endif

#define URL_OldGetPrefs() \
	((struct URL_Prefs * (*)(struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 36))(OPENURL_BASE_NAME)

#define URL_OldFreePrefs(up) \
	((void (*)(struct URL_Prefs * __asm("a0"), struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 42))(up, OPENURL_BASE_NAME)

#define URL_OldSetPrefs(up, permanent) \
	((ULONG (*)(struct URL_Prefs * __asm("a0"), BOOL __asm("d0"), struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 48))(up, permanent, OPENURL_BASE_NAME)

#define URL_OldGetDefaultPrefs() \
	((struct URL_Prefs * (*)(struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 54))(OPENURL_BASE_NAME)

#define URL_OldLaunchPrefsApp() \
	((ULONG (*)(struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 60))(OPENURL_BASE_NAME)

#define URL_GetPrefsA(tags) \
	((struct URL_Prefs * (*)(struct TagItem * __asm("a0"), struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 72))(tags, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_GetPrefs(tags...) \
	({ULONG _tags[] = {tags}; URL_GetPrefsA((struct TagItem *) _tags);})
#endif

#define URL_FreePrefsA(prefs, tags) \
	((void (*)(struct URL_Prefs * __asm("a0"), struct TagItem * __asm("a1"), struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 78))(prefs, tags, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_FreePrefs(prefs, tags...) \
	({ULONG _tags[] = {tags}; URL_FreePrefsA((prefs), (struct TagItem *) _tags);})
#endif

#define URL_SetPrefsA(up, tags) \
	((ULONG (*)(struct URL_Prefs * __asm("a0"), struct TagItem * __asm("a1"), struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 84))(up, tags, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_SetPrefs(up, tags...) \
	({ULONG _tags[] = {tags}; URL_SetPrefsA((up), (struct TagItem *) _tags);})
#endif

#define URL_LaunchPrefsAppA(tags) \
	((ULONG (*)(struct TagItem * __asm("a0"), struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 90))(tags, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_LaunchPrefsApp(tags...) \
	({ULONG _tags[] = {tags}; URL_LaunchPrefsAppA((struct TagItem *) _tags);})
#endif

#define URL_GetAttr(attr, storage) \
	((ULONG (*)(ULONG __asm("d0"), ULONG * __asm("a0"), struct Library * __asm("a6"))) \
  (((char *) OPENURL_BASE_NAME) - 96))(attr, storage, OPENURL_BASE_NAME)

#endif /*  _INLINE_OPENURL_H  */
