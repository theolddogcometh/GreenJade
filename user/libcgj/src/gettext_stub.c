/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * gettext identity stubs (return msgid; no catalog). Clean-room.
 */
#include <libintl.h>
#include <string.h>

static char g_aDomain[64] = "messages";
static char g_aDir[256] = "/usr/share/locale";
static char g_aCodeset[32] = "UTF-8";

char *
gettext(const char *szMsgid)
{
    return (char *)szMsgid;
}

char *
dgettext(const char *szDomain, const char *szMsgid)
{
    (void)szDomain;
    return (char *)szMsgid;
}

char *
dcgettext(const char *szDomain, const char *szMsgid, int nCategory)
{
    (void)szDomain;
    (void)nCategory;
    return (char *)szMsgid;
}

char *
ngettext(const char *szMsgid, const char *szMsgidPlural, unsigned long n)
{
    return (char *)((n == 1UL) ? szMsgid : szMsgidPlural);
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
    if (szDomain != NULL && szDomain[0] != '\0') {
        size_t n = strlen(szDomain);

        if (n >= sizeof(g_aDomain)) {
            n = sizeof(g_aDomain) - 1u;
        }
        memcpy(g_aDomain, szDomain, n);
        g_aDomain[n] = '\0';
    }
    return g_aDomain;
}

char *
bindtextdomain(const char *szDomain, const char *szDir)
{
    (void)szDomain;
    if (szDir != NULL) {
        size_t n = strlen(szDir);

        if (n >= sizeof(g_aDir)) {
            n = sizeof(g_aDir) - 1u;
        }
        memcpy(g_aDir, szDir, n);
        g_aDir[n] = '\0';
    }
    return g_aDir;
}

char *
bind_textdomain_codeset(const char *szDomain, const char *szCodeset)
{
    (void)szDomain;
    if (szCodeset != NULL) {
        size_t n = strlen(szCodeset);

        if (n >= sizeof(g_aCodeset)) {
            n = sizeof(g_aCodeset) - 1u;
        }
        memcpy(g_aCodeset, szCodeset, n);
        g_aCodeset[n] = '\0';
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
