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
