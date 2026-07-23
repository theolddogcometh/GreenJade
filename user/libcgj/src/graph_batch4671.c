/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4671: freestanding bounded C-string length
 * (string-measure wave, collision rename).
 *
 * Surface (unique symbols):
 *   size_t gj_str_nlen_u2(const char *s, size_t maxlen);
 *     - Count octets in s before the first NUL, stopping at maxlen
 *       (strnlen-like). s == NULL → 0. Never reads past s[maxlen-1].
 *       Renamed from gj_str_nlen_u (collision with batch2204).
 *   size_t __gj_str_nlen_u2  (alias)
 *   __libcgj_batch4671_marker = "libcgj-batch4671"
 *
 * Exclusive continuum CREATE-ONLY (4671-4680: nlen_u2, has_prefix_u2,
 * has_suffix_u2, contains_char_u2, count_chars_u, is_ws_u,
 * first_nonws_u, last_nonws_u, hash32_u, batch_id_4680). Distinct from
 * gj_str_nlen_u (batch2204) / gj_str_nlen (batch631) / gj_str_nlength_u
 * (batch2752) — unique gj_str_nlen_u2 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4671_marker[] = "libcgj-batch4671";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4671_nlen(const char *sz, size_t cMax)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}

	n = 0u;
	while (n < cMax && sz[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_nlen_u2 - bounded length of a NUL-terminated string.
 *
 * s:      NUL-terminated string, or NULL
 * maxlen: maximum number of octets to examine
 *
 * Returns the number of octets before the first NUL, or maxlen if no
 * NUL appears in the first maxlen octets. Returns 0 when s is NULL.
 * Does not call libc strnlen/strlen. No parent wires.
 */
size_t
gj_str_nlen_u2(const char *s, size_t maxlen)
{
	(void)NULL;
	return b4671_nlen(s, maxlen);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_nlen_u2(const char *s, size_t maxlen)
    __attribute__((alias("gj_str_nlen_u2")));
