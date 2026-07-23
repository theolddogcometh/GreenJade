/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * argz / envz vectors (NUL-separated string lists). Clean-room.
 */
#include <argz.h>
#include <envz.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

size_t
argz_count(const char *pArgz, size_t cbArgz)
{
    size_t n = 0;
    size_t i = 0;

    if (pArgz == NULL || cbArgz == 0) {
        return 0;
    }
    while (i < cbArgz) {
        n++;
        while (i < cbArgz && pArgz[i] != '\0') {
            i++;
        }
        if (i < cbArgz) {
            i++; /* skip NUL */
        }
    }
    return n;
}

size_t
__argz_count(const char *pArgz, size_t cbArgz)
{
    return argz_count(pArgz, cbArgz);
}

char *
argz_next(const char *pArgz, size_t cbArgz, const char *pEntry)
{
    const char *pEnd;
    const char *p;

    if (pArgz == NULL || cbArgz == 0) {
        return NULL;
    }
    pEnd = pArgz + cbArgz;
    if (pEntry == NULL) {
        return (char *)pArgz;
    }
    if (pEntry < pArgz || pEntry >= pEnd) {
        return NULL;
    }
    p = pEntry;
    while (p < pEnd && *p != '\0') {
        p++;
    }
    if (p < pEnd) {
        p++; /* past NUL */
    }
    if (p >= pEnd) {
        return NULL;
    }
    return (char *)p;
}

char *
__argz_next(const char *pArgz, size_t cbArgz, const char *pEntry)
{
    return argz_next(pArgz, cbArgz, pEntry);
}

void
argz_stringify(char *pArgz, size_t cbArgz, int nSep)
{
    size_t i;

    if (pArgz == NULL || cbArgz == 0) {
        return;
    }
    for (i = 0; i + 1 < cbArgz; i++) {
        if (pArgz[i] == '\0') {
            pArgz[i] = (char)nSep;
        }
    }
}

void
__argz_stringify(char *pArgz, size_t cbArgz, int nSep)
{
    argz_stringify(pArgz, cbArgz, nSep);
}

void
argz_extract(const char *pArgz, size_t cbArgz, char **ppArgv)
{
    size_t i = 0;
    size_t n = 0;

    if (ppArgv == NULL) {
        return;
    }
    if (pArgz == NULL || cbArgz == 0) {
        ppArgv[0] = NULL;
        return;
    }
    while (i < cbArgz) {
        ppArgv[n++] = (char *)(pArgz + i);
        while (i < cbArgz && pArgz[i] != '\0') {
            i++;
        }
        if (i < cbArgz) {
            i++;
        }
    }
    ppArgv[n] = NULL;
}

error_t
argz_create(char *const argv[], char **ppArgz, size_t *pcbArgz)
{
    size_t cb = 0;
    size_t i;
    char *p;
    char *q;

    if (ppArgz == NULL || pcbArgz == NULL) {
        return EINVAL;
    }
    *ppArgz = NULL;
    *pcbArgz = 0;
    if (argv == NULL) {
        return 0;
    }
    for (i = 0; argv[i] != NULL; i++) {
        cb += strlen(argv[i]) + 1u;
    }
    if (cb == 0) {
        return 0;
    }
    p = (char *)malloc(cb);
    if (p == NULL) {
        return ENOMEM;
    }
    q = p;
    for (i = 0; argv[i] != NULL; i++) {
        size_t n = strlen(argv[i]) + 1u;

        memcpy(q, argv[i], n);
        q += n;
    }
    *ppArgz = p;
    *pcbArgz = cb;
    return 0;
}

error_t
argz_create_sep(const char *sz, int nSep, char **ppArgz, size_t *pcbArgz)
{
    size_t cb;
    char *p;
    size_t i;

    if (ppArgz == NULL || pcbArgz == NULL) {
        return EINVAL;
    }
    *ppArgz = NULL;
    *pcbArgz = 0;
    if (sz == NULL || *sz == '\0') {
        return 0;
    }
    cb = strlen(sz) + 1u;
    p = (char *)malloc(cb);
    if (p == NULL) {
        return ENOMEM;
    }
    memcpy(p, sz, cb);
    for (i = 0; i + 1 < cb; i++) {
        if (p[i] == (char)nSep) {
            p[i] = '\0';
        }
    }
    *ppArgz = p;
    *pcbArgz = cb;
    return 0;
}

error_t
argz_append(char **ppArgz, size_t *pcbArgz, const char *pBuf, size_t cbBuf)
{
    char *p;
    size_t cbOld;

    if (ppArgz == NULL || pcbArgz == NULL) {
        return EINVAL;
    }
    if (pBuf == NULL || cbBuf == 0) {
        return 0;
    }
    cbOld = *pcbArgz;
    p = (char *)realloc(*ppArgz, cbOld + cbBuf);
    if (p == NULL) {
        return ENOMEM;
    }
    memcpy(p + cbOld, pBuf, cbBuf);
    *ppArgz = p;
    *pcbArgz = cbOld + cbBuf;
    return 0;
}

error_t
argz_add(char **ppArgz, size_t *pcbArgz, const char *sz)
{
    if (sz == NULL) {
        return EINVAL;
    }
    return argz_append(ppArgz, pcbArgz, sz, strlen(sz) + 1u);
}

error_t
argz_add_sep(char **ppArgz, size_t *pcbArgz, const char *sz, int nSep)
{
    char *pTmp = NULL;
    size_t cbTmp = 0;
    error_t e;

    e = argz_create_sep(sz, nSep, &pTmp, &cbTmp);
    if (e != 0) {
        return e;
    }
    if (cbTmp == 0) {
        return 0;
    }
    e = argz_append(ppArgz, pcbArgz, pTmp, cbTmp);
    free(pTmp);
    return e;
}

void
argz_delete(char **ppArgz, size_t *pcbArgz, char *pEntry)
{
    size_t off;
    size_t nLen;
    size_t cb;
    char *p;

    if (ppArgz == NULL || pcbArgz == NULL || *ppArgz == NULL || pEntry == NULL) {
        return;
    }
    p = *ppArgz;
    cb = *pcbArgz;
    if (pEntry < p || pEntry >= p + cb) {
        return;
    }
    off = (size_t)(pEntry - p);
    nLen = strlen(pEntry) + 1u;
    if (off + nLen > cb) {
        return;
    }
    memmove(pEntry, pEntry + nLen, cb - off - nLen);
    *pcbArgz = cb - nLen;
    if (*pcbArgz == 0) {
        free(p);
        *ppArgz = NULL;
    }
}

error_t
argz_insert(char **ppArgz, size_t *pcbArgz, char *pBefore, const char *sz)
{
    size_t nLen;
    size_t off;
    size_t cb;
    char *p;
    char *pNew;

    if (ppArgz == NULL || pcbArgz == NULL || sz == NULL) {
        return EINVAL;
    }
    nLen = strlen(sz) + 1u;
    cb = *pcbArgz;
    p = *ppArgz;
    if (pBefore == NULL) {
        return argz_append(ppArgz, pcbArgz, sz, nLen);
    }
    if (p == NULL || pBefore < p || pBefore > p + cb) {
        return EINVAL;
    }
    off = (size_t)(pBefore - p);
    pNew = (char *)realloc(p, cb + nLen);
    if (pNew == NULL) {
        return ENOMEM;
    }
    memmove(pNew + off + nLen, pNew + off, cb - off);
    memcpy(pNew + off, sz, nLen);
    *ppArgz = pNew;
    *pcbArgz = cb + nLen;
    return 0;
}

error_t
argz_replace(char **ppArgz, size_t *pcbArgz, const char *szStr,
             const char *szWith, unsigned *pnReplace)
{
    char *p;
    size_t cb;
    char *pEnt;
    unsigned n = 0;
    size_t nStr;
    size_t nWith;

    if (ppArgz == NULL || pcbArgz == NULL || szStr == NULL) {
        return EINVAL;
    }
    if (szWith == NULL) {
        szWith = "";
    }
    nStr = strlen(szStr);
    nWith = strlen(szWith);
    p = *ppArgz;
    cb = *pcbArgz;
    pEnt = NULL;
    while ((pEnt = argz_next(p, cb, pEnt)) != NULL) {
        char *pHit;
        char *pScan = pEnt;

        while ((pHit = strstr(pScan, szStr)) != NULL) {
            size_t offHit = (size_t)(pHit - p);
            size_t offEnt = (size_t)(pEnt - p);
            size_t nEnt = strlen(pEnt) + 1u;
            size_t nNewEnt;
            size_t nPrefix;
            size_t nSuffix;
            char *pNew;
            char *pNewEnt;

            nPrefix = (size_t)(pHit - pEnt);
            nSuffix = nEnt - nPrefix - nStr - 1u;
            nNewEnt = nPrefix + nWith + nSuffix + 1u;
            pNew = (char *)malloc(cb - nEnt + nNewEnt);
            if (pNew == NULL) {
                return ENOMEM;
            }
            memcpy(pNew, p, offEnt);
            pNewEnt = pNew + offEnt;
            memcpy(pNewEnt, pEnt, nPrefix);
            memcpy(pNewEnt + nPrefix, szWith, nWith);
            memcpy(pNewEnt + nPrefix + nWith, pHit + nStr, nSuffix + 1u);
            memcpy(pNewEnt + nNewEnt, p + offEnt + nEnt,
                   cb - offEnt - nEnt);
            free(p);
            p = pNew;
            cb = cb - nEnt + nNewEnt;
            *ppArgz = p;
            *pcbArgz = cb;
            n++;
            pEnt = pNewEnt;
            pScan = pNewEnt + nPrefix + nWith;
            (void)offHit;
        }
    }
    if (pnReplace != NULL) {
        *pnReplace = n;
    }
    return 0;
}

/* ---- envz (NAME or NAME=value entries) ---------------------------------- */

static int
envz_name_eq(const char *pEnt, const char *szName)
{
    size_t n;

    if (pEnt == NULL || szName == NULL) {
        return 0;
    }
    n = strlen(szName);
    if (strncmp(pEnt, szName, n) != 0) {
        return 0;
    }
    return pEnt[n] == '\0' || pEnt[n] == '=';
}

char *
envz_entry(const char *pEnvz, size_t cbEnvz, const char *szName)
{
    char *p = NULL;

    while ((p = argz_next(pEnvz, cbEnvz, p)) != NULL) {
        if (envz_name_eq(p, szName)) {
            return p;
        }
    }
    return NULL;
}

char *
envz_get(const char *pEnvz, size_t cbEnvz, const char *szName)
{
    char *p = envz_entry(pEnvz, cbEnvz, szName);
    char *pEq;

    if (p == NULL) {
        return NULL;
    }
    pEq = strchr(p, '=');
    if (pEq == NULL) {
        return NULL; /* null entry */
    }
    return pEq + 1;
}

void
envz_remove(char **ppEnvz, size_t *pcbEnvz, const char *szName)
{
    char *p;

    if (ppEnvz == NULL || pcbEnvz == NULL) {
        return;
    }
    p = envz_entry(*ppEnvz, *pcbEnvz, szName);
    if (p != NULL) {
        argz_delete(ppEnvz, pcbEnvz, p);
    }
}

error_t
envz_add(char **ppEnvz, size_t *pcbEnvz, const char *szName,
         const char *szValue)
{
    size_t nName;
    size_t nVal;
    size_t nBuf;
    char *pBuf;
    error_t e;

    if (szName == NULL) {
        return EINVAL;
    }
    envz_remove(ppEnvz, pcbEnvz, szName);
    nName = strlen(szName);
    if (szValue == NULL) {
        return argz_add(ppEnvz, pcbEnvz, szName);
    }
    nVal = strlen(szValue);
    nBuf = nName + 1u + nVal + 1u;
    pBuf = (char *)malloc(nBuf);
    if (pBuf == NULL) {
        return ENOMEM;
    }
    memcpy(pBuf, szName, nName);
    pBuf[nName] = '=';
    memcpy(pBuf + nName + 1u, szValue, nVal + 1u);
    e = argz_append(ppEnvz, pcbEnvz, pBuf, nBuf);
    free(pBuf);
    return e;
}

error_t
envz_merge(char **ppEnvz, size_t *pcbEnvz, const char *pEnvz2, size_t cbEnvz2,
           int nOverride)
{
    char *p = NULL;

    while ((p = argz_next(pEnvz2, cbEnvz2, p)) != NULL) {
        char aName[256];
        const char *pEq = strchr(p, '=');
        size_t nName;
        char *pExist;

        if (pEq != NULL) {
            nName = (size_t)(pEq - p);
        } else {
            nName = strlen(p);
        }
        if (nName >= sizeof(aName)) {
            nName = sizeof(aName) - 1u;
        }
        memcpy(aName, p, nName);
        aName[nName] = '\0';
        pExist = envz_entry(*ppEnvz, *pcbEnvz, aName);
        if (pExist != NULL && !nOverride) {
            continue;
        }
        if (pEq != NULL) {
            error_t e = envz_add(ppEnvz, pcbEnvz, aName, pEq + 1);

            if (e != 0) {
                return e;
            }
        } else {
            error_t e = envz_add(ppEnvz, pcbEnvz, aName, NULL);

            if (e != 0) {
                return e;
            }
        }
    }
    return 0;
}

void
envz_strip(char **ppEnvz, size_t *pcbEnvz)
{
    char *p = NULL;

    if (ppEnvz == NULL || pcbEnvz == NULL) {
        return;
    }
    for (;;) {
        int fFound = 0;

        p = NULL;
        while ((p = argz_next(*ppEnvz, *pcbEnvz, p)) != NULL) {
            if (strchr(p, '=') == NULL) {
                argz_delete(ppEnvz, pcbEnvz, p);
                fFound = 1;
                break;
            }
        }
        if (!fFound) {
            break;
        }
    }
}
