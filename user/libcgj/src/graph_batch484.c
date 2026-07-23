/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch484: freestanding seconds → milliseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_secs_to_ms(uint64_t s);
 *     — Convert seconds to milliseconds: s * 1000, saturating at
 *       UINT64_MAX when the product would overflow.
 *   uint64_t __gj_secs_to_ms  (alias)
 *   __libcgj_batch484_marker = "libcgj-batch484"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch484_marker[] = "libcgj-batch484";

/* Floor(UINT64_MAX / 1000): largest s with s * 1000 still in range. */
#define B484_MAX_SECS  (UINT64_C(0xffffffffffffffff) / 1000u)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_secs_to_ms — seconds to milliseconds with saturation.
 *
 * s: seconds (unsigned 64-bit)
 *
 * Returns s * 1000 if the product fits in uint64_t, else UINT64_MAX.
 * s == 0 → 0; s <= B484_MAX_SECS → exact; larger → saturate.
 */
uint64_t
gj_secs_to_ms(uint64_t s)
{
	if (s > B484_MAX_SECS) {
		return UINT64_C(0xffffffffffffffff);
	}
	return s * 1000u;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_secs_to_ms(uint64_t s)
    __attribute__((alias("gj_secs_to_ms")));
