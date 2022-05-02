
#undef BitMapObject
#define BitMapObject       IIntuition->NewObject(BitMapClass, NULL

#undef ChooserObject
#define ChooserObject      IIntuition->NewObject(ChooserClass, NULL

#undef VLayoutObject
#define VLayoutObject      IIntuition->NewObject(LayoutClass, NULL, \
                                                 LAYOUT_Orientation, LAYOUT_ORIENT_VERT

#undef HLayoutObject
#define HLayoutObject      IIntuition->NewObject(LayoutClass, NULL

#undef ListBrowserObject
#define ListBrowserObject  IIntuition->NewObject(ListBrowserClass, NULL

#undef SpaceObject
#define SpaceObject        IIntuition->NewObject(SpaceClass, NULL

#undef CheckBoxObject
#define CheckBoxObject     IIntuition->NewObject(CheckBoxClass, NULL

#undef ClickTabObject
#define ClickTabObject     IIntuition->NewObject(ClickTabClass, NULL

#undef PageObject
#define PageObject         IIntuition->NewObject(NULL, "page.gadget"

#undef WindowObject
#define WindowObject       IIntuition->NewObject(WindowClass, NULL

#undef ButtonObject
#define ButtonObject       IIntuition->NewObject(ButtonClass, NULL

#undef LabelObject
#define LabelObject        IIntuition->NewObject(LabelClass, NULL

#undef StringObject
#define StringObject       IIntuition->NewObject(StringClass, NULL

#undef GetFileObject
#define GetFileObject      IIntuition->NewObject(GetFileClass, NULL

#undef RequesterObject
#define RequesterObject    IIntuition->NewObject(RequesterClass, NULL


//#undef RA_OpenWindow
//#define RA_OpenWindow(wobj)        (struct Window *)IIntuition->IDoMethod(wobj, WM_OPEN, NULL)

//#undef RA_CloseWindow
//#define RA_CloseWindow(wobj)       IIntuition->IDoMethod(wobj, WM_CLOSE, NULL)

//#undef RA_HandleInput
//#define RA_HandleInput(wobj,code)  IIntuition->IDoMethod(wobj, WM_HANDLEINPUT, code)

//#undef RA_Iconify
//#define RA_Iconify(wobj)           IIntuition->IDoMethod(wobj, WM_ICONIFY, NULL)
