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
#ifndef IFNAMSIZ
#define IFNAMSIZ IF_NAMESIZE
#endif

#define IFF_UP          0x1
#define IFF_BROADCAST   0x2
#define IFF_DEBUG       0x4
#define IFF_LOOPBACK    0x8
#define IFF_POINTOPOINT 0x10
#define IFF_NOTRAILERS  0x20
#define IFF_RUNNING     0x40
#define IFF_NOARP       0x80
#define IFF_PROMISC     0x100
#define IFF_ALLMULTI    0x200
#define IFF_MASTER      0x400
#define IFF_SLAVE       0x800
#define IFF_MULTICAST   0x1000
#define IFF_PORTSEL     0x2000
#define IFF_AUTOMEDIA   0x4000
#define IFF_DYNAMIC     0x8000

#ifndef SIOCGIFNAME
#define SIOCGIFNAME  0x8910
#endif
#ifndef SIOCGIFCONF
#define SIOCGIFCONF  0x8912
#endif
#ifndef SIOCGIFFLAGS
#define SIOCGIFFLAGS 0x8913
#endif
#ifndef SIOCSIFFLAGS
#define SIOCSIFFLAGS 0x8914
#endif
#ifndef SIOCGIFADDR
#define SIOCGIFADDR  0x8915
#endif
#ifndef SIOCSIFADDR
#define SIOCSIFADDR  0x8916
#endif
#ifndef SIOCGIFDSTADDR
#define SIOCGIFDSTADDR 0x8917
#endif
#ifndef SIOCGIFBRDADDR
#define SIOCGIFBRDADDR 0x8919
#endif
#ifndef SIOCGIFNETMASK
#define SIOCGIFNETMASK 0x891b
#endif
#ifndef SIOCGIFMTU
#define SIOCGIFMTU   0x8921
#endif
#ifndef SIOCSIFMTU
#define SIOCSIFMTU   0x8922
#endif
#ifndef SIOCGIFHWADDR
#define SIOCGIFHWADDR 0x8927
#endif
#ifndef SIOCGIFINDEX
#define SIOCGIFINDEX 0x8933
#endif

struct ifreq {
    char ifr_name[IFNAMSIZ];
    union {
        struct sockaddr ifr_addr;
        struct sockaddr ifr_dstaddr;
        struct sockaddr ifr_broadaddr;
        struct sockaddr ifr_netmask;
        struct sockaddr ifr_hwaddr;
        short           ifr_flags;
        int             ifr_ifindex;
        int             ifr_metric;
        int             ifr_mtu;
        char            ifr_slave[IFNAMSIZ];
        char            ifr_newname[IFNAMSIZ];
        char           *ifr_data;
    } ifr_ifru;
};
#define ifr_addr      ifr_ifru.ifr_addr
#define ifr_dstaddr   ifr_ifru.ifr_dstaddr
#define ifr_broadaddr ifr_ifru.ifr_broadaddr
#define ifr_netmask   ifr_ifru.ifr_netmask
#define ifr_hwaddr    ifr_ifru.ifr_hwaddr
#define ifr_flags     ifr_ifru.ifr_flags
#define ifr_ifindex   ifr_ifru.ifr_ifindex
#define ifr_metric    ifr_ifru.ifr_metric
#define ifr_mtu       ifr_ifru.ifr_mtu
#define ifr_slave     ifr_ifru.ifr_slave
#define ifr_newname   ifr_ifru.ifr_newname
#define ifr_data      ifr_ifru.ifr_data

struct ifconf {
    int           ifc_len;
    union {
        char         *ifcu_buf;
        struct ifreq *ifcu_req;
    } ifc_ifcu;
};
#define ifc_buf ifc_ifcu.ifcu_buf
#define ifc_req ifc_ifcu.ifcu_req

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
