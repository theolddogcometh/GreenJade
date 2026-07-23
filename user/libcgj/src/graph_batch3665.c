/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3665: HyperLogLog register index (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_hll_register_u(uint64_t hash, size_t m);
 *     - Return the register index for a 64-bit hash under an HLL
 *       sketch with m power-of-two registers in [1, 256]. Index is
 *       the high p = log2(m) bits of hash (p==0 → 0). Invalid m
 *       (0, non-power-of-two, or > 256) → 0.
 *   size_t __gj_hll_register_u  (alias)
 *   __libcgj_batch3665_marker = "libcgj-batch3665"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670). Distinct from
 * gj_hll_add / gj_hll_init (batch494) — unique register-index _u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3665_marker[] = "libcgj-batch3665";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count leading zeros in a 64-bit word (0 → 64). No builtins. */
static unsigned
b3665_clz64(uint64_t x)
{
	unsigned n;
	uint64_t v;

	if (x == 0ull) {
		return 64u;
	}
	v = x;
	n = 0u;
	if ((v & 0xffffffff00000000ull) == 0ull) {
		n += 32u;
		v <<= 32;
	}
	if ((v & 0xffff000000000000ull) == 0ull) {
		n += 16u;
		v <<= 16;
	}
	if ((v & 0xff00000000000000ull) == 0ull) {
		n += 8u;
		v <<= 8;
	}
	if ((v & 0xf000000000000000ull) == 0ull) {
		n += 4u;
		v <<= 4;
	}
	if ((v & 0xc000000000000000ull) == 0ull) {
		n += 2u;
		v <<= 2;
	}
	if ((v & 0x8000000000000000ull) == 0ull) {
		n += 1u;
	}
	return n;
}

/* True iff m is a power of two in [1, 256]. */
static int
b3665_m_ok(size_t m)
{
	if (m == 0u || m > 256u) {
		return 0;
	}
	return (m & (m - 1u)) == 0u;
}

/* p = log2(m) for power-of-two m in [1, 256]. */
static unsigned
b3665_log2_m(size_t m)
{
	return 63u - b3665_clz64((uint64_t)m);
}

/* Register index from high p bits of hash. */
static size_t
b3665_idx(uint64_t u64Hash, size_t cM)
{
	unsigned p;

	p = b3665_log2_m(cM);
	if (p == 0u) {
		return 0u;
	}
	return (size_t)(u64Hash >> (64u - p));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hll_register_u - HLL register index for hash under m registers.
 *
 * hash: 64-bit observation hash
 * m:    power of two in [1, 256]; invalid → 0
 *
 * Returns index in [0, m). Does not call libc. No parent wires.
 */
size_t
gj_hll_register_u(uint64_t u64Hash, size_t cM)
{
	(void)NULL;
	if (!b3665_m_ok(cM)) {
		return 0u;
	}
	return b3665_idx(u64Hash, cM);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_hll_register_u(uint64_t u64Hash, size_t cM)
    __attribute__((alias("gj_hll_register_u")));
