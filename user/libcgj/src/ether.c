/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * ether_aton / ether_ntoa family (ASCII MAC). Clean-room.
 */
#include <errno.h>
#include <netinet/ether.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int
hex_nibble(int c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

struct ether_addr *
ether_aton_r(const char *sz, struct ether_addr *pAddr)
{
    int i;
    const char *p = sz;

    if (sz == NULL || pAddr == NULL) {
        return NULL;
    }
    for (i = 0; i < 6; i++) {
        int hi, lo;

        hi = hex_nibble((unsigned char)*p++);
        if (hi < 0) {
            return NULL;
        }
        lo = hex_nibble((unsigned char)*p);
        if (lo >= 0) {
            p++;
            pAddr->ether_addr_octet[i] = (uint8_t)((hi << 4) | lo);
        } else {
            pAddr->ether_addr_octet[i] = (uint8_t)hi;
        }
        if (i < 5) {
            if (*p != ':' && *p != '-') {
                return NULL;
            }
            p++;
        }
    }
    return pAddr;
}

struct ether_addr *
ether_aton(const char *sz)
{
    static struct ether_addr a;

    return ether_aton_r(sz, &a);
}

char *
ether_ntoa_r(const struct ether_addr *pAddr, char *szBuf)
{
    static const char hex[] = "0123456789abcdef";
    int i;

    if (pAddr == NULL || szBuf == NULL) {
        return NULL;
    }
    for (i = 0; i < 6; i++) {
        unsigned v = pAddr->ether_addr_octet[i];

        szBuf[i * 3] = hex[(v >> 4) & 0xf];
        szBuf[i * 3 + 1] = hex[v & 0xf];
        szBuf[i * 3 + 2] = (i < 5) ? ':' : '\0';
    }
    return szBuf;
}

char *
ether_ntoa(const struct ether_addr *pAddr)
{
    static char aBuf[18];

    return ether_ntoa_r(pAddr, aBuf);
}

int
ether_line(const char *szLine, struct ether_addr *pAddr, char *szHostname)
{
    char aMac[32];
    char aHost[256];
    int n;

    if (szLine == NULL || pAddr == NULL || szHostname == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = sscanf(szLine, "%31s %255s", aMac, aHost);
    if (n < 2) {
        errno = EINVAL;
        return -1;
    }
    if (ether_aton_r(aMac, pAddr) == NULL) {
        errno = EINVAL;
        return -1;
    }
    strcpy(szHostname, aHost);
    return 0;
}

int
ether_hostton(const char *szHost, struct ether_addr *pAddr)
{
    (void)szHost;
    (void)pAddr;
    errno = ENOSYS;
    return -1;
}

int
ether_ntohost(char *szHost, const struct ether_addr *pAddr)
{
    (void)szHost;
    (void)pAddr;
    errno = ENOSYS;
    return -1;
}
