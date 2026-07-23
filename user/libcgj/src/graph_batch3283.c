/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3283: C-string prefix predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_str_has_prefix_u(const char *s, const char *pfx);
 *     - Return 1 if s begins with every octet of pfx (including when
 *       pfx is empty and s is non-NULL), else 0. NULL s or NULL pfx
 *       returns 0.
 *   int __gj_str_has_prefix_u  (alias)
 *   __libcgj_batch3283_marker = "libcgj-batch3283"
 *
 * Exclusive continuum CREATE-ONLY (3281-3290). Distinct from
 * gj_str_has_prefix (batch707), gj_starts_with (batch356), and
 * gj_str_starts (batch601) — unique _u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3283_marker[] = "libcgj-batch3283";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3283_has_prefix(const char *sz, const char *szPfx)
{
size_t i;

if (sz == NULL || szPfx == NULL) {
return 0;
}

i = 0u;
while (szPfx[i] != '\0') {
if (sz[i] == '\0' || sz[i] != szPfx[i]) {
return 0;
}
i++;
}
return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_has_prefix_u - true if s has pfx as a leading substring (exact).
 *
 * s:   haystack C string (NULL -> 0)
 * pfx: required prefix (NULL -> 0; empty string -> 1 when s is non-NULL)
 *
 * Comparison is unsigned-byte exact (no locale). No parent wires.
 */
int
gj_str_has_prefix_u(const char *s, const char *pfx)
{
(void)NULL;
return b3283_has_prefix(s, pfx);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_has_prefix_u(const char *s, const char *pfx)
    __attribute__((alias("gj_str_has_prefix_u")));
