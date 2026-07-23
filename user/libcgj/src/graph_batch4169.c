/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4169: freestanding C-string length (wave-unique
 * rename).
 *
 * Surface (unique symbols):
 *   size_t gj_str_len_u2(const char *s);
 *     - Count octets in s before the first NUL. s == NULL -> 0.
 *       Renamed from gj_str_len_u (collision with batch2621).
 *   size_t __gj_str_len_u2  (alias)
 *   __libcgj_batch4169_marker = "libcgj-batch4169"
 *
 * CREATE-ONLY exclusive continuum wave (4161-4170). Unique
 * gj_str_len_u2 surface only; no multi-def. Distinct from
 * gj_str_len (batch411) and gj_str_len_u (batch2621). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4169_marker[] = "libcgj-batch4169";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4169_len(const char *sz)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}

	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_len_u2 - length of a NUL-terminated string (not counting the NUL).
 *
 * s: NUL-terminated string, or NULL
 *
 * Does not call libc strlen. Returns 0 when s is NULL. No parent wires.
 */
size_t
gj_str_len_u2(const char *s)
{
	(void)NULL;
	return b4169_len(s);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_len_u2(const char *s)
    __attribute__((alias("gj_str_len_u2")));
