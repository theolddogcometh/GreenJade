/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3281: empty C-string predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_str_empty_p_u(const char *s);
 *     - Return 1 if s is NULL or s[0] == '\0', else 0.
 *   int __gj_str_empty_p_u  (alias)
 *   __libcgj_batch3281_marker = "libcgj-batch3281"
 *
 * Exclusive continuum CREATE-ONLY (3281-3290): str_empty_p_u,
 * str_nonempty_p_u, str_has_prefix_u, str_has_suffix_u,
 * str_contains_char_u, str_contains_str_u, str_index_of_u,
 * str_last_index_of_u, str_count_substr_u, batch_id_3290.
 * Distinct from gj_str_is_empty (batch731), gj_str_is_empty_u
 * (batch2001), and gj_str_nonempty_p (batch2002) — unique _p_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3281_marker[] = "libcgj-batch3281";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3281_empty_p(const char *sz)
{
if (sz == NULL) {
return 1;
}
if (sz[0] == '\0') {
return 1;
}
return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_empty_p_u - 1 if s is NULL or the empty string, else 0.
 *
 * s: NUL-terminated C string, or NULL
 *
 * Does not walk past the first character. No parent wires.
 */
int
gj_str_empty_p_u(const char *s)
{
(void)NULL;
return b3281_empty_p(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_empty_p_u(const char *s)
    __attribute__((alias("gj_str_empty_p_u")));
