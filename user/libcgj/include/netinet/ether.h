/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room ether_* surface. Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <netinet/ether.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj netinet/ether.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = ether_aton/ntoa/ntoa_r/line/hostton pure MAC string helpers
 *   product  = Dual DoD B wire = rtl8168_udx + DDI (OPEN); freestanding rtl SKIP
 *   honesty  = ether_* soft != product L2 ownership; agent PASS != Dual DoD close;
 *              stamp-free bar honesty (no version stamp); G-AC-1 no .ko AC
 *   law      = dual MIT OR Apache-2.0; no GPL; H1 no net_eth_poll from IRQ;
 *              H2 no stamp storms
 * greppable: libcgj: netinet/ether soft residual Soft!=product
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ether_addr {
    uint8_t ether_addr_octet[6];
};

struct ether_addr *ether_aton(const char *sz);
struct ether_addr *ether_aton_r(const char *sz, struct ether_addr *pAddr);
char              *ether_ntoa(const struct ether_addr *pAddr);
char              *ether_ntoa_r(const struct ether_addr *pAddr, char *szBuf);
int                ether_line(const char *szLine, struct ether_addr *pAddr,
                              char *szHostname);
int                ether_hostton(const char *szHost, struct ether_addr *pAddr);
int                ether_ntohost(char *szHost, const struct ether_addr *pAddr);

#ifdef __cplusplus
}
#endif
