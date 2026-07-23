/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2129: freestanding uint32 FIFO bulk discard.
 *
 * Surface (unique symbols):
 *   size_t gj_fifo_u32_discard_n(size_t cap, size_t *tail, size_t *count,
 *                                size_t n);
 *     - Drop up to n oldest elements by advancing *tail and decreasing
 *       *count. Does not read or write the buffer payload. Returns the
 *       number of elements discarded (0..n; clamped to occupancy).
 *   size_t __gj_fifo_u32_discard_n  (alias)
 *   __libcgj_batch2129_marker = "libcgj-batch2129"
 *
 * FIFO design (count-based head/tail):
 *   tail  - next read index (mod cap); advanced by elements discarded
 *   count - live occupancy; decremented by elements discarded
 *   head is not touched (dropped slots become free space for push)
 *
 * Post-2100 fifo_u32 exclusive wave (2121-2130). Distinct from
 * gj_ring_u8_discard_n - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2129_marker[] = "libcgj-batch2129";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by k steps with modular wrap (any cap > 0). */
static size_t
b2129_add_mod(size_t iIdx, size_t cK, size_t cCap)
{
	size_t iN;

	/* cK < cCap in our call sites (clamped to count <= cap). */
	iN = iIdx + cK;
	if (iN >= cCap) {
		iN -= cCap;
		if (iN >= cCap) {
			iN %= cCap;
		}
	}
	return iN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fifo_u32_discard_n - drop up to n oldest elements without reading.
 *
 * cap:   total slot count (needed for modular tail advance)
 * tail:  next-read index (advanced by elements discarded)
 * count: live occupancy (decremented by elements discarded)
 * n:     maximum elements to discard
 *
 * Returns elements actually discarded. Returns 0 if NULL required args,
 * cap == 0, corrupt cursors, or n == 0.
 */
size_t
gj_fifo_u32_discard_n(size_t cCap, size_t *pTail, size_t *pCount, size_t cN)
{
	size_t iT;
	size_t cLive;

	(void)NULL;

	if (pTail == NULL || pCount == NULL || cCap == 0u) {
		return 0u;
	}
	if (cN == 0u) {
		return 0u;
	}

	iT = *pTail;
	cLive = *pCount;
	if (iT >= cCap || cLive > cCap) {
		/* Corrupt cursor - refuse rather than advance out of bounds. */
		return 0u;
	}

	if (cN > cLive) {
		cN = cLive;
	}

	*pTail = b2129_add_mod(iT, cN, cCap);
	*pCount = cLive - cN;
	return cN;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_fifo_u32_discard_n(size_t cCap, size_t *pTail, size_t *pCount,
                               size_t cN)
    __attribute__((alias("gj_fifo_u32_discard_n")));
