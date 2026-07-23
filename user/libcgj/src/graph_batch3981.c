/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3981: buddy-class size (size_t, next power of two).
 *
 * Surface (unique symbols):
 *   size_t gj_buddy_size_u(size_t n);
 *     - Smallest power of two >= n (buddy allocator class size).
 *       gj_buddy_size_u(0) == 1.
 *       If n is already a power of two, return n unchanged.
 *       If the next power of two is not representable in size_t, return 0.
 *   size_t __gj_buddy_size_u  (alias)
 *   __libcgj_batch3981_marker = "libcgj-batch3981"
 *
 * Exclusive continuum CREATE-ONLY (3981-3990: buddy_size_u, page_align_u,
 * cacheline_align_u, is_page_aligned_u, is_cacheline_aligned_u, kb_align_u,
 * mb_align_u, size_add_sat_u, size_sub_sat_u, batch_id_3990). Distinct from
 * gj_next_pow2_u64 (batch374) / gj_u32_next_pow2 (batch778) / gj_next_power2
 * (batch524) — size_t buddy surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3981_marker[] = "libcgj-batch3981";

/* Highest power of two representable in size_t. */
#define B3981_POW2_MAX  ((SIZE_MAX >> 1) + 1u)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3981_buddy — ceil to power of two; 0 on overflow past B3981_POW2_MAX.
 *
 * Smear-bits roundup after (n-1) so already-pow2 values round to self.
 */
static size_t
b3981_buddy(size_t uN)
{
	size_t uV;

	if (uN == 0u) {
		return 1u;
	}
	if (uN > B3981_POW2_MAX) {
		return 0u;
	}
	if ((uN & (uN - 1u)) == 0u) {
		return uN;
	}

	uV = uN - 1u;
	uV |= uV >> 1;
	uV |= uV >> 2;
	uV |= uV >> 4;
	uV |= uV >> 8;
	uV |= uV >> 16;
#if SIZE_MAX > 0xFFFFFFFFu
	uV |= uV >> 32;
#endif
	uV += 1u;
	return uV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buddy_size_u - buddy-class block size for a request of n bytes.
 *
 * Returns the smallest power of two >= n. Overflow of the round-up past
 * the top representable power of two in size_t yields 0. No parent wires.
 */
size_t
gj_buddy_size_u(size_t uN)
{
	(void)NULL;
	return b3981_buddy(uN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_buddy_size_u(size_t uN)
    __attribute__((alias("gj_buddy_size_u")));
