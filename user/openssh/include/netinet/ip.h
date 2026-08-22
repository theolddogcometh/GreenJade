/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <netinet/ip.h> for OpenSSH-portable (defines.h IPTOS catalog).
 * Host POSIX compiles (#include_next glibc). DUT: libcgj has no ip.h.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <netinet/ip.h>
#endif

#ifndef IPTOS_LOWDELAY
# define IPTOS_LOWDELAY    0x10
#endif
#ifndef IPTOS_THROUGHPUT
# define IPTOS_THROUGHPUT  0x08
#endif
#ifndef IPTOS_RELIABILITY
# define IPTOS_RELIABILITY 0x04
#endif
#ifndef IPTOS_LOWCOST
# define IPTOS_LOWCOST     0x02
#endif
#ifndef IPTOS_MINCOST
# define IPTOS_MINCOST     IPTOS_LOWCOST
#endif
#ifndef IPVERSION
# define IPVERSION 4
#endif
#ifndef IP_DF
# define IP_DF 0x4000
#endif
#ifndef IP_MF
# define IP_MF 0x2000
#endif
#ifndef IP_OFFMASK
# define IP_OFFMASK 0x1fff
#endif
