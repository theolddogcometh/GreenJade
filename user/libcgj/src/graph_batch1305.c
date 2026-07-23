/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1305: Ring index mask: next power-of-two minus one.
 *
 * Surface (unique symbols):
 *   size_t gj_ring_mask_for_cap(size_t cap);
 *     — Round cap up to the next power of two (identity if already
 *       pot2 and > 0), then return pot2 - 1 (bitmask for indexing).
 *       cap == 0 → 0. Overflow (no pot2 fits in size_t) → 0.
 *   size_t __gj_ring_mask_for_cap  (alias)
 *   __libcgj_batch1305_marker = "libcgj-batch1305"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>

const char __libcgj_batch1305_marker[] = "libcgj-batch1305";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_mask_for_cap — bitmask for a power-of-two ring covering cap slots.
 *
 * If cap is already a positive power of two, returns cap - 1.
 * Otherwise rounds up to the next power of two and returns that - 1.
 * cap == 0, or no representable next pot2, returns 0.
 */
size_t
gj_ring_mask_for_cap(size_t uCap)
{
	size_t uV;

	if (uCap == 0u) {
		return 0u;
	}

	/* Already pot2 → mask is cap - 1. */
	if ((uCap & (uCap - 1u)) == 0u) {
		return uCap - 1u;
	}

	/* Round up to next power of two (bit-smear). */
	uV = uCap - 1u;
	uV |= uV >> 1;
	uV |= uV >> 2;
	uV |= uV >> 4;
	uV |= uV >> 8;
	uV |= uV >> 16;
	/* size_t may be 64-bit; smear high half when present. */
	if (sizeof(size_t) > 4u) {
		uV |= uV >> 32;
	}
	/* uV + 1 is next pot2; overflow → all bits set after smear of huge n. */
	if (uV == (size_t)-1) {
		return 0u;
	}
	return uV; /* next_pot2 - 1 */
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_mask_for_cap(size_t uCap)
    __attribute__((alias("gj_ring_mask_for_cap")));

