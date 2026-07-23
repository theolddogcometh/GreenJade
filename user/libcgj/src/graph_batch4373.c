/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4373: seconds to milliseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_sec_to_ms_u(uint64_t sec);
 *     - Return sec * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_sec_to_ms_u  (alias)
 *   __libcgj_batch4373_marker = "libcgj-batch4373"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380). Distinct from
 * gj_sec_to_ms (batch1973) and gj_secs_to_ms (batch484) — unique
 * gj_sec_to_ms_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4373_marker[] = "libcgj-batch4373";

/* Milliseconds per second (SI: 1 s = 1000 ms). */
#define B4373_MS_PER_SEC  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B4373_MS_PER_SEC. */
static uint64_t
b4373_mul_ms(uint64_t u64Sec)
{
	if (u64Sec > (UINT64_MAX / B4373_MS_PER_SEC)) {
		return UINT64_MAX;
	}
	return u64Sec * B4373_MS_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sec_to_ms_u - convert whole seconds to milliseconds (saturating).
 *
 * sec: seconds
 *
 * Returns sec * 1000, or UINT64_MAX if the product would overflow.
 * Self-contained; no parent wires.
 */
uint64_t
gj_sec_to_ms_u(uint64_t u64Sec)
{
	(void)NULL;
	return b4373_mul_ms(u64Sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sec_to_ms_u(uint64_t u64Sec)
    __attribute__((alias("gj_sec_to_ms_u")));
