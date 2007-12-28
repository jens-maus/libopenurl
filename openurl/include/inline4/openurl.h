#ifndef INLINE4_OPENURL_H
#define INLINE4_OPENURL_H

/*
**  $VER: openurl.h 7.3 (28.12.2007)
**
**  This file provides compatibility to OS3 style
**  library calls by substituting functions under OS4.
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


/* Inline macros for Interface "main" */
#define URL_OpenA(url, tags) IOpenURL->URL_OpenA(url, tags) 
#if !defined(__cplusplus) && (__GNUC__ >= 3    \
    || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95) \
    || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#define URL_Open(...) IOpenURL->URL_Open(__VA_ARGS__) 
#endif
#define URL_GetPrefsA(tags) IOpenURL->URL_GetPrefsA(tags) 
#if !defined(__cplusplus) && (__GNUC__ >= 3    \
    || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95) \
    || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#define URL_GetPrefs(...) IOpenURL->URL_GetPrefs(__VA_ARGS__) 
#endif
#define URL_FreePrefsA(prefs, tags) IOpenURL->URL_FreePrefsA(prefs, tags) 
#if !defined(__cplusplus) && (__GNUC__ >= 3    \
    || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95) \
    || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#define URL_FreePrefs(...) IOpenURL->URL_FreePrefs(__VA_ARGS__) 
#endif
#define URL_SetPrefsA(up, tags) IOpenURL->URL_SetPrefsA(up, tags) 
#if !defined(__cplusplus) && (__GNUC__ >= 3    \
    || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95) \
    || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#define URL_SetPrefs(...) IOpenURL->URL_SetPrefs(__VA_ARGS__) 
#endif
#define URL_LaunchPrefsAppA(tags) IOpenURL->URL_LaunchPrefsAppA(tags) 
#if !defined(__cplusplus) && (__GNUC__ >= 3    \
    || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95) \
    || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#define URL_LaunchPrefsApp(...) IOpenURL->URL_LaunchPrefsApp(__VA_ARGS__) 
#endif
#define URL_GetAttr(attr, storage) IOpenURL->URL_GetAttr(attr, storage) 

#endif /* INLINE4_OPENURL_H */
