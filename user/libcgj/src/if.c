/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room if_nametoindex / getifaddrs (bring-up: lo only).
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
    if (strcmp(szIfname, "lo") == 0) {
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
    if (uIndex == GJ_IF_LO_INDEX) {
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
    if (p == NULL) {
        return;
    }
    free(p[0].if_name);
    free(p);
}

int
getifaddrs(struct ifaddrs **ppIfap)
{
    struct ifaddrs *p;
    struct sockaddr_in *pAddr;
    struct sockaddr_in *pMask;
    char *szName;

    if (ppIfap == NULL) {
        errno = EFAULT;
        return -1;
    }
    p = (struct ifaddrs *)calloc(1, sizeof(*p));
    pAddr = (struct sockaddr_in *)calloc(1, sizeof(*pAddr));
    pMask = (struct sockaddr_in *)calloc(1, sizeof(*pMask));
    szName = (char *)malloc(3);
    if (p == NULL || pAddr == NULL || pMask == NULL || szName == NULL) {
        free(p);
        free(pAddr);
        free(pMask);
        free(szName);
        errno = ENOMEM;
        return -1;
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
    *ppIfap = p;
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
