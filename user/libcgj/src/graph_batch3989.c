/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3989: saturating size_t subtract.
 *
 * Surface (unique symbols):
 *   size_t gj_size_sub_sat_u(size_t a, size_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *   size_t __gj_size_sub_sat_u  (alias)
 *   __libcgj_batch3989_marker = "libcgj-batch3989"
 *
 * Exclusive continuum CREATE-ONLY (3981-3990). Distinct from
 * gj_sat_sub_u64 (batch377) / gj_u64_sub_sat (batch2032) /
 * gj_size_add_sat_u (batch3988) — size_t surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3989_marker[] = "libcgj-batch3989";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3989_sub_sat — unsigned size_t subtract; clamp to 0 if a < b.
 */
static size_t
b3989_sub_sat(size_t uA, size_t uB)
{
	if (uA < uB) {
		return 0u;
	}
	return uA - uB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_size_sub_sat_u - size_t subtract with saturation at 0.
 *
 * No wrap on underflow. No parent wires.
 */
size_t
gj_size_sub_sat_u(size_t uA, size_t uB)
{
	(void)NULL;
	return b3989_sub_sat(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_size_sub_sat_u(size_t uA, size_t uB)
    __attribute__((alias("gj_size_sub_sat_u")));
