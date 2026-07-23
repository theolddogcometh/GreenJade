/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4479: next power-of-two capacity for n items.
 *
 * Surface (unique symbols):
 *   size_t gj_capacity_pow2_u(size_t n);
 *     - Smallest power of two >= n suitable as a ring/stack capacity.
 *       n == 0            → 0  (no slots required)
 *       n already 2^k     → n
 *       next would overflow size_t high bit → 0
 *   size_t __gj_capacity_pow2_u  (alias)
 *   __libcgj_batch4479_marker = "libcgj-batch4479"
 *
 * Exclusive continuum CREATE-ONLY (4471-4480: stack_push_idx_u,
 * stack_pop_idx_u, queue_en_idx_u, queue_de_idx_u, deque_push_front_u,
 * deque_push_back_u, idx_wrap_inc_u, idx_wrap_dec_u, capacity_pow2_u,
 * batch_id_4480). Distinct from gj_next_pow2_u64 (batch374) /
 * gj_next_power2 (batch524) — size_t capacity surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4479_marker[] = "libcgj-batch4479";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Next power-of-two capacity. Highest representable power of two in
 * size_t is 2^(bits-1); asking for more returns 0 (overflow).
 * Bit-smear is portable; the second 16-bit cascade covers 64-bit size_t
 * without a >>32 that would be undefined for 32-bit size_t.
 */
static size_t
b4479_pow2(size_t nN)
{
	size_t nV;
	size_t nHi;

	if (nN == 0u) {
		return 0u;
	}

	/* 2^(CHAR_BIT*sizeof(size_t) - 1); assume 8-bit bytes. */
	nHi = (size_t)1 << ((sizeof(size_t) * 8u) - 1u);
	if (nN > nHi) {
		return 0u;
	}

	nV = nN - 1u;
	nV |= nV >> 1;
	nV |= nV >> 2;
	nV |= nV >> 4;
	nV |= nV >> 8;
	nV |= nV >> 16;
	/* Smear bits 32..63 when size_t is 64-bit; no-op when 32-bit. */
	nV |= (nV >> 16) >> 16;
	return nV + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_capacity_pow2_u - next power-of-two capacity for n items.
 *
 * n: requested item count
 *
 * Returns ceil_pow2(n), 0 when n is 0, or 0 on overflow past size_t
 * high-bit power of two.
 * Self-contained; no parent wires.
 */
size_t
gj_capacity_pow2_u(size_t nN)
{
	(void)NULL;
	return b4479_pow2(nN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_capacity_pow2_u(size_t nN)
    __attribute__((alias("gj_capacity_pow2_u")));
