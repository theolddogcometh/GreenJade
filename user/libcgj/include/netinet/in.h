/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <netinet/in.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * sockaddr_in/sockaddr_in6, in_addr, IPPROTO_*, and INADDR_* constants for
 * IPv4/IPv6 with <sys/socket.h> and <arpa/inet.h>.
 *
 * Design notes
 * ------------
 * Layout and constant values follow Linux LP64 so wire and ioctl paths agree.
 *
 * Non-goals
 * ---------
 * Every historic BSD multicast option name.
 *
 * Soft residual (C2 libcgj netinet/in.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = sockaddr_in/in6 + in_addr + IPPROTO_* INADDR_* Linux LP64 catalog
 *   product  = Dual DoD B wire = rtl8168_udx + DDI (OPEN); freestanding rtl SKIP
 *   honesty  = address family catalog != product stack close; agent PASS != Dual
 *              DoD close; stamp-free bar honesty (no version stamp); G-AC-1 no .ko AC
 *   law      = dual MIT OR Apache-2.0; no GPL; H1 no net_eth_poll from IRQ;
 *              H2 no stamp storms
 * greppable: libcgj: netinet/in soft residual Soft!=product
 */
#pragma once

#include <stdint.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPPROTO_IP   0
#define IPPROTO_ICMP 1
#define IPPROTO_TCP  6
#define IPPROTO_UDP  17
#define IPPROTO_IPV6 41
#define IPPROTO_ICMPV6 58
#ifndef IPPROTO_RAW
#define IPPROTO_RAW  255
#endif

#define IP_TOS  1
#define IP_TTL  2
#define IP_HDRINCL 3
#define IP_OPTIONS 4
#define IP_RECVOPTS 6
#define IP_RETOPTS 7
#define IP_PKTINFO 8
#define IP_MTU_DISCOVER 10
#define IP_RECVERR 11
#define IP_RECVTTL 12
#define IP_RECVTOS 13
#define IP_MTU 14
#define IP_FREEBIND 15
#define IP_TRANSPARENT 19
#define IP_ADD_MEMBERSHIP 35
#define IP_DROP_MEMBERSHIP 36
#define IPV6_V6ONLY  26
#define IPV6_UNICAST_HOPS 16
#define IPV6_MULTICAST_HOPS 18
#define IPV6_MULTICAST_LOOP 19
#define IPV6_RECVPKTINFO 49
#define IPV6_PKTINFO 50
#define IPV6_HOPLIMIT 52
#define IPV6_TCLASS  67

#ifndef IPPORT_RESERVED
#define IPPORT_RESERVED 1024
#endif

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN  16
#endif
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

#define INADDR_ANY       ((uint32_t)0x00000000u)
#define INADDR_LOOPBACK  ((uint32_t)0x7f000001u)
#define INADDR_BROADCAST ((uint32_t)0xffffffffu)
#define INADDR_NONE      ((uint32_t)0xffffffffu)

typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;

#ifndef IN_LOOPBACKNET
#define IN_LOOPBACKNET 127
#endif
#ifndef IN_CLASSA
#define IN_CLASSA(a) ((((in_addr_t)(a)) & 0x80000000u) == 0u)
#define IN_CLASSA_NET    0xff000000u
#define IN_CLASSA_NSHIFT 24
#define IN_CLASSA_HOST   0x00ffffffu
#define IN_CLASSA_MAX    128
#define IN_CLASSB(a) ((((in_addr_t)(a)) & 0xc0000000u) == 0x80000000u)
#define IN_CLASSB_NET    0xffff0000u
#define IN_CLASSB_NSHIFT 16
#define IN_CLASSB_HOST   0x0000ffffu
#define IN_CLASSB_MAX    65536
#define IN_CLASSC(a) ((((in_addr_t)(a)) & 0xe0000000u) == 0xc0000000u)
#define IN_CLASSC_NET    0xffffff00u
#define IN_CLASSC_NSHIFT 8
#define IN_CLASSC_HOST   0x000000ffu
#define IN_CLASSD(a) ((((in_addr_t)(a)) & 0xf0000000u) == 0xe0000000u)
#define IN_MULTICAST(a)  IN_CLASSD(a)
#define IN_EXPERIMENTAL(a) ((((in_addr_t)(a)) & 0xe0000000u) == 0xe0000000u)
#define IN_BADCLASS(a)   ((((in_addr_t)(a)) & 0xf0000000u) == 0xf0000000u)
#endif

struct in_addr {
    in_addr_t s_addr;
};

struct in6_addr {
    union {
        uint8_t  u6_addr8[16];
        uint16_t u6_addr16[8];
        uint32_t u6_addr32[4];
    } in6_u;
};
#define s6_addr in6_u.u6_addr8

#ifndef IN6_IS_ADDR_UNSPECIFIED
#define IN6_IS_ADDR_UNSPECIFIED(a) \
    ((a)->s6_addr[0] == 0 && (a)->s6_addr[1] == 0 && \
     (a)->s6_addr[2] == 0 && (a)->s6_addr[3] == 0 && \
     (a)->s6_addr[4] == 0 && (a)->s6_addr[5] == 0 && \
     (a)->s6_addr[6] == 0 && (a)->s6_addr[7] == 0 && \
     (a)->s6_addr[8] == 0 && (a)->s6_addr[9] == 0 && \
     (a)->s6_addr[10] == 0 && (a)->s6_addr[11] == 0 && \
     (a)->s6_addr[12] == 0 && (a)->s6_addr[13] == 0 && \
     (a)->s6_addr[14] == 0 && (a)->s6_addr[15] == 0)
#endif
#ifndef IN6_IS_ADDR_LOOPBACK
#define IN6_IS_ADDR_LOOPBACK(a) \
    ((a)->s6_addr[0] == 0 && (a)->s6_addr[1] == 0 && \
     (a)->s6_addr[2] == 0 && (a)->s6_addr[3] == 0 && \
     (a)->s6_addr[4] == 0 && (a)->s6_addr[5] == 0 && \
     (a)->s6_addr[6] == 0 && (a)->s6_addr[7] == 0 && \
     (a)->s6_addr[8] == 0 && (a)->s6_addr[9] == 0 && \
     (a)->s6_addr[10] == 0 && (a)->s6_addr[11] == 0 && \
     (a)->s6_addr[12] == 0 && (a)->s6_addr[13] == 0 && \
     (a)->s6_addr[14] == 0 && (a)->s6_addr[15] == 1)
#endif
#ifndef IN6_IS_ADDR_V4MAPPED
#define IN6_IS_ADDR_V4MAPPED(a) \
    ((a)->s6_addr[0] == 0 && (a)->s6_addr[1] == 0 && \
     (a)->s6_addr[2] == 0 && (a)->s6_addr[3] == 0 && \
     (a)->s6_addr[4] == 0 && (a)->s6_addr[5] == 0 && \
     (a)->s6_addr[6] == 0 && (a)->s6_addr[7] == 0 && \
     (a)->s6_addr[8] == 0 && (a)->s6_addr[9] == 0 && \
     (a)->s6_addr[10] == 0xff && (a)->s6_addr[11] == 0xff)
#endif
#ifndef IN6_IS_ADDR_LINKLOCAL
#define IN6_IS_ADDR_LINKLOCAL(a) \
    ((a)->s6_addr[0] == 0xfe && (((a)->s6_addr[1] & 0xc0) == 0x80))
#endif
#ifndef IN6_IS_ADDR_SITELOCAL
#define IN6_IS_ADDR_SITELOCAL(a) \
    ((a)->s6_addr[0] == 0xfe && (((a)->s6_addr[1] & 0xc0) == 0xc0))
#endif
#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(a) ((a)->s6_addr[0] == 0xff)
#endif
#ifndef IN6_IS_ADDR_MC_NODELOCAL
#define IN6_IS_ADDR_MC_NODELOCAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && (((a)->s6_addr[1] & 0x0f) == 0x01))
#endif
#ifndef IN6_IS_ADDR_MC_LINKLOCAL
#define IN6_IS_ADDR_MC_LINKLOCAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && (((a)->s6_addr[1] & 0x0f) == 0x02))
#endif
#ifndef IN6_IS_ADDR_MC_SITELOCAL
#define IN6_IS_ADDR_MC_SITELOCAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && (((a)->s6_addr[1] & 0x0f) == 0x05))
#endif
#ifndef IN6_IS_ADDR_MC_ORGLOCAL
#define IN6_IS_ADDR_MC_ORGLOCAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && (((a)->s6_addr[1] & 0x0f) == 0x08))
#endif
#ifndef IN6_IS_ADDR_MC_GLOBAL
#define IN6_IS_ADDR_MC_GLOBAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && (((a)->s6_addr[1] & 0x0f) == 0x0e))
#endif
#ifndef IN6_ARE_ADDR_EQUAL
#define IN6_ARE_ADDR_EQUAL(a, b) \
    ((a)->s6_addr[0] == (b)->s6_addr[0] && \
     (a)->s6_addr[1] == (b)->s6_addr[1] && \
     (a)->s6_addr[2] == (b)->s6_addr[2] && \
     (a)->s6_addr[3] == (b)->s6_addr[3] && \
     (a)->s6_addr[4] == (b)->s6_addr[4] && \
     (a)->s6_addr[5] == (b)->s6_addr[5] && \
     (a)->s6_addr[6] == (b)->s6_addr[6] && \
     (a)->s6_addr[7] == (b)->s6_addr[7] && \
     (a)->s6_addr[8] == (b)->s6_addr[8] && \
     (a)->s6_addr[9] == (b)->s6_addr[9] && \
     (a)->s6_addr[10] == (b)->s6_addr[10] && \
     (a)->s6_addr[11] == (b)->s6_addr[11] && \
     (a)->s6_addr[12] == (b)->s6_addr[12] && \
     (a)->s6_addr[13] == (b)->s6_addr[13] && \
     (a)->s6_addr[14] == (b)->s6_addr[14] && \
     (a)->s6_addr[15] == (b)->s6_addr[15])
#endif

struct sockaddr_in {
    sa_family_t    sin_family;
    in_port_t      sin_port;
    struct in_addr sin_addr;
    unsigned char  sin_zero[8];
};

struct sockaddr_in6 {
    sa_family_t     sin6_family;
    in_port_t       sin6_port;
    uint32_t        sin6_flowinfo;
    struct in6_addr sin6_addr;
    uint32_t        sin6_scope_id;
};

#ifndef IN6ADDR_ANY_INIT
#define IN6ADDR_ANY_INIT \
    { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } }
#endif
#ifndef IN6ADDR_LOOPBACK_INIT
#define IN6ADDR_LOOPBACK_INIT \
    { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } } }
#endif

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;

struct ip_mreq {
    struct in_addr imr_multiaddr;
    struct in_addr imr_interface;
};

struct in_pktinfo {
    int            ipi_ifindex;
    struct in_addr ipi_spec_dst;
    struct in_addr ipi_addr;
};

struct in6_pktinfo {
    struct in6_addr ipi6_addr;
    unsigned int    ipi6_ifindex;
};

uint16_t htons(uint16_t v);
uint16_t ntohs(uint16_t v);
uint32_t htonl(uint32_t v);
uint32_t ntohl(uint32_t v);

int bindresvport(int nFd, struct sockaddr_in *pSin);
int bindresvport_sa(int nFd, struct sockaddr *pSa);

#ifdef __cplusplus
}
#endif
