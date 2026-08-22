/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <netinet/tcp.h> for OpenSSH-portable (TCP_NODELAY in misc.c).
 * Host POSIX compiles (#include_next glibc).
 */
#pragma once
#include_next <netinet/tcp.h>

#ifndef TCP_NODELAY
# define TCP_NODELAY 1
#endif
#ifndef TCP_MAXSEG
# define TCP_MAXSEG 2
#endif
#ifndef TH_FIN
# define TH_FIN  0x01
# define TH_SYN  0x02
# define TH_RST  0x04
# define TH_PUSH 0x08
# define TH_ACK  0x10
# define TH_URG  0x20
#endif
