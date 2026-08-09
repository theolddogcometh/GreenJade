/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped ifaddrs.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <ifaddrs.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj ifaddrs.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = struct ifaddrs + getifaddrs/freeifaddrs (subset; soft fill OK)
 *   product  = Dual DoD B wire = rtl8168_udx + DDI (OPEN); freestanding rtl SKIP
 *   honesty  = ifaddrs soft != product NIC/stack close; agent PASS != Dual DoD
 *              close; stamp-free bar honesty (no version stamp); G-AC-1 no .ko AC
 *   law      = dual MIT OR Apache-2.0; no GPL; H1 no net_eth_poll from IRQ;
 *              H2 no stamp storms
 * greppable: libcgj: ifaddrs soft residual Soft!=product
 */
#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ifaddrs {
    struct ifaddrs  *ifa_next;
    char            *ifa_name;
    unsigned int     ifa_flags;
    struct sockaddr *ifa_addr;
    struct sockaddr *ifa_netmask;
    union {
        struct sockaddr *ifu_broadaddr;
        struct sockaddr *ifu_dstaddr;
    } ifa_ifu;
    void            *ifa_data;
};

#define ifa_broadaddr ifa_ifu.ifu_broadaddr
#define ifa_dstaddr   ifa_ifu.ifu_dstaddr

int  getifaddrs(struct ifaddrs **ppIfap);
void freeifaddrs(struct ifaddrs *pIfa);

#ifdef __cplusplus
}
#endif
