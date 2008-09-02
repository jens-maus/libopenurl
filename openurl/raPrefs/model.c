;/* RKMModel.c - A simple custom modelclass subclass.
 LC -cfist -b1 -y -v -j73 rkmmodel.c
quit ;*/
#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/cghooks.h>
#include <intuition/icclass.h>
#include <utility/tagitem.h>
#include <utility/hooks.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <clib/alib_protos.h>
#include <clib/alib_stdio_protos.h>
extern struct Library *IntuitionBase, *UtilityBase;
/*************************************************************************************************/
/****************  The attributes defined by this class  *****************************************/
/*************************************************************************************************/
#define OURL_Selected       (TAG_USER + 1)  /* This is the current selected node.*******/
#define OURL_Edited         (TAG_USER + 2)  /* This is the current edited node  *******/
#define OURL_HasSelection   (TAG_USER + 3)  /* This tells if there is a current selected node *******/
#define OURL_IsEditing      (TAG_USER + 4)  /* This tells if we are editing a node *******/

#define OM_EDIT         (TAG_USER + 100)/* This method is for editing a node   *******/
#define OM_VALID        (TAG_USER + 101)/* This method validate the current edited node   *******/
/*************************************************************************************************/
struct OURLModData
{
    struct Node *  m_pSelectedNode;
    struct Node *  m_pEditedNode;
};
/*************************************************************************************************/
/**************************     The functions in this module    ********************************/
/*************************************************************************************************/
Class *initOURLModClass(void);                                                   /***************/
BOOL freeOURLModClass(Class *);                                                  /***************/
ULONG dispatchOURLModel(Class *, Object *, Msg);                                 /***************/
void NotifySelected(Class *, Object *, struct opUpdate *, struct OURLModData *);  /***************/
void NotifyEdited(Class *, Object *, struct opUpdate *, struct OURLModData *);  /***************/
/*************************************************************************************************/

/*************************************************************************************************/
/******************************** Initialize the class **************************************/
/*************************************************************************************************/
Class *initOURLModClass(void) /* Make the class and set */
{ /* up the dispatcher’s hook. */
    Class *cl;
    if ( cl = MakeClass( NULL,
        "modelclass", NULL,
        sizeof ( struct OURLModData ),
        0 ))
    {
#ifdef UNUSED
        cl->cl_Dispatcher.h_Entry = HookEntry; /* initialize the */
        cl->cl_Dispatcher.h_SubEntry = dispatchOURLModel; /* cl_Dispatcher */
#endif
        cl->cl_Dispatcher.h_Entry = dispatchOURLModel;
    }
    return ( cl );
}
/*************************************************************************************************/
/********************************* Free the class ***************************************/
/*************************************************************************************************/
BOOL freeOURLModClass( Class *cl )
{
    return (FreeClass(cl));
}
/*************************************************************************************************/
/******************************** The class Dispatcher ***********************************/
/*************************************************************************************************/
ULONG dispatchOURLModel(Class *cl, Object *o, Msg msg)
{
    struct OURLModData *pObjectData;
    APTR retval = NULL; /* A generic return value used by this class’s methods. The */
                        /* meaning of this field depends on the method. For example,*/
                        /* OM_GET uses this a a boolean return value, while OM_NEW  */
                        /* uses it as a pointer to the new object.                  */

    switch (msg->MethodID)
    {
    case OM_NEW: /* Pass message onto superclass first so it can set aside the memory */
        /* for the object and take care of superclass instance data. */
        if (retval = (APTR)DoSuperMethodA(cl, o, msg))
        {   /************************************************************************/
            /* For the OM_NEW method, the object pointer passed to the dispatcher   */
            /* does not point to an object (how could it? The object doesn’t exist  */
            /* yet.) DoSuperMethodA() returns a pointer to a newly created          */
            /* object. INST_DATA() is a macro defined in <intuition/classes.h>      */
            /* that returns a pointer to the object’s instance data that is local   */
            /* to this class. For example, the instance data local to this class    */
            /* is the RKMModData structure defined above.                           */
            /************************************************************************/
            pObjectData = INST_DATA(cl, retval);
            pObjectData->m_pSelectedNode    = NULL; /* initialize object’s attributes. */
            pObjectData->m_pEditedNode      = NULL;
        }
        break;
    case OM_SET:
    case OM_UPDATE:
        pObjectData = INST_DATA(cl, o);
        DoSuperMethodA(cl, o, msg); /* Let the superclasses set their attributes first. */
        {
            struct TagItem *tstate, *ti; /* grab some temp variables off of the stack. */
            ti = ((struct opSet *)msg)->ops_AttrList;
            tstate = ti;
            /* Step through all of the attribute/value pairs in the list. Use the */
            /* utility.library tag functions to do this so they can properly process */
            /* special tag IDs like TAG_SKIP, TAG_IGNORE, etc. */
            while (ti = NextTagItem(&tstate)) /* Step through all of the attribute/value */
            { /* pairs in the list. Use the utility.library tag functions */
                /* to do this so they can properly process special tag IDs */
                /* like TAG_SKIP, TAG_IGNORE, etc. */
                switch (ti->ti_Tag)
                {
                case OURL_Selected:
                    pObjectData->m_pSelectedNode = (struct Node*)ti->ti_Data;
                    NotifySelected( cl, o, (struct opUpdate *)msg, pObjectData );
                    retval = (APTR)1L;  /* Changing OURL_Selected can cause a visual */
                    break;              /* change to gadgets in the rkmmodel’s broadcast */
                                        /* list. The rkmmodel has to tell the applica- */
                                        /* tion by returning a value besides zero. */
                case OURL_Edited:
                    pObjectData->m_pEditedNode = (struct Node*)ti->ti_Data;
                    NotifyEdited( cl, o, (struct opUpdate *)msg, pObjectData );
                    retval = (APTR)1L;  /* Changing OURL_Edited can cause a visual */
                    break;              /* change to gadgets in the rkmmodel’s broadcast */
                                        /* list. The rkmmodel has to tell the applica- */
                                        /* tion by returning a value besides zero. */
#ifdef UNUSED
                case RKMMOD_Down:
                    mmd->currval--;
                    /* Make sure currval didn’t go negative. */
                    if ((LONG)(mmd->currval) == -1L)
                        mmd->currval = 0L;
                    NotifyCurrVal(cl, o, msg, mmd);
                    retval = (APTR)1L; /* Changing RKMMOD_Down can cause a visual */
                    break; /* change to gadgets in the rkmmodel’s broadcast */
                    /* list. The rkmmodel has to tell the applica- */
                    /* tion by returning a value besides zero. */
                case RKMMOD_Limit:
                    mmd->vallimit = ti->ti_Data; /* Set the limit. Note that this does */
                    break; /* not do bounds checking on the */
                    /* current RKMModData.currval value. */
#endif
                }
            }
        }
        break;
    case OM_GET:                        /* The only attributes that are "gettable" in this */
        pObjectData = INST_DATA(cl, o); /* class or its superclasses are RKMMOD_CurrVal. */

        switch(((struct opGet *)msg)->opg_AttrID)
        {
            case OURL_Selected :
                *(((struct opGet *)msg)->opg_Storage) = (ULONG)(pObjectData->m_pSelectedNode);
                retval = (Object *)((pObjectData->m_pSelectedNode)?TRUE:FALSE);
                break;
            case OURL_Edited :
                *(((struct opGet *)msg)->opg_Storage) = (ULONG)(pObjectData->m_pEditedNode);
                retval = (Object *)((pObjectData->m_pSelectedNode)?TRUE:FALSE);
                break;
            case OURL_HasSelection :
                *(((struct opGet *)msg)->opg_Storage) = pObjectData->m_pSelectedNode?TRUE:FALSE;
                retval = (Object *)TRUE;
            case OURL_IsEditing :
                *(((struct opGet *)msg)->opg_Storage) = pObjectData->m_pEditedNode?TRUE:FALSE;
                retval = (Object *)TRUE;
            default:
                retval = (APTR)DoSuperMethodA(cl, o, msg);
                break;
        }
        break;
    case OM_EDIT: /* we want to edit the  node */
        if( )
        break;
    default:    /* our modelclass does not recognize the methodID, so let the superclass’s */
                /* dispatcher take a look at it. */
        retval = (APTR)DoSuperMethodA(cl, o, msg);
        break;
    }
    return((ULONG)retval);
}
void NotifySelected(Class *cl, Object *o, struct opUpdate *msg, struct OURLModData *pObjectData)
{
    struct TagItem tt[2];
    tt[0].ti_Tag = OURL_HasSelection; /* make a tag list. */
    tt[0].ti_Data = pObjectData->m_pSelectedNode?TRUE:FALSE;
    tt[1].ti_Tag = TAG_DONE;
                                    /* If the OURL_Selected changes, we want everyone to know about*/
    DoSuperMethod(cl, o,            /* it. Theoretically, the class is supposed to send itself a    */
                    OM_NOTIFY,      /* OM_NOTIFY message. Because this class lets its superclass    */
                    tt,             /* handle the OM_NOTIFY message, it skips the middleman and     */
                    msg->opu_GInfo, /* sends the OM_NOTIFY directly to its superclass. */
        ((msg->MethodID == OM_UPDATE) ? (msg->opu_Flags) : 0L)); /* If this is an OM_UPDATE */
    /* method, make sure the part the OM_UPDATE message adds to the */
    /* OM_SET message gets added. That lets the dispatcher handle */
} /* OM_UPDATE and OM_SET in the same case. */

void NotifyEdited(Class *cl, Object *o, struct opUpdate *msg, struct OURLModData *pObjectData)
{
    struct TagItem tt[2];
    tt[0].ti_Tag = OURL_IsEditing; /* make a tag list. */
    tt[0].ti_Data = pObjectData->m_pEditedNode?TRUE:FALSE;
    tt[1].ti_Tag = TAG_DONE;
                                    /* If the OURL_Edited changes, we want everyone to know about*/
    DoSuperMethod(cl, o,            /* it. Theoretically, the class is supposed to send itself a    */
                    OM_NOTIFY,      /* OM_NOTIFY message. Because this class lets its superclass    */
                    tt,             /* handle the OM_NOTIFY message, it skips the middleman and     */
                    msg->opu_GInfo, /* sends the OM_NOTIFY directly to its superclass. */
        ((msg->MethodID == OM_UPDATE) ? (msg->opu_Flags) : 0L)); /* If this is an OM_UPDATE */
    /* method, make sure the part the OM_UPDATE message adds to the */
    /* OM_SET message gets added. That lets the dispatcher handle */
} /* OM_UPDATE and OM_SET in the same case. */
