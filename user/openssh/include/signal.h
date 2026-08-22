/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Extra Linux signal numbers for OpenSSH-portable. libcgj <signal.h>
 * stops at SIGTSTP; misc.c uses SIGSYS.
 */
#pragma once

#include_next <signal.h>

#ifndef SIGTTIN
# define SIGTTIN 21
#endif
#ifndef SIGTTOU
# define SIGTTOU 22
#endif
#ifndef SIGURG
# define SIGURG 23
#endif
#ifndef SIGXCPU
# define SIGXCPU 24
#endif
#ifndef SIGXFSZ
# define SIGXFSZ 25
#endif
#ifndef SIGVTALRM
# define SIGVTALRM 26
#endif
#ifndef SIGPROF
# define SIGPROF 27
#endif
#ifndef SIGWINCH
# define SIGWINCH 28
#endif
#ifndef SIGIO
# define SIGIO 29
#endif
#ifndef SIGPOLL
# define SIGPOLL SIGIO
#endif
#ifndef SIGPWR
# define SIGPWR 30
#endif
#ifndef SIGSYS
# define SIGSYS 31
#endif
#ifndef SIGSTKFLT
# define SIGSTKFLT 16
#endif
#ifndef SIGUNUSED
# define SIGUNUSED SIGSYS
#endif
#ifndef NSIG
# define NSIG 65
#endif
#ifndef _NSIG
# define _NSIG NSIG
#endif
