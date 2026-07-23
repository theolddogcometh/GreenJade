/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch715: reentrant xorshift64 PRNG step.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party PRNG source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   rand / srand / random / srandom          → stdlib / rand_div.c
 *   drand48 / lrand48 / srand48 / seed48     → rand48.c
 *   arc4random* / __arc4random*              → graph_batch106.c
 *   gj_xorshift128_seed / next / fill        → graph_batch173.c
 *   gj_pcg32_seed / gj_pcg32_next            → graph_batch303.c
 *   cookie xorshift64* (kernel only)         → kernel/mm/fault.c
 *
 * This TU adds only unique symbols:
 *   uint64_t gj_xorshift64_next(uint64_t *state);
 *     — Advance caller-owned *state (Marsaglia xorshift64: 13/7/17) and
 *       return the new state word. NULL state or *state == 0 → return 0
 *       (zero is a fixed point; no write on NULL).
 *   uint64_t __gj_xorshift64_next(uint64_t *state);  (alias)
 *   __libcgj_batch715_marker = "libcgj-batch715"
 *
 * Algorithm (xorshift64):
 *   x ^= x << 13; x ^= x >> 7; x ^= x << 17;
 *
 * Notes:
 *   - Not cryptographically secure; non-crypto randomness only.
 *   - Caller seeds and owns state; no process-global storage, no locking.
 *   - Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 *     No malloc, no errno, no libc.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch715_marker[] = "libcgj-batch715";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xorshift64_next — one xorshift64 step; updates *state in place.
 *
 * state: caller-owned 64-bit word. Must be non-zero for a non-trivial
 *        stream (all-zero is a fixed point of the map).
 * Returns the new *state value, or 0 if state is NULL or *state is 0.
 */
uint64_t
gj_xorshift64_next(uint64_t *state)
{
	uint64_t x;

	if (state == NULL) {
		return 0ULL;
	}

	x = *state;
	if (x == 0ULL) {
		return 0ULL;
	}

	x ^= x << 13; /* a */
	x ^= x >> 7;  /* b */
	x ^= x << 17; /* c */
	*state = x;
	return x;
}

uint64_t __gj_xorshift64_next(uint64_t *state)
    __attribute__((alias("gj_xorshift64_next")));
