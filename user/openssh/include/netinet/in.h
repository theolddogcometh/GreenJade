/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Extra IP/IPv6 socket options for OpenSSH-portable misc.c (IP_TOS,
 * IPV6_TCLASS, IPV6_V6ONLY). libcgj <netinet/in.h> has address types only.
 */
#pragma once

#include_next <netinet/in.h>

#ifndef IPPORT_RESERVED
# define IPPORT_RESERVED 1024
#endif
#ifndef IP_TOS
# define IP_TOS 1
#endif
#ifndef IP_TTL
# define IP_TTL 2
#endif
#ifndef IPPROTO_IPV6
# define IPPROTO_IPV6 41
#endif
#ifndef IPV6_V6ONLY
# define IPV6_V6ONLY 26
#endif
#ifndef IPV6_TCLASS
# define IPV6_TCLASS 67
#endif
#ifndef IN6_IS_ADDR_LOOPBACK
# define IN6_IS_ADDR_LOOPBACK(a) \
    ((a)->s6_addr[0] == 0 && (a)->s6_addr[1] == 0 && \
     (a)->s6_addr[2] == 0 && (a)->s6_addr[3] == 0 && \
     (a)->s6_addr[4] == 0 && (a)->s6_addr[5] == 0 && \
     (a)->s6_addr[6] == 0 && (a)->s6_addr[7] == 0 && \
     (a)->s6_addr[8] == 0 && (a)->s6_addr[9] == 0 && \
     (a)->s6_addr[10] == 0 && (a)->s6_addr[11] == 0 && \
     (a)->s6_addr[12] == 0 && (a)->s6_addr[13] == 0 && \
     (a)->s6_addr[14] == 0 && (a)->s6_addr[15] == 1)
#endif
#ifndef INET_ADDRSTRLEN
# define INET_ADDRSTRLEN 16
#endif
#ifndef INET6_ADDRSTRLEN
# define INET6_ADDRSTRLEN 46
#endif
