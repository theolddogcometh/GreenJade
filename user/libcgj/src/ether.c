/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * ether_aton / ether_ntoa family (ASCII MAC). Clean-room pure C.
 *
 * greppable: CGJ_ETHER_SOFT_LINE_PARSE
 * greppable: CGJ_ETHER_SOFT_TRAIL
 * greppable: CGJ_ETHER_SOFT_HOST_TABLE
 *
 * Soft deepen: pure-C ether_line (no sscanf), require end-of-MAC,
 * comment/blank skip, tiny host↔MAC soft table (localhost).
 */
#include <errno.h>
#include <netinet/ether.h>
#include <stdint.h>
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

        hi = hex_nibble((unsigned char)*p);
        if (hi < 0) {
            return NULL;
        }
        p++;
        lo = hex_nibble((unsigned char)*p);
        if (lo >= 0) {
            p++;
            pAddr->ether_addr_octet[i] = (uint8_t)((hi << 4) | lo);
        } else {
            /* single nibble → low nibble form (legacy soft) */
            pAddr->ether_addr_octet[i] = (uint8_t)hi;
        }
        if (i < 5) {
            if (*p != ':' && *p != '-') {
                return NULL;
            }
            p++;
        }
    }
    /* greppable: CGJ_ETHER_SOFT_TRAIL */
    if (*p != '\0') {
        return NULL;
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

/* greppable: CGJ_ETHER_SOFT_LINE_PARSE */
static int
soft_scan_word(const char **pp, char *szOut, size_t cbOut)
{
    const char *p;
    size_t n = 0;

    if (pp == NULL || *pp == NULL || szOut == NULL || cbOut == 0) {
        return -1;
    }
    p = *pp;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '\0' || *p == '\n' || *p == '#') {
        return -1;
    }
    while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '#') {
        if (n + 1 >= cbOut) {
            return -1;
        }
        szOut[n++] = *p++;
    }
    szOut[n] = '\0';
    *pp = p;
    return (n > 0) ? 0 : -1;
}

int
ether_line(const char *szLine, struct ether_addr *pAddr, char *szHostname)
{
    const char *p;
    char aMac[32];
    char aHost[256];

    if (szLine == NULL || pAddr == NULL || szHostname == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = szLine;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '\0' || *p == '\n' || *p == '#') {
        errno = EINVAL;
        return -1;
    }
    if (soft_scan_word(&p, aMac, sizeof(aMac)) != 0) {
        errno = EINVAL;
        return -1;
    }
    if (soft_scan_word(&p, aHost, sizeof(aHost)) != 0) {
        errno = EINVAL;
        return -1;
    }
    if (ether_aton_r(aMac, pAddr) == NULL) {
        errno = EINVAL;
        return -1;
    }
    {
        size_t n = strlen(aHost);
        size_t i;

        for (i = 0; i < n; i++) {
            szHostname[i] = aHost[i];
        }
        szHostname[n] = '\0';
    }
    return 0;
}

/* greppable: CGJ_ETHER_SOFT_HOST_TABLE */
static const struct {
    const char *szHost;
    uint8_t     aMac[6];
} g_aEtherHost[] = {
    { "localhost", { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { NULL, { 0, 0, 0, 0, 0, 0 } }
};

static int
soft_eq_ci(const char *szA, const char *szB)
{
    size_t i;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    for (i = 0;; i++) {
        unsigned char ca = (unsigned char)szA[i];
        unsigned char cb = (unsigned char)szB[i];

        if (ca >= 'A' && ca <= 'Z') {
            ca = (unsigned char)(ca - 'A' + 'a');
        }
        if (cb >= 'A' && cb <= 'Z') {
            cb = (unsigned char)(cb - 'A' + 'a');
        }
        if (ca != cb) {
            return 0;
        }
        if (ca == '\0') {
            return 1;
        }
    }
}

int
ether_hostton(const char *szHost, struct ether_addr *pAddr)
{
    size_t i;

    if (szHost == NULL || pAddr == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; g_aEtherHost[i].szHost != NULL; i++) {
        if (soft_eq_ci(szHost, g_aEtherHost[i].szHost)) {
            memcpy(pAddr->ether_addr_octet, g_aEtherHost[i].aMac, 6);
            return 0;
        }
    }
    errno = ENOENT;
    return -1;
}

int
ether_ntohost(char *szHost, const struct ether_addr *pAddr)
{
    size_t i;
    size_t n;

    if (szHost == NULL || pAddr == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; g_aEtherHost[i].szHost != NULL; i++) {
        if (memcmp(pAddr->ether_addr_octet, g_aEtherHost[i].aMac, 6) == 0) {
            n = strlen(g_aEtherHost[i].szHost);
            memcpy(szHost, g_aEtherHost[i].szHost, n + 1);
            return 0;
        }
    }
    errno = ENOENT;
    return -1;
}
