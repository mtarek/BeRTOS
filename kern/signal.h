/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * All Rights Reserved.
 * -->
 *
 * \brief Signal module (public interface).
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

#ifndef KERN_SIGNAL_H
#define KERN_SIGNAL_H

#include "compiler.h"


/* Fwd decl */
struct Process;

/* Inter-process Communication services */
sigset_t sig_check(sigset_t sigs);
void sig_signal(struct Process *proc, sigset_t sig);
void _sig_signal(struct Process *proc, sigset_t sig);
sigset_t sig_wait(sigset_t sigs);


/*!
 * \name Signal definitions
 *
 * \{
 */
#define SIG_USER0    BV(0)  /*!< Free for user usage */
#define SIG_USER1    BV(1)  /*!< Free for user usage */
#define SIG_USER2    BV(2)  /*!< Free for user usage */
#define SIG_USER3    BV(3)  /*!< Free for user usage */
#define SIG_SYSTEM4  BV(4)  /*!< Reserved for system use */
#define SIG_SYSTEM5  BV(5)  /*!< Reserved for system use */
#define SIG_SYSTEM6  BV(6)  /*!< Reserved for system use */
#define SIG_SINGLE   BV(7)  /*!< Used to wait for a single event */
/*\}*/

#endif /* KERN_SIGNAL_H */
