/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1973: seconds to milliseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sec_to_ms(uint64_t sec);
 *     — Return sec * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_sec_to_ms  (alias)
 *   __libcgj_batch1973_marker = "libcgj-batch1973"
 *
 * Time-duration exclusive pure-data wave (1971–1980). Distinct from
 * gj_secs_to_ms (batch484, plural name) — unique symbol. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1973_marker[] = "libcgj-batch1973";

/* Milliseconds per second (SI: 1 s = 1000 ms). */
#define B1973_MS_PER_SEC  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B1973_MS_PER_SEC. */
static uint64_t
b1973_mul_ms(uint64_t u64Sec)
{
	if (u64Sec > (UINT64_MAX / B1973_MS_PER_SEC)) {
		return UINT64_MAX;
	}
	return u64Sec * B1973_MS_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sec_to_ms — convert whole seconds to milliseconds (saturating).
 *
 * sec: seconds
 * Returns sec * 1000, or UINT64_MAX if the product would overflow.
 */
uint64_t
gj_sec_to_ms(uint64_t u64Sec)
{
	(void)NULL;
	return b1973_mul_ms(u64Sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sec_to_ms(uint64_t u64Sec)
    __attribute__((alias("gj_sec_to_ms")));
