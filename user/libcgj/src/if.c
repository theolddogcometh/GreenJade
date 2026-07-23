/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room if_nametoindex / getifaddrs (bring-up: lo only).
 *
 * greppable: CGJ_IF_SOFT_LO_V6
 * greppable: CGJ_IF_SOFT_NAMESIZE
 * greppable: CGJ_IF_SOFT_NAMEINDEX_FREE
 *
 * Soft deepen: lo IPv4+IPv6 ifaddrs chain, IF_NAMESIZE bounds on
 * indextoname, freenameindex walks sentinel safely.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#define GJ_IF_LO_INDEX 1u

unsigned int
if_nametoindex(const char *szIfname)
{
    if (szIfname == NULL) {
        errno = EFAULT;
        return 0;
    }
    if (szIfname[0] == 'l' && szIfname[1] == 'o' && szIfname[2] == '\0') {
        return GJ_IF_LO_INDEX;
    }
    errno = ENODEV;
    return 0;
}

char *
if_indextoname(unsigned int uIndex, char *szIfname)
{
    if (szIfname == NULL) {
        errno = EFAULT;
        return NULL;
    }
    /* greppable: CGJ_IF_SOFT_NAMESIZE */
    if (uIndex == GJ_IF_LO_INDEX) {
        if (IF_NAMESIZE < 3) {
            errno = ENOSPC;
            return NULL;
        }
        szIfname[0] = 'l';
        szIfname[1] = 'o';
        szIfname[2] = '\0';
        return szIfname;
    }
    errno = ENXIO;
    return NULL;
}

struct if_nameindex *
if_nameindex(void)
{
    struct if_nameindex *p;
    char *sz;

    p = (struct if_nameindex *)calloc(2, sizeof(*p));
    sz = (char *)malloc(3);
    if (p == NULL || sz == NULL) {
        free(p);
        free(sz);
        errno = ENOMEM;
        return NULL;
    }
    sz[0] = 'l';
    sz[1] = 'o';
    sz[2] = '\0';
    p[0].if_index = GJ_IF_LO_INDEX;
    p[0].if_name = sz;
    p[1].if_index = 0;
    p[1].if_name = NULL;
    return p;
}

void
if_freenameindex(struct if_nameindex *p)
{
    size_t i;

    if (p == NULL) {
        return;
    }
    /* greppable: CGJ_IF_SOFT_NAMEINDEX_FREE */
    for (i = 0; p[i].if_name != NULL || p[i].if_index != 0; i++) {
        free(p[i].if_name);
        if (p[i].if_index == 0 && p[i].if_name == NULL) {
            break;
        }
    }
    free(p);
}

static struct ifaddrs *
soft_make_lo_v4(void)
{
    struct ifaddrs *p;
    struct sockaddr_in *pAddr;
    struct sockaddr_in *pMask;
    char *szName;

    p = (struct ifaddrs *)calloc(1, sizeof(*p));
    pAddr = (struct sockaddr_in *)calloc(1, sizeof(*pAddr));
    pMask = (struct sockaddr_in *)calloc(1, sizeof(*pMask));
    szName = (char *)malloc(3);
    if (p == NULL || pAddr == NULL || pMask == NULL || szName == NULL) {
        free(p);
        free(pAddr);
        free(pMask);
        free(szName);
        return NULL;
    }
    szName[0] = 'l';
    szName[1] = 'o';
    szName[2] = '\0';
    pAddr->sin_family = AF_INET;
    pAddr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    pMask->sin_family = AF_INET;
    pMask->sin_addr.s_addr = htonl(0xff000000u);
    p->ifa_name = szName;
    p->ifa_flags = IFF_UP | IFF_LOOPBACK | IFF_RUNNING;
    p->ifa_addr = (struct sockaddr *)pAddr;
    p->ifa_netmask = (struct sockaddr *)pMask;
    p->ifa_next = NULL;
    return p;
}

/* greppable: CGJ_IF_SOFT_LO_V6 */
static struct ifaddrs *
soft_make_lo_v6(void)
{
    struct ifaddrs *p;
    struct sockaddr_in6 *pAddr;
    struct sockaddr_in6 *pMask;
    char *szName;

    p = (struct ifaddrs *)calloc(1, sizeof(*p));
    pAddr = (struct sockaddr_in6 *)calloc(1, sizeof(*pAddr));
    pMask = (struct sockaddr_in6 *)calloc(1, sizeof(*pMask));
    szName = (char *)malloc(3);
    if (p == NULL || pAddr == NULL || pMask == NULL || szName == NULL) {
        free(p);
        free(pAddr);
        free(pMask);
        free(szName);
        return NULL;
    }
    szName[0] = 'l';
    szName[1] = 'o';
    szName[2] = '\0';
    pAddr->sin6_family = AF_INET6;
    pAddr->sin6_addr.s6_addr[15] = 1; /* ::1 */
    pMask->sin6_family = AF_INET6;
    memset(pMask->sin6_addr.s6_addr, 0xff, 16); /* /128 soft */
    p->ifa_name = szName;
    p->ifa_flags = IFF_UP | IFF_LOOPBACK | IFF_RUNNING;
    p->ifa_addr = (struct sockaddr *)pAddr;
    p->ifa_netmask = (struct sockaddr *)pMask;
    p->ifa_next = NULL;
    return p;
}

int
getifaddrs(struct ifaddrs **ppIfap)
{
    struct ifaddrs *p4;
    struct ifaddrs *p6;

    if (ppIfap == NULL) {
        errno = EFAULT;
        return -1;
    }
    p4 = soft_make_lo_v4();
    p6 = soft_make_lo_v6();
    if (p4 == NULL || p6 == NULL) {
        freeifaddrs(p4);
        freeifaddrs(p6);
        errno = ENOMEM;
        return -1;
    }
    p4->ifa_next = p6;
    *ppIfap = p4;
    return 0;
}

void
freeifaddrs(struct ifaddrs *pIfa)
{
    struct ifaddrs *p;

    while (pIfa != NULL) {
        p = pIfa->ifa_next;
        free(pIfa->ifa_name);
        free(pIfa->ifa_addr);
        free(pIfa->ifa_netmask);
        free(pIfa->ifa_broadaddr);
        free(pIfa);
        pIfa = p;
    }
}
