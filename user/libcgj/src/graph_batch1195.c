/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1195: exponential retry backoff in microseconds.
 *
 * Surface (unique symbols):
 *   uint32_t gj_retry_backoff_us(unsigned attempt, uint32_t base_us,
 *                                uint32_t max_us);
 *     — Compute min(base_us * 2^attempt, max_us) with saturating
 *       multiply. attempt is 0-based. base_us==0 → 0. If max_us <
 *       base_us, returns max_us. Overflow clamps to max_us.
 *   uint32_t __gj_retry_backoff_us  (alias)
 *   __libcgj_batch1195_marker = "libcgj-batch1195"
 *
 * Pure integer backoff helper; unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1195_marker[] = "libcgj-batch1195";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_retry_backoff_us — exponential backoff, microsecond units.
 *
 * uAttempt: 0-based attempt index (0 → base_us).
 * u32BaseUs: base delay.
 * u32MaxUs:  hard ceiling (returned when shift would exceed or overflow).
 */
uint32_t
gj_retry_backoff_us(unsigned uAttempt, uint32_t u32BaseUs, uint32_t u32MaxUs)
{
	uint32_t u32Delay;
	unsigned i;

	if (u32BaseUs == 0u) {
		return 0u;
	}
	if (u32MaxUs < u32BaseUs) {
		return u32MaxUs;
	}
	u32Delay = u32BaseUs;
	for (i = 0u; i < uAttempt; i++) {
		/* Saturating *2: if high bit set, next shift overflows. */
		if (u32Delay > (u32MaxUs >> 1)) {
			return u32MaxUs;
		}
		u32Delay <<= 1;
		if (u32Delay >= u32MaxUs) {
			return u32MaxUs;
		}
	}
	return u32Delay;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_retry_backoff_us(unsigned uAttempt, uint32_t u32BaseUs,
    uint32_t u32MaxUs)
    __attribute__((alias("gj_retry_backoff_us")));
