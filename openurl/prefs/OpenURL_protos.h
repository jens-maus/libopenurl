/*
**  OpenURL - MUI preferences for openurl.library
**
**  Written by Troels Walsted Hansen <troels@thule.no>
**  Placed in the public domain.
**
**  Developed by:
**  - Alfonso Ranieri <alforan@tin.it>
**  - Stefan Kost <ensonic@sonicpulse.de>
**
**  Ported to OS4 by Alexandre Balaban <alexandre@balaban.name>
**
*/

#if !defined(__amigaos4__)
#  undef VARARGS68K
#  define VARARGS68K
#endif /* !__amigaos4__ */

#ifdef __MORPHOS__
#  undef NewObject
#  undef MUI_NewObject
#  undef DoSuperNew
   APTR STDARGS NewObject ( struct IClass *classPtr , STRPTR classID , ...);
   APTR STDARGS MUI_NewObject ( char classID[] , ...);
#endif /* __MORPHOS__ */
APTR STDARGS DoSuperNew ( struct IClass *cl , Object *obj , ...) VARARGS68K;

/* loc.c */
void initStrings ( void );
void uninitStrings( void );
STRPTR getString ( ULONG id );
void localizeStrings ( STRPTR *s );
void localizeNewMenu ( struct NewMenu *nm );
ULONG getKeyChar ( UBYTE *string , ULONG id );

/* utils.c */
#if defined(__amigaos4__)
   int stccpy(char *dst, const char *src, int m);
   void SetAmiUpdateENVVariable( CONST_STRPTR varname );
#endif
ULONG xget ( Object *obj , ULONG attribute );
Object *olabel ( ULONG id );
Object *ollabel ( ULONG id );
Object *ollabel1 ( ULONG id );
Object *olabel1 ( ULONG id );
Object *olabel2 ( ULONG id );
Object *oflabel ( ULONG text );
Object *obutton ( ULONG text , ULONG help );
Object *oibutton ( ULONG spec , ULONG help );
Object *otbutton ( ULONG label , ULONG help );
Object *ocheckmark ( ULONG key , ULONG help );
Object *opopbutton ( ULONG img , ULONG help );
Object *ostring ( ULONG maxlen , ULONG key , ULONG help );
Object *opopport ( ULONG maxLen , ULONG key , ULONG help );
Object *opopph ( STRPTR *syms , STRPTR *names , ULONG maxLen , ULONG key , ULONG asl , ULONG help );
ULONG openWindow ( Object *app , Object *win );
ULONG delEntry ( Object *obj , APTR entry );
void STDARGS msprintf ( STRPTR to , STRPTR fmt , ... ) VARARGS68K;
int STDARGS msnprintf ( STRPTR buf , int size , STRPTR fmt , ... ) VARARGS68K;

/* ftpeditwin.c */
ULONG initFTPEditWinClass ( void );
void disposeFTPEditWinClass ( void );

/* mailereditwin.c */
ULONG initMailerEditWinClass ( void );
void disposeMailerEditWinClass ( void );

/* browsereditwin.c */
ULONG initBrowserEditWinClass ( void );
void disposeBrowserEditWinClass ( void );

/* applist.c */
ULONG initAppListClass ( void );
void disposeAppListClass ( void );

/* win.c */
ULONG initWinClass ( void );
void disposeWinClass ( void );

/* about.c */
ULONG initAboutClass ( void );
void disposeAboutClass ( void );

/* app.c */
ULONG initAppClass ( void );
void disposeAppClass ( void );

/* popport.c */
ULONG initPopportClass ( void );
void disposePopportClass ( void );

/* popph.c */
ULONG initPopphClass ( void );
void disposePopphClass ( void );

/* prefs.c */
ULONG initPensClass ( void );
void disposePensClass ( void );

/* about.c */
ULONG initAboutClass ( void );
void disposeAboutClass ( void );
