/***************************************************************************

 openurl.library - universal URL display and browser launcher library
 Copyright (C) 1998-2005 by Troels Walsted Hansen, et al.
 Copyright (C) 2005-2009 by openurl.library Open Source Team

 This library is free software; it has been placed in the public domain
 and you can freely redistribute it and/or modify it. Please note, however,
 that some components may be under the LGPL or GPL license.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 openurl.library project: http://sourceforge.net/projects/openurllib/

 $Id$

***************************************************************************/

#include "lib.h"

/***********************************************************************/

static ULONG
sendRexxMsg(struct MsgPort *reply,STRPTR rxport,STRPTR rxcmd)
{
    struct RexxMsg *rxmsg;

    if((rxmsg = CreateRexxMsg(reply,NULL,NULL)))
    {
        rxmsg->rm_Action = RXCOMM|RXFF_STRING|RXFF_NOIO;

        if((rxmsg->rm_Args[0] = CreateArgstring(rxcmd,strlen(rxcmd))))
        {
            struct MsgPort *port;

            Forbid();

            if((port = FindPort(rxport)))
            {
                PutMsg(port,(struct Message *)rxmsg);
                Permit();

                return TRUE;
            }

            Permit();

            DeleteArgstring(rxmsg->rm_Args[0]);
        }

        DeleteRexxMsg(rxmsg);
    }

    return FALSE;
}

/**************************************************************************/

#ifdef __MORPHOS__
void handler(void)
#else
void SAVEDS handler(void)
#endif
{
    struct MsgPort   port;
    struct Process   *me = (struct Process *)FindTask(NULL);
    struct startMsg  *smsg;
    ULONG            res;
    int              sig;

    WaitPort(&me->pr_MsgPort);
    smsg = (struct startMsg *)GetMsg(&me->pr_MsgPort);

    if ((sig = AllocSignal(-1))>=0)
    {
        INITPORT(&port,sig);
    res = sendRexxMsg(&port,smsg->port,smsg->cmd);
    }
    else res = FALSE;

    smsg->res = res;
    ReplyMsg((struct Message *)smsg);

    if (res)
    {
        struct RexxMsg *rxmsg;

        WaitPort(&port);
    rxmsg = (struct RexxMsg *)GetMsg(&port);

        DeleteArgstring(rxmsg->rm_Args[0]);
        DeleteRexxMsg(rxmsg);
    }

    if (sig>=0) FreeSignal(sig);

    Forbid();
    OpenURLBase->rexx_use--;
}

/**************************************************************************/

