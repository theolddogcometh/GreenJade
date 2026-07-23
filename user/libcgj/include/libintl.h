/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room gettext/libintl-shaped header (C locale stubs).
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

char *gettext(const char *szMsgid);
char *dgettext(const char *szDomain, const char *szMsgid);
char *dcgettext(const char *szDomain, const char *szMsgid, int nCategory);
char *ngettext(const char *szMsgid, const char *szMsgidPlural, unsigned long n);
char *dngettext(const char *szDomain, const char *szMsgid,
                const char *szMsgidPlural, unsigned long n);
char *dcngettext(const char *szDomain, const char *szMsgid,
                 const char *szMsgidPlural, unsigned long n, int nCategory);
char *textdomain(const char *szDomain);
char *bindtextdomain(const char *szDomain, const char *szDir);
char *bind_textdomain_codeset(const char *szDomain, const char *szCodeset);

#ifdef __cplusplus
}
#endif
