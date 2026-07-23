/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room inet_pton / inet_ntop / helpers (IPv4; IPv6 soft).
 * Not GNU glibc.
 *
 * greppable: CGJ_INET_SOFT_ATON_ABBREV
 * greppable: CGJ_INET_SOFT_V6_COMPRESS
 * greppable: CGJ_INET_SOFT_V4MAPPED
 *
 * Soft deepen: BSD-style abbreviated IPv4 forms, IPv6 longest-run :: in
 * ntop, IPv4-embedded tails and mapped addresses in pton. Pure C, no DNS.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

/* IPv6 address layout when AF_INET6 used without full in6_addr header */
struct gj_in6 {
    uint8_t s6[16];
};

static int
parse_dec_u(const char **pp, unsigned *pVal, unsigned uMax, int nMaxDig)
{
    const char *p = *pp;
    unsigned v = 0;
    int n = 0;

    if (p == NULL || *p < '0' || *p > '9') {
        return -1;
    }
    while (*p >= '0' && *p <= '9' && n < nMaxDig) {
        unsigned d = (unsigned)(*p - '0');

        if (v > (uMax - d) / 10u) {
            return -1;
        }
        v = v * 10u + d;
        p++;
        n++;
    }
    if (n == 0 || v > uMax) {
        return -1;
    }
    *pVal = v;
    *pp = p;
    return 0;
}

/*
 * greppable: CGJ_INET_SOFT_ATON_ABBREV
 * a | a.b | a.b.c | a.b.c.d  (host-order classful-style assemble).
 */
int
inet_aton(const char *sz, struct in_addr *pAddr)
{
    const char *p;
    unsigned a[4];
    int cParts = 0;
    uint32_t uHost;

    if (sz == NULL || pAddr == NULL) {
        return 0;
    }
    p = sz;
    for (;;) {
        if (cParts >= 4) {
            return 0;
        }
        if (parse_dec_u(&p, &a[cParts], 0xffffffffu, 10) != 0) {
            return 0;
        }
        cParts++;
        if (*p == '\0') {
            break;
        }
        if (*p != '.') {
            return 0;
        }
        p++;
        if (*p == '\0') {
            return 0; /* trailing dot */
        }
    }

    if (cParts == 1) {
        /* greppable: CGJ_INET_SOFT_ATON_ABBREV */
        uHost = a[0];
    } else if (cParts == 2) {
        if (a[0] > 0xffu || a[1] > 0xffffffu) {
            return 0;
        }
        uHost = (a[0] << 24) | a[1];
    } else if (cParts == 3) {
        if (a[0] > 0xffu || a[1] > 0xffu || a[2] > 0xffffu) {
            return 0;
        }
        uHost = (a[0] << 24) | (a[1] << 16) | a[2];
    } else {
        if (a[0] > 0xffu || a[1] > 0xffu || a[2] > 0xffu || a[3] > 0xffu) {
            return 0;
        }
        uHost = (a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3];
    }
    pAddr->s_addr = htonl(uHost);
    return 1;
}

in_addr_t
inet_addr(const char *sz)
{
    struct in_addr a;

    if (inet_aton(sz, &a) == 0) {
        return (in_addr_t)0xffffffffu;
    }
    return a.s_addr;
}

char *
inet_ntoa(struct in_addr in)
{
    static char aBuf[16];
    uint32_t v = ntohl(in.s_addr);
    unsigned parts[4];
    char *p = aBuf;
    int i;

    parts[0] = (v >> 24) & 0xffu;
    parts[1] = (v >> 16) & 0xffu;
    parts[2] = (v >> 8) & 0xffu;
    parts[3] = v & 0xffu;
    for (i = 0; i < 4; i++) {
        char t[4];
        int n = 0;
        unsigned x = parts[i];

        if (x == 0) {
            t[n++] = '0';
        } else {
            while (x > 0 && n < 3) {
                t[n++] = (char)('0' + (x % 10u));
                x /= 10u;
            }
            {
                int a = 0;
                int b = n - 1;

                while (a < b) {
                    char c = t[a];

                    t[a] = t[b];
                    t[b] = c;
                    a++;
                    b--;
                }
            }
        }
        memcpy(p, t, (size_t)n);
        p += n;
        if (i < 3) {
            *p++ = '.';
        }
    }
    *p = '\0';
    return aBuf;
}

static int
hex_val(int c)
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

/* Minimal IPv6: full form, :: compression, optional IPv4 tail. */
static int
pton6(const char *sz, struct gj_in6 *pOut)
{
    uint16_t aWord[8];
    int nWords = 0;
    int nCompress = -1;
    const char *p = sz;
    int i;
    int fV4Tail = 0;

    memset(aWord, 0, sizeof(aWord));
    if (p[0] == ':' && p[1] == ':') {
        nCompress = 0;
        p += 2;
    } else if (p[0] == ':') {
        return 0; /* lone leading colon without second */
    }
    while (*p != '\0' && nWords < 8) {
        unsigned v = 0;
        int nDig = 0;
        int d;
        const char *pSave;

        if (*p == ':') {
            if (nCompress >= 0) {
                return 0; /* second :: */
            }
            nCompress = nWords;
            p++;
            if (*p == '\0') {
                break;
            }
            continue;
        }

        /* greppable: CGJ_INET_SOFT_V4MAPPED — dotted IPv4 tail */
        pSave = p;
        if (nWords <= 6) {
            const char *q = p;
            int fDot = 0;

            while (*q != '\0' && *q != ':') {
                if (*q == '.') {
                    fDot = 1;
                    break;
                }
                q++;
            }
            if (fDot) {
                struct in_addr a4;

                if (inet_aton(p, &a4) == 0) {
                    return 0;
                }
                {
                    uint32_t u = ntohl(a4.s_addr);

                    aWord[nWords++] = (uint16_t)((u >> 16) & 0xffffu);
                    if (nWords >= 8) {
                        return 0;
                    }
                    aWord[nWords++] = (uint16_t)(u & 0xffffu);
                }
                fV4Tail = 1;
                p = p + strlen(p);
                break;
            }
        }
        (void)pSave;

        while ((d = hex_val((unsigned char)*p)) >= 0 && nDig < 4) {
            v = (v << 4) | (unsigned)d;
            p++;
            nDig++;
        }
        if (nDig == 0) {
            return 0;
        }
        aWord[nWords++] = (uint16_t)v;
        if (*p == '\0') {
            break;
        }
        if (*p != ':') {
            return 0;
        }
        p++;
        if (*p == '\0') {
            return 0; /* trailing single : */
        }
    }
    if (*p != '\0') {
        return 0;
    }
    if (nCompress >= 0) {
        int nTail = nWords - nCompress;
        int nZ = 8 - nWords;

        if (nZ < 0) {
            return 0;
        }
        if (nZ == 0 && !(fV4Tail && nCompress == nWords)) {
            /* :: with no room only OK if empty expand already full */
            if (nWords != 8) {
                return 0;
            }
        }
        memmove(&aWord[nCompress + nZ], &aWord[nCompress],
                (size_t)nTail * sizeof(uint16_t));
        memset(&aWord[nCompress], 0, (size_t)nZ * sizeof(uint16_t));
        nWords = 8;
    }
    if (nWords != 8) {
        return 0;
    }
    for (i = 0; i < 8; i++) {
        pOut->s6[i * 2] = (uint8_t)(aWord[i] >> 8);
        pOut->s6[i * 2 + 1] = (uint8_t)(aWord[i] & 0xffu);
    }
    return 1;
}

int
inet_pton(int nAf, const char *szSrc, void *pDst)
{
    if (szSrc == NULL || pDst == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (nAf == AF_INET) {
        struct in_addr a;

        if (inet_aton(szSrc, &a) == 0) {
            return 0;
        }
        /* Soft: full a.b.c.d only for pton (POSIX); abbreviated still via aton */
        {
            const char *p = szSrc;
            int cDot = 0;

            while (*p != '\0') {
                if (*p == '.') {
                    cDot++;
                }
                p++;
            }
            if (cDot != 3) {
                return 0;
            }
        }
        memcpy(pDst, &a, sizeof(a));
        return 1;
    }
    if (nAf == AF_INET6) {
        struct gj_in6 a6;

        if (pton6(szSrc, &a6) == 0) {
            return 0;
        }
        memcpy(pDst, a6.s6, 16);
        return 1;
    }
    errno = EAFNOSUPPORT;
    return -1;
}

static size_t
fmt_u16_hex(char *p, uint16_t v)
{
    char t[4];
    int n = 0;
    size_t i;

    if (v == 0) {
        p[0] = '0';
        return 1;
    }
    while (v > 0 && n < 4) {
        unsigned d = v & 0xfu;

        t[n++] = (char)(d < 10 ? '0' + d : 'a' + (d - 10));
        v >>= 4;
    }
    for (i = 0; i < (size_t)n; i++) {
        p[i] = t[n - 1 - (int)i];
    }
    return (size_t)n;
}

/* greppable: CGJ_INET_SOFT_V6_COMPRESS */
static void
ntop6(const uint8_t *b, char *aTmp)
{
    uint16_t aWord[8];
    int i;
    int iBest = -1;
    int nBest = 0;
    int iRun = -1;
    int nRun = 0;
    char *p = aTmp;

    for (i = 0; i < 8; i++) {
        aWord[i] = (uint16_t)((b[i * 2] << 8) | b[i * 2 + 1]);
    }
    /* longest zero run (≥2 words) — first wins on ties (RFC 5952) */
    for (i = 0; i < 8; i++) {
        if (aWord[i] == 0) {
            if (iRun < 0) {
                iRun = i;
                nRun = 1;
            } else {
                nRun++;
            }
        } else {
            if (nRun > nBest) {
                nBest = nRun;
                iBest = iRun;
            }
            iRun = -1;
            nRun = 0;
        }
    }
    if (nRun > nBest) {
        nBest = nRun;
        iBest = iRun;
    }
    if (nBest < 2) {
        iBest = -1;
    }

    i = 0;
    while (i < 8) {
        if (i == iBest) {
            *p++ = ':';
            i += nBest;
            if (i >= 8) {
                *p++ = ':';
            }
            continue;
        }
        if (i > 0) {
            *p++ = ':';
        }
        p += fmt_u16_hex(p, aWord[i]);
        i++;
    }
    *p = '\0';
}

const char *
inet_ntop(int nAf, const void *pSrc, char *szDst, size_t cbDst)
{
    if (pSrc == NULL || szDst == NULL || cbDst == 0) {
        errno = EFAULT;
        return NULL;
    }
    if (nAf == AF_INET) {
        struct in_addr a;
        char *p;

        memcpy(&a, pSrc, sizeof(a));
        p = inet_ntoa(a);
        if (strlen(p) + 1 > cbDst) {
            errno = ENOSPC;
            return NULL;
        }
        strcpy(szDst, p);
        return szDst;
    }
    if (nAf == AF_INET6) {
        const uint8_t *b = (const uint8_t *)pSrc;
        char aTmp[48];

        ntop6(b, aTmp);
        if (strlen(aTmp) + 1 > cbDst) {
            errno = ENOSPC;
            return NULL;
        }
        strcpy(szDst, aTmp);
        return szDst;
    }
    errno = EAFNOSUPPORT;
    return NULL;
}

/* Classful helpers (historical; still linked by older graphs). */
in_addr_t
inet_network(const char *sz)
{
    struct in_addr a;

    if (inet_aton(sz, &a) == 0) {
        return (in_addr_t)0xffffffffu;
    }
    return ntohl(a.s_addr);
}

struct in_addr
inet_makeaddr(in_addr_t net, in_addr_t host)
{
    struct in_addr a;
    uint32_t n = (uint32_t)net;
    uint32_t h = (uint32_t)host;

    if (n < 0x100u) {
        a.s_addr = htonl((n << 24) | (h & 0x00ffffffu));
    } else if (n < 0x10000u) {
        a.s_addr = htonl((n << 16) | (h & 0x0000ffffu));
    } else if (n < 0x1000000u) {
        a.s_addr = htonl((n << 8) | (h & 0x000000ffu));
    } else {
        a.s_addr = htonl(n | h);
    }
    return a;
}

in_addr_t
inet_lnaof(struct in_addr in)
{
    uint32_t host = ntohl(in.s_addr);

    if ((host & 0x80000000u) == 0u) {
        return host & 0x00ffffffu; /* class A */
    }
    if ((host & 0xc0000000u) == 0x80000000u) {
        return host & 0x0000ffffu; /* class B */
    }
    return host & 0x000000ffu; /* class C */
}

in_addr_t
inet_netof(struct in_addr in)
{
    uint32_t host = ntohl(in.s_addr);

    if ((host & 0x80000000u) == 0u) {
        return (host >> 24) & 0xffu;
    }
    if ((host & 0xc0000000u) == 0x80000000u) {
        return (host >> 16) & 0xffffu;
    }
    return (host >> 8) & 0xffffffu;
}
