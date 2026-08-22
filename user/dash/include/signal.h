/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Dash glue: extra POSIX/Linux signal numbers + NSIG on top of libcgj.
 * Dual MIT OR Apache-2.0. Not a dash source.
 */
#ifndef DASH_GJ_SIGNAL_H
#define DASH_GJ_SIGNAL_H

#include_next <signal.h>

#ifndef SIGSTKFLT
#define SIGSTKFLT 16
#endif
#ifndef SIGTTIN
#define SIGTTIN 21
#endif
#ifndef SIGTTOU
#define SIGTTOU 22
#endif
#ifndef SIGURG
#define SIGURG 23
#endif
#ifndef SIGXCPU
#define SIGXCPU 24
#endif
#ifndef SIGXFSZ
#define SIGXFSZ 25
#endif
#ifndef SIGVTALRM
#define SIGVTALRM 26
#endif
#ifndef SIGPROF
#define SIGPROF 27
#endif
#ifndef SIGWINCH
#define SIGWINCH 28
#endif
#ifndef SIGIO
#define SIGIO 29
#endif
#ifndef SIGPOLL
#define SIGPOLL SIGIO
#endif
#ifndef SIGPWR
#define SIGPWR 30
#endif
#ifndef SIGSYS
#define SIGSYS 31
#endif
#ifndef SIGCLD
#define SIGCLD SIGCHLD
#endif
#ifndef SIGIOT
#define SIGIOT SIGABRT
#endif
#ifndef SIGRTMIN
#define SIGRTMIN 34
#endif
#ifndef SIGRTMAX
#define SIGRTMAX 64
#endif
#ifndef _NSIG
#define _NSIG 65
#endif
#ifndef NSIG
#define NSIG _NSIG
#endif

#endif /* DASH_GJ_SIGNAL_H */
