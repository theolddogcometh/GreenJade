/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4604: tick deadline expired predicate.
 *
 * Surface (unique symbols):
 *   int gj_tick_expired_u(uint64_t deadline, uint64_t now);
 *     - Return 1 if the free-running tick counter has reached or passed
 *       deadline. Uses half-range modular test:
 *         (now - deadline) < (1ull << 63)
 *       so wrap-around within half the u64 space is treated as expired.
 *   int __gj_tick_expired_u  (alias)
 *   __libcgj_batch4604_marker = "libcgj-batch4604"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610). Distinct from
 * gj_deadline_passed_u (batch3137) / gj_deadline_passed (batch1037) —
 * unique gj_tick_expired_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4604_marker[] = "libcgj-batch4604";

/* Half of the u64 modular domain (MSB clear ⇒ "not behind by half+"). */
#define B4604_HALF  (1ull << 63)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4604_expired — modular half-range "time has reached deadline".
 *
 * (now - deadline) < 2^63 means now is at or forward of deadline within
 * the safe half-range window used by free-running tick counters.
 */
static int
b4604_expired(uint64_t u64Deadline, uint64_t u64Now)
{
	return ((u64Now - u64Deadline) < B4604_HALF) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tick_expired_u - 1 if now has reached/passed deadline (wrap-safe).
 *
 * deadline: absolute free-running tick deadline
 * now:      current free-running tick sample
 *
 * Returns 1 when (now - deadline) < (1ull << 63), else 0.
 * Self-contained; no parent wires.
 */
int
gj_tick_expired_u(uint64_t u64Deadline, uint64_t u64Now)
{
	(void)NULL;
	return b4604_expired(u64Deadline, u64Now);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_tick_expired_u(uint64_t u64Deadline, uint64_t u64Now)
    __attribute__((alias("gj_tick_expired_u")));
