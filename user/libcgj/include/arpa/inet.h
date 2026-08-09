/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <arpa/inet.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * inet_pton/ntop/aton/ntoa/addr and byte-order helpers used with <netinet/in.h>.
 * Pure computation — no network I/O.
 *
 * Design notes
 * ------------
 * htonl/htons/ntohl/ntohs may also be exported from other headers for dynlink
 * graphs; keep semantics identical (big-endian wire order).
 *
 * Non-goals
 * ---------
 * Classful legacy network macros beyond common app needs.
 *
 * Soft residual (C2 libcgj arpa/inet.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = inet_pton/ntop/aton/ntoa/addr/network/makeaddr + net_ntop/pton
 *              pure string/addr compute; no socket I/O; no wire ownership
 *   product  = Dual DoD B wire = rtl8168_udx + DDI (OPEN); freestanding rtl SKIP
 *   honesty  = address helpers != product NIC/stack close; agent PASS != Dual
 *              DoD close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H1 no net_eth_poll from IRQ;
 *              H2 no stamp storms
 * greppable: libcgj: arpa/inet soft residual Soft!=product
 */
#pragma once

#include <netinet/in.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* AF_INET / AF_INET6 from socket.h */
#ifndef AF_INET
#define AF_INET  2
#endif
#ifndef AF_INET6
#define AF_INET6 10
#endif

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN  16
#endif
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

int inet_pton(int nAf, const char *szSrc, void *pDst);
const char *inet_ntop(int nAf, const void *pSrc, char *szDst, size_t cbDst);
int inet_aton(const char *sz, struct in_addr *pAddr);
char *inet_ntoa(struct in_addr in);
in_addr_t inet_addr(const char *sz);
in_addr_t inet_network(const char *sz);
struct in_addr inet_makeaddr(in_addr_t net, in_addr_t host);
in_addr_t inet_lnaof(struct in_addr in);
in_addr_t inet_netof(struct in_addr in);
char *inet_net_ntop(int nAf, const void *pSrc, int nBits, char *szDst,
                    size_t cbDst);
int inet_net_pton(int nAf, const char *szSrc, void *pDst, size_t cbDst);

#ifdef __cplusplus
}
#endif
