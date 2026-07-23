/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * newlocale / uselocale / duplocale / freelocale — C locale only bring-up.
 * Not GNU glibc (xlocale is public ABI surface only).
 */
#include <errno.h>
#include <locale.h>
#include <stddef.h>
#include <string.h>

/* Opaque locale object: only the static C locale is supported. */
struct __locale_struct {
    int nMagic;
};

#define CGJ_LOCALE_MAGIC 0x43474a4c /* 'CGJL' */

static struct __locale_struct g_localeC = { CGJ_LOCALE_MAGIC };
static locale_t g_pCurLocale = LC_GLOBAL_LOCALE;

static int
locale_is_c(const char *sz)
{
    if (sz == NULL || sz[0] == '\0') {
        return 1;
    }
    if (sz[0] == 'C' && sz[1] == '\0') {
        return 1;
    }
    if (sz[0] == 'P' && sz[1] == 'O' && sz[2] == 'S' && sz[3] == 'I' &&
        sz[4] == 'X' && sz[5] == '\0') {
        return 1;
    }
    return 0;
}

locale_t
newlocale(int nMask, const char *szLocale, locale_t base)
{
    (void)nMask;
    (void)base;

    if (!locale_is_c(szLocale)) {
        errno = ENOENT;
        return (locale_t)0;
    }
    return &g_localeC;
}

locale_t
duplocale(locale_t loc)
{
    if (loc == LC_GLOBAL_LOCALE || loc == NULL) {
        return &g_localeC;
    }
    if (loc->nMagic != CGJ_LOCALE_MAGIC) {
        errno = EINVAL;
        return (locale_t)0;
    }
    return &g_localeC;
}

void
freelocale(locale_t loc)
{
    (void)loc;
    /* static C locale — nothing to free */
}

locale_t
uselocale(locale_t newloc)
{
    locale_t old = g_pCurLocale;

    if (newloc == (locale_t)0) {
        /* query only */
        return old;
    }
    if (newloc == LC_GLOBAL_LOCALE) {
        g_pCurLocale = LC_GLOBAL_LOCALE;
        return old;
    }
    if (newloc->nMagic != CGJ_LOCALE_MAGIC) {
        errno = EINVAL;
        return (locale_t)0;
    }
    g_pCurLocale = newloc;
    return old;
}
