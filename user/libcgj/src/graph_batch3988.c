/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3988: saturating size_t add.
 *
 * Surface (unique symbols):
 *   size_t gj_size_add_sat_u(size_t a, size_t b);
 *     - a + b with saturation at SIZE_MAX on overflow (no wrap).
 *   size_t __gj_size_add_sat_u  (alias)
 *   __libcgj_batch3988_marker = "libcgj-batch3988"
 *
 * Exclusive continuum CREATE-ONLY (3981-3990). Distinct from
 * gj_sat_add_u64 (batch377) / gj_u32_sat_add (batch923) /
 * gj_i64_add_sat_u (batch2041) — size_t surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3988_marker[] = "libcgj-batch3988";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3988_add_sat — unsigned size_t add; clamp to SIZE_MAX if a + b overflows.
 *
 * Detect overflow without wrapping: b > SIZE_MAX - a.
 */
static size_t
b3988_add_sat(size_t uA, size_t uB)
{
	if (uB > (SIZE_MAX - uA)) {
		return SIZE_MAX;
	}
	return uA + uB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_size_add_sat_u - size_t add with saturation at SIZE_MAX.
 *
 * No wrap on overflow. No parent wires.
 */
size_t
gj_size_add_sat_u(size_t uA, size_t uB)
{
	(void)NULL;
	return b3988_add_sat(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_size_add_sat_u(size_t uA, size_t uB)
    __attribute__((alias("gj_size_add_sat_u")));
