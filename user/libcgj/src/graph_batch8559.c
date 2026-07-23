/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8559: regex escape-letter ok predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_re_escape_ok_u_8559(uint32_t ch);
 *     - Return 1 if ch is a soft known escape letter / metachar that
 *       may follow a backslash in catalog patterns (n, t, r, \\, and
 *       the common metachars . * + ? ^ $ [ ] ( ) |), else 0. Catalog
 *       validator only; not an escape expander or matcher.
 *   uint32_t __gj_re_escape_ok_u_8559  (alias)
 *   __libcgj_batch8559_marker = "libcgj-batch8559"
 *
 * Exclusive continuum CREATE-ONLY (8551-8560: regex/class id stubs —
 * empty_ok, dot_id, star_id, plus_id, qmark_id, anchor_start_id,
 * anchor_end_id, class_ok, escape_ok, batch_id_8560). Unique
 * gj_re_escape_ok_u_8559 surface only; no multi-def. No parent wires.
 * No real regex engine. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8559_marker[] = "libcgj-batch8559";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8559_escape_ok — 1 when ch is in the soft known-escape set.
 * Compares against a fixed table of catalog-accepted bytes.
 */
static uint32_t
b8559_escape_ok(uint32_t u32Ch)
{
	/* Soft known escape / meta set (byte values only). */
	static const uint32_t au32Ok[] = {
		(uint32_t)'n', (uint32_t)'t', (uint32_t)'r',
		(uint32_t)'\\',
		(uint32_t)'.', (uint32_t)'*', (uint32_t)'+', (uint32_t)'?',
		(uint32_t)'^', (uint32_t)'$',
		(uint32_t)'[', (uint32_t)']',
		(uint32_t)'(', (uint32_t)')',
		(uint32_t)'|'
	};
	uint32_t u32I;
	uint32_t u32N;

	u32N = (uint32_t)(sizeof(au32Ok) / sizeof(au32Ok[0]));
	for (u32I = 0u; u32I < u32N; u32I++) {
		if (u32Ch == au32Ok[u32I]) {
			return 1u;
		}
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_re_escape_ok_u_8559 - 1 if ch is a known soft escape / meta byte.
 *
 * ch: candidate byte immediately after a soft backslash
 *
 * Returns 1 for catalog-accepted escape letters and metachars, else 0.
 * Soft escape-token catalog validator; does not expand escapes or
 * match. No parent wires.
 */
uint32_t
gj_re_escape_ok_u_8559(uint32_t u32Ch)
{
	(void)NULL;
	return b8559_escape_ok(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_re_escape_ok_u_8559(uint32_t u32Ch)
    __attribute__((alias("gj_re_escape_ok_u_8559")));
