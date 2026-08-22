/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PF_* / SO_* aliases for OpenSSH-portable (unix_listener uses PF_UNIX).
 * libcgj may already define these; keep ifndefs. struct ucred is in libcgj.
 */
#pragma once

#include_next <sys/socket.h>

#ifndef PF_UNSPEC
# define PF_UNSPEC AF_UNSPEC
#endif
#ifndef PF_UNIX
# define PF_UNIX AF_UNIX
#endif
#ifndef PF_INET
# define PF_INET AF_INET
#endif
#ifndef PF_INET6
# define PF_INET6 AF_INET6
#endif
#ifndef PF_LOCAL
# define PF_LOCAL AF_UNIX
#endif
#ifndef SOMAXCONN
# define SOMAXCONN 4096
#endif
#ifndef SO_BINDTODEVICE
# define SO_BINDTODEVICE 25
#endif
#ifndef SO_PEERCRED
# define SO_PEERCRED 17
#endif
/* libcgj <sys/socket.h> already has struct ucred (SO_PEERCRED). */
