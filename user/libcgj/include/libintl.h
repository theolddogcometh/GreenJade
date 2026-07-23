/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room gettext/libintl-shaped header (C locale stubs).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <libintl.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
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
