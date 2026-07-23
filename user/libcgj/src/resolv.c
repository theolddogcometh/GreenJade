/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * resolv soft — apps link; no network DNS. Localhost A synthesised in-buffer.
 * Not GNU glibc.
 *
 * greppable: CGJ_RES_SOFT_AUTOINIT
 * greppable: CGJ_RES_SOFT_LOCALHOST_A
 * greppable: CGJ_RES_SOFT_NAME_NORM
 *
 * Soft deepen: auto res_init on first query, synthetic IN A for localhost
 * (wire-format answer, no I/O). Other names stay ENOSYS/HOST_NOT_FOUND soft.
 */
#include <errno.h>
#include <resolv.h>
#include <string.h>

struct __res_state _res;

/* DNS wire constants (soft local only). */
enum {
    CGJ_NS_C_IN = 1,
    CGJ_NS_T_A = 1,
    CGJ_NS_T_AAAA = 28
};

static int
soft_res_ready(void)
{
    /* greppable: CGJ_RES_SOFT_AUTOINIT */
    if ((_res.options & RES_INIT) == 0) {
        return res_init();
    }
    return 0;
}

int
res_init(void)
{
    memset(&_res, 0, sizeof(_res));
    _res.options = RES_INIT | RES_DEFAULT;
    _res.retrans = 5;
    _res.retry = 2;
    _res.nscount = 0;
    return 0;
}

/* greppable: CGJ_RES_SOFT_NAME_NORM — strip trailing dots, fold case ASCII */
static int
soft_name_is_localhost(const char *sz)
{
    char a[64];
    size_t n = 0;
    size_t i;

    if (sz == NULL || sz[0] == '\0') {
        return 0;
    }
    while (sz[n] != '\0' && n + 1 < sizeof(a)) {
        unsigned char c = (unsigned char)sz[n];

        if (c >= 'A' && c <= 'Z') {
            c = (unsigned char)(c - 'A' + 'a');
        }
        a[n++] = (char)c;
    }
    if (sz[n] != '\0') {
        return 0;
    }
    while (n > 0 && a[n - 1] == '.') {
        n--;
    }
    a[n] = '\0';
    if (strcmp(a, "localhost") == 0 || strcmp(a, "localhost.localdomain") == 0) {
        return 1;
    }
    /* 127.0.0.1 as qname (rare) */
    if (strcmp(a, "127.0.0.1") == 0) {
        return 1;
    }
    (void)i;
    return 0;
}

/*
 * greppable: CGJ_RES_SOFT_LOCALHOST_A
 * Build a minimal positive response: 1 question + 1 A answer (127.0.0.1).
 * Returns message length or -1.
 */
static int
soft_mk_localhost_a(const char *szDname, unsigned char *pAns, int nAnslen)
{
    size_t i;
    size_t nLabel;
    const char *p;
    int nOff = 0;
    int nQnameOff;
    unsigned char aName[128];
    int nName = 0;

    if (pAns == NULL || nAnslen < 64) {
        errno = EINVAL;
        return -1;
    }

    /* Encode qname from szDname (ASCII labels). */
    p = szDname;
    while (*p != '\0') {
        nLabel = 0;
        while (p[nLabel] != '\0' && p[nLabel] != '.') {
            nLabel++;
        }
        if (nLabel == 0 || nLabel > 63 || nName + 1 + (int)nLabel + 1 > 120) {
            errno = EINVAL;
            return -1;
        }
        aName[nName++] = (unsigned char)nLabel;
        for (i = 0; i < nLabel; i++) {
            unsigned char c = (unsigned char)p[i];

            if (c >= 'A' && c <= 'Z') {
                c = (unsigned char)(c - 'A' + 'a');
            }
            aName[nName++] = c;
        }
        p += nLabel;
        if (*p == '.') {
            p++;
        }
    }
    aName[nName++] = 0; /* root */

    /* header: id=0, QR|AA|RD|RA, qd=1 an=1 */
    if (nAnslen < 12 + nName + 4 + 2 + 2 + 2 + 4 + 2 + 4) {
        errno = ENOSPC;
        return -1;
    }
    memset(pAns, 0, (size_t)nAnslen);
    pAns[0] = 0;
    pAns[1] = 0;
    pAns[2] = 0x85; /* QR | AA | RD */
    pAns[3] = 0x80; /* RA */
    pAns[4] = 0;
    pAns[5] = 1; /* qdcount */
    pAns[6] = 0;
    pAns[7] = 1; /* ancount */
    nOff = 12;
    nQnameOff = nOff;
    memcpy(pAns + nOff, aName, (size_t)nName);
    nOff += nName;
    /* QTYPE A, QCLASS IN */
    pAns[nOff++] = 0;
    pAns[nOff++] = (unsigned char)CGJ_NS_T_A;
    pAns[nOff++] = 0;
    pAns[nOff++] = (unsigned char)CGJ_NS_C_IN;
    /* Answer: compression pointer to qname */
    pAns[nOff++] = (unsigned char)(0xc0 | ((nQnameOff >> 8) & 0x3f));
    pAns[nOff++] = (unsigned char)(nQnameOff & 0xff);
    pAns[nOff++] = 0;
    pAns[nOff++] = (unsigned char)CGJ_NS_T_A;
    pAns[nOff++] = 0;
    pAns[nOff++] = (unsigned char)CGJ_NS_C_IN;
    /* TTL 60 */
    pAns[nOff++] = 0;
    pAns[nOff++] = 0;
    pAns[nOff++] = 0;
    pAns[nOff++] = 60;
    /* RDLENGTH 4 */
    pAns[nOff++] = 0;
    pAns[nOff++] = 4;
    /* 127.0.0.1 */
    pAns[nOff++] = 127;
    pAns[nOff++] = 0;
    pAns[nOff++] = 0;
    pAns[nOff++] = 1;
    return nOff;
}

int
res_query(const char *szDname, int nClass, int nType, unsigned char *pAns,
          int nAnslen)
{
    if (soft_res_ready() != 0) {
        return -1;
    }
    if (szDname == NULL || pAns == NULL || nAnslen <= 0) {
        errno = EINVAL;
        return -1;
    }
    /* Soft positive path: localhost IN A only. */
    if (nClass == CGJ_NS_C_IN && nType == CGJ_NS_T_A &&
        soft_name_is_localhost(szDname)) {
        int n;

        n = soft_mk_localhost_a(szDname, pAns, nAnslen);
        if (n < 0) {
            return -1;
        }
        return n;
    }
    if (nClass == CGJ_NS_C_IN && nType == CGJ_NS_T_AAAA &&
        soft_name_is_localhost(szDname)) {
        /* Soft: no AAAA synthesised yet — signal no data */
        errno = ENODEV;
        return -1;
    }
    (void)nType;
    errno = ENOSYS;
    return -1;
}

int
res_search(const char *szDname, int nClass, int nType, unsigned char *pAns,
           int nAnslen)
{
    /* Soft: no search path / ndots — same as query after auto-init. */
    return res_query(szDname, nClass, nType, pAns, nAnslen);
}
