/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Simple inter-process messaging system
 *
 * This module implements a common system for executing
 * a user defined action calling a hook function.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2005/02/09 21:48:30  bernie
 *#* Doxygen fixes.
 *#*
 *#* Revision 1.6  2005/01/22 04:20:26  bernie
 *#* Write extensive documentation; Add simple locking.
 *#*
 *#* Revision 1.5  2004/11/28 23:20:25  bernie
 *#* Remove obsolete INITLIST macro.
 *#*
 *#* Revision 1.4  2004/10/19 08:22:09  bernie
 *#* msg_peek(): New function.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/08/14 19:37:57  rasky
 *#* Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *#*
 *#* Revision 1.1  2004/06/06 15:11:08  bernie
 *#* Import into DevLib.
 *#*
 *#*/
#ifndef KERN_MSG_H
#define KERN_MSG_H

#include "event.h"
#include <mware/list.h>


/*!
 * Handle queues of messages associated an action.
 *
 * A message port is an abstraction used to exchange information
 * asynchronously between processes or other entities such as
 * interrupts and call-back functions.
 *
 * This form of IPC is higher-level than bare signals and
 * semaphores, because it sets a policy for exchanging
 * structured data with well-defined synchronization and
 * ownership semantics.
 *
 * Before using it, a message port must be initialized by
 * calling msg_initPort(), which associates the port with
 * an Event object, which can be setup to signal a process
 * or invoke a call-back hook.
 *
 * A process or interrupt routine can deliver messages to any
 * message port by calling msg_put().  By sending a message,
 * the sender temporarly or permanently transfers ownership
 * of its associated data to the receiver.
 *
 * Queuing a message to a port automatically triggers the
 * associated Event to notify the receiver.  When the
 * receiver wakes up, it usually invokes msg_get() to pick
 * the next message from the port.
 *
 * Message ports can hold any number of pending messages,
 * and receivers usually process them in FIFO order.
 * Other scheduling policies are possible, but not implemented
 * in this API.
 *
 * After the receiver has done processing a message, it replies
 * it back to the sender with msg_reply(), which transfer
 * ownership back to the original sender.  Replies are delivered
 * to a reply port, which is nothing more than another MsgPort
 * structure designated by the sender.
 *
 * Returning messages to senders is not mandatory, but it provides
 * a convenient way to provide some kind of result and simplify
 * the resource allocation scheme at the same time.
 *
 * When using signals to receive messages in a process, you
 * call sig_wait() in an event-loop to wake up when messages
 * are delivered to any of your ports.  When your process
 * wakes up with the port signal active, multiple messages
 * may already have queued up at the message port, and the
 * process must process them all before returning to sleep.
 * Signals don't keep a nesting count.
 *
 * A simple message loop works like this:
 *
 * \code
 *	// Our message port.
 *	static MsgPort test_port;
 *
 *	// A test message with two parameters and a result.
 *	typedef struct
 *	{
 *		Msg msg;
 *
 *		int x, y;
 *		int result;
 *	} TestMsg;
 *
 *
 *	// A process that sends two messages and waits for replies.
 *	static void sender_proc(void)
 *	{
 *		MsgPort test_reply_port;
 *		TestMsg msg1;
 *		TestMsg msg2;
 *		Msg *reply;
 *
 *		msg_initPort(&reply_port,
 *			event_createSignal(proc_current(), SIGF_SINGLE);
 *
 *		// Fill-in first message and send it out.
 *		msg1.x = 3;
 *		msg1.y = 2;
 *		msg1.msg.replyPort = &test_reply_port;
 *		msg_put(&test_port, &msg1);
 *
 *		// Fill-in second message and send it out too.
 *		msg2.x = 5;
 *		msg2.y = 4;
 *		msg2.msg.replyPort = &test_reply_port;
 *		msg_put(&test_port, &msg1);
 *
 *		// Wait for a reply...
 *		sig_wait(SIG_SINGLE);
 *
 *		reply = (TestMsg *)msg_get(&test_reply_port);
 *		ASSERT(reply != NULL);
 *		ASSERT(reply->result == 5);
 *
 *		// Get reply to second message.
 *		while (!(reply = (TestMsg *)msg_get(&test_reply_port))
 *		{
 *			// Not yet, be patient and wait some more.
 *			sig_wait(SIG_SINGLE);
 *		}
 *
 *		ASSERT(reply->result == 9);
 *	}
 *
 *
 *	// Receive messages and do something boring with them.
 *	static void receiver_proc(void)
 *	{
 *		msg_initPort(&test_port,
 *			event_createSignal(proc_current(), SIGF_EXAMPLE);
 *
 *		proc_new(sender_proc, (iptr_t)&test_port,
 *			sender_stack, sizeof(sender_stack);
 *
 *		for (;;)
 *		{
 *			sigmask_t sigs = sig_wait(SIGF_EXAMPLE | more_signals);
 *
 *			if (sigs & SIGF_EXAMPLE)
 *			{
 *				TestMsg *emsg;
 *				while (emsg = (TestMsg *)msg_get(&test_port)
 *				{
 *					// Do something with the message
 *					emsg->result = emsg->x + emsg->y;
 *					msg_reply((Msg *)msg);
 *				}
 *			}
 *		}
 *	}
 * \endcode
 */
typedef struct MsgPort
{
	List  queue;   /*!< Messages queued at this port. */
	Event event;   /*!< Event to trigger when a message arrives. */
} MsgPort;


typedef struct Msg
{
	Node     link;      /*!< Link into message port queue. */
	MsgPort *replyPort; /*!< Port to which the msg is to be replied. */
	/* User data may follow */
} Msg;


/*!
 * Lock a message port.
 *
 * This is required before reading or manipulating
 * any field of the MsgPort structure.
 *
 * \note Ports may be locked multiple times and each
 *       call to msg_lockPort() must be paired with
 *       a corresponding call to msg_unlockPort().
 *
 * \todo Add a configurable policy for locking against
 *       interrupts and locking with semaphorse.
 *
 * \see msg_unlockPort()
 */
INLINE void msg_lockPort(MsgPort *port)
{
	proc_forbid();
}

/*!
 * Unlock a message port.
 *
 * \see msg_lockPort()
 */
INLINE void msg_unlockPort(MsgPort *port)
{
	proc_permit();
}


/*! Initialize a message port */
INLINE void msg_initPort(MsgPort *port, Event event)
{
	LIST_INIT(&port->queue);
	port->event = event;
}

/*! Queue \a msg into \a port, triggering the associated event */
INLINE void msg_put(MsgPort *port, Msg *msg)
{
	msg_portLock(port);
	ADDTAIL(&port->queue, &msg->link);
	msg_portUnlock(port);

	event_do(&port->event);
}

/*!
 * Get the first message from the queue of \a port.
 *
 * \return Pointer to the message or NULL if the port was empty.
 */
INLINE Msg *msg_get(MsgPort *port)
{
	Msg *msg;

	msg_portLock(port);
	msg = (Msg *)REMHEAD(&port->queue);
	msg_portUnlock(port);

	return msg;
}

/*! Peek the first message in the queue of \a port, or NULL if the port is empty. */
INLINE Msg *msg_peek(MsgPort *port)
{
	Msg *msg;

	msg_portLock(port);
	msg = (Msg *)port->queue.head;
	if (ISLISTEMPTY(&port->queue))
		msg = NULL;
	msg_portUnlock(port);

	return msg;
}

/*! Send back (reply) \a msg to its sender. */
INLINE void msg_reply(Msg *msg)
{
	msg_put(msg->replyPort, msg);
}

#endif /* KERN_MSG_H */
