/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1979: exponential backoff in milliseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_backoff_exp_ms(unsigned attempt, uint64_t base_ms,
 *                              uint64_t max_ms);
 *     — Compute min(base_ms * 2^attempt, max_ms) with saturating
 *       multiply. attempt is 0-based. base_ms == 0 → 0. If max_ms <
 *       base_ms, returns max_ms. Overflow clamps to max_ms.
 *   uint64_t __gj_backoff_exp_ms  (alias)
 *   __libcgj_batch1979_marker = "libcgj-batch1979"
 *
 * Time-duration exclusive pure-data wave (1971–1980). Distinct from
 * gj_retry_backoff_us (batch1195, microsecond uint32). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1979_marker[] = "libcgj-batch1979";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1979_backoff — exponential delay capped at max_ms.
 *
 * attempt 0 → base_ms; each step doubles until max_ms.
 */
static uint64_t
b1979_backoff(unsigned uAttempt, uint64_t u64BaseMs, uint64_t u64MaxMs)
{
	uint64_t u64Delay;
	unsigned i;

	if (u64BaseMs == 0ull) {
		return 0ull;
	}
	if (u64MaxMs < u64BaseMs) {
		return u64MaxMs;
	}
	u64Delay = u64BaseMs;
	for (i = 0u; i < uAttempt; i++) {
		/* Saturating *2: if high bit set, next shift overflows. */
		if (u64Delay > (u64MaxMs >> 1)) {
			return u64MaxMs;
		}
		u64Delay <<= 1;
		if (u64Delay >= u64MaxMs) {
			return u64MaxMs;
		}
	}
	return u64Delay;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_backoff_exp_ms — exponential backoff, millisecond units.
 *
 * attempt: 0-based attempt index (0 → base_ms)
 * base_ms: base delay in milliseconds
 * max_ms:  hard ceiling (returned when shift would exceed or overflow)
 */
uint64_t
gj_backoff_exp_ms(unsigned uAttempt, uint64_t u64BaseMs, uint64_t u64MaxMs)
{
	(void)NULL;
	return b1979_backoff(uAttempt, u64BaseMs, u64MaxMs);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_backoff_exp_ms(unsigned uAttempt, uint64_t u64BaseMs,
    uint64_t u64MaxMs)
    __attribute__((alias("gj_backoff_exp_ms")));
