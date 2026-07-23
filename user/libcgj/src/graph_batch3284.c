/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3284: C-string suffix predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_str_has_suffix_u(const char *s, const char *sfx);
 *     - Return 1 if s ends with every octet of sfx (including when
 *       sfx is empty and s is non-NULL), else 0. NULL s or NULL sfx
 *       returns 0.
 *   int __gj_str_has_suffix_u  (alias)
 *   __libcgj_batch3284_marker = "libcgj-batch3284"
 *
 * Exclusive continuum CREATE-ONLY (3281-3290). Distinct from
 * gj_str_has_suffix (batch708), gj_ends_with (batch356), and
 * gj_str_ends (batch602) — unique _u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3284_marker[] = "libcgj-batch3284";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3284_strlen(const char *sz)
{
size_t cb;

if (sz == NULL) {
return 0u;
}
cb = 0u;
while (sz[cb] != '\0') {
cb++;
}
return cb;
}

static int
b3284_has_suffix(const char *sz, const char *szSfx)
{
size_t cbS;
size_t cbSfx;
size_t i;
size_t iBase;

if (sz == NULL || szSfx == NULL) {
return 0;
}

cbS = b3284_strlen(sz);
cbSfx = b3284_strlen(szSfx);
if (cbSfx > cbS) {
return 0;
}

iBase = cbS - cbSfx;
for (i = 0u; i < cbSfx; i++) {
if (sz[iBase + i] != szSfx[i]) {
return 0;
}
}
return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_has_suffix_u - true if s has sfx as a trailing substring.
 *
 * s:   haystack C string (NULL -> 0)
 * sfx: required suffix (NULL -> 0; empty string matches any non-NULL s)
 *
 * Comparison is unsigned-byte exact (no locale). No parent wires.
 */
int
gj_str_has_suffix_u(const char *s, const char *sfx)
{
(void)NULL;
return b3284_has_suffix(s, sfx);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_has_suffix_u(const char *s, const char *sfx)
    __attribute__((alias("gj_str_has_suffix_u")));
