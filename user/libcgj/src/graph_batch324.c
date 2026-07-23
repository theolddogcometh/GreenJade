/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch324: clock (second-chance) page replacement.
 *
 * Surface (unique symbols):
 *   int gj_clock_replace(uint8_t *refbits, size_t n, size_t *hand,
 *                        size_t *victim);
 *     — One step of the classic clock page-replacement simulation.
 *       refbits[0..n) are use/reference bits for n frames in a circular
 *       buffer. *hand is the clock hand index (normalized modulo n).
 *       Scan from *hand: if refbits[i] is non-zero (recently used), clear
 *       it to zero and advance; if refbits[i] is zero, select i as the
 *       victim, advance the hand past i, write *victim = i and update
 *       *hand. Returns 0 on success; returns -1 if any required pointer
 *       is NULL or n == 0. Does not set errno (freestanding).
 *   __gj_clock_replace  (alias)
 *   __libcgj_batch324_marker = "libcgj-batch324"
 *
 * Algorithm (second-chance / clock):
 *   - Circular scan of frames; each frame has a reference bit.
 *   - Referenced frames get a second chance: bit cleared, hand moves on.
 *   - First frame with a clear reference bit is the eviction victim.
 *   - Terminates in at most 2*n frame inspections when n > 0 (one full
 *     clear pass plus one selection when every bit was initially set).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch324_marker[] = "libcgj-batch324";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clock_replace — select a clock-algorithm victim and update state.
 *
 * refbits: mutable reference-bit array of length n (0 = cold, !=0 = used).
 * n:       frame count; must be non-zero.
 * hand:    in/out clock hand; on entry any size_t, used modulo n; on
 *          success points one past the chosen victim (mod n).
 * victim:  out index of the selected frame in [0, n).
 *
 * Returns 0 on success, -1 on invalid arguments.
 */
int
gj_clock_replace(uint8_t *refbits, size_t n, size_t *hand, size_t *victim)
{
	size_t iPos;
	size_t cSteps;
	size_t cMax;

	if (refbits == NULL || hand == NULL || victim == NULL) {
		return -1;
	}
	if (n == 0u) {
		return -1;
	}

	/* Normalize hand into [0, n). */
	iPos = *hand % n;

	/*
	 * Bound the scan: worst case clears every bit once then selects
	 * the next frame (2*n inspections). The loop body always advances
	 * iPos, so progress is guaranteed.
	 */
	cMax = n * 2u;
	for (cSteps = 0u; cSteps < cMax; cSteps++) {
		if (refbits[iPos] != 0u) {
			/* Second chance: clear use bit and advance. */
			refbits[iPos] = 0u;
			iPos++;
			if (iPos >= n) {
				iPos = 0u;
			}
		} else {
			/* Cold frame: take as victim, hand moves past. */
			*victim = iPos;
			iPos++;
			if (iPos >= n) {
				iPos = 0u;
			}
			*hand = iPos;
			return 0;
		}
	}

	/*
	 * Unreachable for n > 0 under the algorithm above; defensive
	 * fallback keeps the API total if the bound were ever wrong.
	 */
	*victim = iPos;
	iPos++;
	if (iPos >= n) {
		iPos = 0u;
	}
	*hand = iPos;
	return 0;
}

int __gj_clock_replace(uint8_t *refbits, size_t n, size_t *hand,
    size_t *victim) __attribute__((alias("gj_clock_replace")));
