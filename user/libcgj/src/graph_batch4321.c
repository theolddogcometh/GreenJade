/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4321: unsigned empty C-string predicate
 * (wave-unique rename).
 *
 * Surface (unique symbols):
 *   unsigned gj_str_is_empty_u2(const char *s);
 *     - Return 1u if s is NULL or s[0] == '\0', else 0u.
 *       Renamed from gj_str_is_empty_u (collision with batch2001).
 *   unsigned __gj_str_is_empty_u2  (alias)
 *   __libcgj_batch4321_marker = "libcgj-batch4321"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330: str_is_empty_u2,
 * str_eq_u2, str_neq_u2, str_startswith_u, str_endswith_u, str_ncmp_u2,
 * str_chr_idx_u, str_count_char_u2, str_hash_mix_u, batch_id_4330).
 * Distinct from gj_str_is_empty (batch731) and gj_str_is_empty_u
 * (batch2001) — unique gj_str_is_empty_u2 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4321_marker[] = "libcgj-batch4321";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b4321_empty(const char *sz)
{
	if (sz == NULL) {
		return 1u;
	}
	if (sz[0] == '\0') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_empty_u2 - 1u if s is NULL or the empty string, else 0u.
 *
 * s: NUL-terminated C string, or NULL
 *
 * Does not walk past the first character. Wave-unique rename:
 * gj_str_is_empty_u is owned by batch2001. Self-contained; no parent
 * wires.
 */
unsigned
gj_str_is_empty_u2(const char *s)
{
	(void)NULL;
	return b4321_empty(s);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_str_is_empty_u2(const char *s)
    __attribute__((alias("gj_str_is_empty_u2")));
