/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * newlocale / uselocale / duplocale / freelocale — C locale only bring-up.
 * Not GNU glibc (xlocale is public ABI surface only).
 *
 * greppable: CGJ_XLOCALE_SOFT_C_UTF8
 * greppable: CGJ_XLOCALE_SOFT_EN_US
 * greppable: CGJ_XLOCALE_SOFT_MASK
 * greppable: CGJ_XLOCALE_SOFT_BASE
 *
 * Soft deepen: accept C.UTF-8 / POSIX / common en_US.UTF-8 as C-equivalent,
 * mask validation, base reuse when mask is 0.
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
soft_eq_ci_ascii(const char *szA, const char *szB)
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

static int
locale_is_c(const char *sz)
{
    if (sz == NULL || sz[0] == '\0') {
        return 1;
    }
    if (sz[0] == 'C' && sz[1] == '\0') {
        return 1;
    }
    if (sz[0] == 'c' && sz[1] == '\0') {
        return 1;
    }
    if (soft_eq_ci_ascii(sz, "POSIX")) {
        return 1;
    }
    /* greppable: CGJ_XLOCALE_SOFT_C_UTF8 */
    if (soft_eq_ci_ascii(sz, "C.UTF-8") || soft_eq_ci_ascii(sz, "C.utf8") ||
        soft_eq_ci_ascii(sz, "C.UTF8")) {
        return 1;
    }
    /* greppable: CGJ_XLOCALE_SOFT_EN_US — desktop soft map to C */
    if (soft_eq_ci_ascii(sz, "en_US.UTF-8") ||
        soft_eq_ci_ascii(sz, "en_US.utf8") ||
        soft_eq_ci_ascii(sz, "en_US") ||
        soft_eq_ci_ascii(sz, "en_US.UTF8")) {
        return 1;
    }
    return 0;
}

static int
soft_mask_ok(int nMask)
{
    /* greppable: CGJ_XLOCALE_SOFT_MASK */
    if (nMask == 0) {
        return 1;
    }
    if (nMask & ~LC_ALL_MASK) {
        return 0;
    }
    return 1;
}

locale_t
newlocale(int nMask, const char *szLocale, locale_t base)
{
    if (!soft_mask_ok(nMask)) {
        errno = EINVAL;
        return (locale_t)0;
    }
    /* greppable: CGJ_XLOCALE_SOFT_BASE */
    if (nMask == 0) {
        if (base == LC_GLOBAL_LOCALE || base == NULL) {
            return &g_localeC;
        }
        if (base->nMagic != CGJ_LOCALE_MAGIC) {
            errno = EINVAL;
            return (locale_t)0;
        }
        return base;
    }
    if (!locale_is_c(szLocale)) {
        errno = ENOENT;
        return (locale_t)0;
    }
    (void)base; /* all categories C — ignore base merge soft */
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
