/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * gettext identity stubs (return msgid; no catalog). Soft multi-domain
 * bind state for textdomain / bindtextdomain / codeset. Clean-room.
 */
#include <libintl.h>
#include <string.h>

#define GJ_GETTEXT_DOMAIN_MAX 8
#define GJ_GETTEXT_NAME_MAX   64
#define GJ_GETTEXT_DIR_MAX    256
#define GJ_GETTEXT_CODE_MAX   32

struct gj_gettext_domain {
    char aName[GJ_GETTEXT_NAME_MAX];
    char aDir[GJ_GETTEXT_DIR_MAX];
    char aCodeset[GJ_GETTEXT_CODE_MAX];
    int  fUsed;
};

static char g_aDomain[GJ_GETTEXT_NAME_MAX] = "messages";
static char g_aDir[GJ_GETTEXT_DIR_MAX] = "/usr/share/locale";
static char g_aCodeset[GJ_GETTEXT_CODE_MAX] = "UTF-8";
static struct gj_gettext_domain g_aDom[GJ_GETTEXT_DOMAIN_MAX];

static void
copy_trunc(char *szDst, size_t cb, const char *szSrc)
{
    size_t n;

    if (szDst == NULL || cb == 0) {
        return;
    }
    if (szSrc == NULL) {
        szDst[0] = '\0';
        return;
    }
    n = strlen(szSrc);
    if (n >= cb) {
        n = cb - 1u;
    }
    memcpy(szDst, szSrc, n);
    szDst[n] = '\0';
}

static struct gj_gettext_domain *
domain_find(const char *szDomain, int fCreate)
{
    int i;
    int nFree = -1;
    const char *szKey = (szDomain != NULL && szDomain[0] != '\0')
                            ? szDomain
                            : g_aDomain;

    for (i = 0; i < GJ_GETTEXT_DOMAIN_MAX; i++) {
        if (g_aDom[i].fUsed) {
            if (strcmp(g_aDom[i].aName, szKey) == 0) {
                return &g_aDom[i];
            }
        } else if (nFree < 0) {
            nFree = i;
        }
    }
    if (!fCreate || nFree < 0) {
        return NULL;
    }
    g_aDom[nFree].fUsed = 1;
    copy_trunc(g_aDom[nFree].aName, sizeof(g_aDom[nFree].aName), szKey);
    copy_trunc(g_aDom[nFree].aDir, sizeof(g_aDom[nFree].aDir), g_aDir);
    copy_trunc(g_aDom[nFree].aCodeset, sizeof(g_aDom[nFree].aCodeset),
               g_aCodeset);
    return &g_aDom[nFree];
}

char *
gettext(const char *szMsgid)
{
    if (szMsgid == NULL) {
        return (char *)"";
    }
    return (char *)szMsgid;
}

char *
dgettext(const char *szDomain, const char *szMsgid)
{
    (void)szDomain;
    return gettext(szMsgid);
}

char *
dcgettext(const char *szDomain, const char *szMsgid, int nCategory)
{
    (void)szDomain;
    (void)nCategory;
    return gettext(szMsgid);
}

char *
ngettext(const char *szMsgid, const char *szMsgidPlural, unsigned long n)
{
    /* Soft: n==1 singular; all other counts (incl. 0) use plural form. */
    if (n == 1UL) {
        return gettext(szMsgid);
    }
    if (szMsgidPlural != NULL) {
        return (char *)szMsgidPlural;
    }
    return gettext(szMsgid);
}

char *
dngettext(const char *szDomain, const char *szMsgid,
          const char *szMsgidPlural, unsigned long n)
{
    (void)szDomain;
    return ngettext(szMsgid, szMsgidPlural, n);
}

char *
dcngettext(const char *szDomain, const char *szMsgid,
           const char *szMsgidPlural, unsigned long n, int nCategory)
{
    (void)szDomain;
    (void)nCategory;
    return ngettext(szMsgid, szMsgidPlural, n);
}

char *
textdomain(const char *szDomain)
{
    if (szDomain != NULL) {
        if (szDomain[0] == '\0') {
            /* Empty string: leave current domain unchanged (glibc-shaped). */
            return g_aDomain;
        }
        copy_trunc(g_aDomain, sizeof(g_aDomain), szDomain);
        (void)domain_find(g_aDomain, 1);
    }
    return g_aDomain;
}

char *
bindtextdomain(const char *szDomain, const char *szDir)
{
    struct gj_gettext_domain *pDom;

    pDom = domain_find(szDomain, 1);
    if (szDir != NULL) {
        if (pDom != NULL) {
            copy_trunc(pDom->aDir, sizeof(pDom->aDir), szDir);
        }
        /* Also refresh global default dir for identity path. */
        if (szDomain == NULL ||
            (szDomain[0] != '\0' && strcmp(szDomain, g_aDomain) == 0)) {
            copy_trunc(g_aDir, sizeof(g_aDir), szDir);
        } else if (szDomain != NULL && szDomain[0] != '\0') {
            /* domain-specific only */
        } else {
            copy_trunc(g_aDir, sizeof(g_aDir), szDir);
        }
    }
    if (pDom != NULL) {
        return pDom->aDir;
    }
    return g_aDir;
}

char *
bind_textdomain_codeset(const char *szDomain, const char *szCodeset)
{
    struct gj_gettext_domain *pDom;

    pDom = domain_find(szDomain, 1);
    if (szCodeset != NULL) {
        if (pDom != NULL) {
            copy_trunc(pDom->aCodeset, sizeof(pDom->aCodeset), szCodeset);
        }
        if (szDomain == NULL || szDomain[0] == '\0' ||
            strcmp(szDomain, g_aDomain) == 0) {
            copy_trunc(g_aCodeset, sizeof(g_aCodeset), szCodeset);
        }
    }
    if (pDom != NULL) {
        return pDom->aCodeset;
    }
    return g_aCodeset;
}

char *
__dgettext(const char *szDomain, const char *szMsgid)
{
    return dgettext(szDomain, szMsgid);
}

char *
__dcgettext(const char *szDomain, const char *szMsgid, int nCategory)
{
    return dcgettext(szDomain, szMsgid, nCategory);
}

char *
__gettext(const char *szMsgid)
{
    return gettext(szMsgid);
}

char *
__ngettext(const char *szMsgid, const char *szMsgidPlural, unsigned long n)
{
    return ngettext(szMsgid, szMsgidPlural, n);
}

char *
__textdomain(const char *szDomain)
{
    return textdomain(szDomain);
}

char *
__bindtextdomain(const char *szDomain, const char *szDir)
{
    return bindtextdomain(szDomain, szDir);
}
