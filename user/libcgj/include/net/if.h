/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped net/if.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <net/if.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
 *
 * Soft residual (C2 libcgj net/if.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = ifreq/ifconf + if_nametoindex/if_indextoname/if_nameindex subset
 *   product  = Dual DoD B wire = rtl8168_udx + DDI (OPEN); freestanding rtl SKIP
 *   honesty  = if_* soft != product NIC ownership; agent PASS != Dual DoD close;
 *              stamp-free bar honesty (no version stamp); G-AC-1 no .ko AC
 *   law      = dual MIT OR Apache-2.0; no GPL; H1 no net_eth_poll from IRQ;
 *              H2 no stamp storms
 * greppable: libcgj: net/if soft residual Soft!=product
 */
#pragma once

#include <sys/socket.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IF_NAMESIZE 16

#define IFF_UP          0x1
#define IFF_BROADCAST   0x2
#define IFF_LOOPBACK    0x8
#define IFF_RUNNING     0x40
#define IFF_MULTICAST   0x1000

struct if_nameindex {
    unsigned int if_index;
    char        *if_name;
};

unsigned int if_nametoindex(const char *szIfname);
char        *if_indextoname(unsigned int uIndex, char *szIfname);
struct if_nameindex *if_nameindex(void);
void         if_freenameindex(struct if_nameindex *p);

#ifdef __cplusplus
}
#endif
