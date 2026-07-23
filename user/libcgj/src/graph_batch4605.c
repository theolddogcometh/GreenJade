/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4605: remaining ticks until deadline.
 *
 * Surface (unique symbols):
 *   uint64_t gj_tick_remaining_u(uint64_t deadline, uint64_t now);
 *     - Modular remaining distance deadline - now when not expired;
 *       returns 0 once the deadline has been reached/passed (same
 *       half-range expired test as gj_tick_expired_u).
 *   uint64_t __gj_tick_remaining_u  (alias)
 *   __libcgj_batch4605_marker = "libcgj-batch4605"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610). Unique
 * gj_tick_remaining_u surface only; no multi-def. Complements
 * gj_tick_expired_u (batch4604). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4605_marker[] = "libcgj-batch4605";

/* Half of the u64 modular domain (matches batch4604 expired test). */
#define B4605_HALF  (1ull << 63)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4605_remaining — ticks left until deadline, or 0 if expired.
 *
 * Expired when (now - deadline) < 2^63 (half-range modular test).
 * Otherwise remaining is the modular (deadline - now).
 */
static uint64_t
b4605_remaining(uint64_t u64Deadline, uint64_t u64Now)
{
	if ((u64Now - u64Deadline) < B4605_HALF) {
		return 0ull;
	}
	return u64Deadline - u64Now;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tick_remaining_u - ticks remaining until deadline (0 if expired).
 *
 * deadline: absolute free-running tick deadline
 * now:      current free-running tick sample
 *
 * Returns 0 when expired, else (deadline - now) modulo 2^64.
 * Self-contained; no parent wires.
 */
uint64_t
gj_tick_remaining_u(uint64_t u64Deadline, uint64_t u64Now)
{
	(void)NULL;
	return b4605_remaining(u64Deadline, u64Now);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_tick_remaining_u(uint64_t u64Deadline, uint64_t u64Now)
    __attribute__((alias("gj_tick_remaining_u")));
