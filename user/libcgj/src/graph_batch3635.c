/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3635: string-repeat buffer-need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_str_repeat_need_u(const char *s, size_t count);
 *     - Bytes needed (including trailing NUL) for the string that
 *       gj_str_repeat (batch1081) would write for (s, count). count == 0
 *       → 1 (empty + NUL). NULL s with count > 0 → 0. Empty s → 1.
 *       Overflow of size_t product → 0.
 *   size_t __gj_str_repeat_need_u  (alias)
 *   __libcgj_batch3635_marker = "libcgj-batch3635"
 *
 * Exclusive continuum CREATE-ONLY (3631-3640). Distinct from
 * gj_str_repeat (batch1081 write) — length-only surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3635_marker[] = "libcgj-batch3635";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3635_len(const char *sz)
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

/*
 * Bytes needed (incl. trailing NUL) for s repeated count times.
 * count == 0 → 1. NULL s with count > 0 → 0. Overflow → 0.
 */
static size_t
b3635_repeat_need(const char *sz, size_t cCount)
{
	size_t cbSrc;
	size_t nProd;

	if (cCount == 0u) {
		return 1u;
	}
	if (sz == NULL) {
		return 0u;
	}

	cbSrc = b3635_len(sz);
	if (cbSrc == 0u) {
		return 1u;
	}

	/* cbSrc * cCount + 1 must not wrap */
	if (cCount > (SIZE_MAX - 1u) / cbSrc) {
		return 0u;
	}
	nProd = cbSrc * cCount;
	return nProd + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_repeat_need_u - bytes needed (incl. NUL) for s * count.
 *
 * s:     subject string (NULL with count > 0 → 0)
 * count: number of repetitions (0 → 1 for empty result)
 *
 * Examples (return includes trailing NUL):
 *   ("ab", 3) → 7  ("ababab" + NUL)
 *   ("x", 0)  → 1  ("" + NUL)
 *   ("", 5)   → 1
 *   (NULL, 2) → 0
 *
 * Length-only; does not write a buffer. No parent wires.
 */
size_t
gj_str_repeat_need_u(const char *s, size_t count)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3635_repeat_need(s, count);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_repeat_need_u(const char *s, size_t count)
    __attribute__((alias("gj_str_repeat_need_u")));
