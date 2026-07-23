/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4989: x86_64 soft PTE constructor (uint64, _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_pte_make_u(uint64_t addr, uint64_t flags);
 *     - Build a soft PTE word from a page-frame address and low flags:
 *         (addr & ~0xfff & ~(1ull << 63)) | (flags & 0xfff)
 *   uint64_t __gj_pte_make_u  (alias)
 *   __libcgj_batch4989_marker = "libcgj-batch4989"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990). Unique gj_pte_make_u
 * surface only; no multi-def. Soft freestanding compose — pairs with
 * gj_pte_addr_u / gj_pte_flags_u. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4989_marker[] = "libcgj-batch4989";

/* Soft low 12 flag bits and soft NX high bit. */
#define B4989_FLAG_LO  0xfffull
#define B4989_NX       (1ull << 63)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4989_make(uint64_t u64Addr, uint64_t u64Flags)
{
	uint64_t u64A;
	uint64_t u64F;

	u64A = u64Addr & ~B4989_FLAG_LO & ~B4989_NX;
	u64F = u64Flags & B4989_FLAG_LO;
	return u64A | u64F;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_make_u - soft-compose a PTE word from address and low flags.
 *
 * addr:  page-frame / physical address candidate (low 12 and NX stripped)
 * flags: soft low flag bits (masked to 0xfff)
 *
 * Returns (addr & ~0xfff & ~(1ull<<63)) | (flags & 0xfff).
 * Self-contained; no parent wires.
 */
uint64_t
gj_pte_make_u(uint64_t u64Addr, uint64_t u64Flags)
{
	(void)NULL;
	return b4989_make(u64Addr, u64Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_pte_make_u(uint64_t u64Addr, uint64_t u64Flags)
    __attribute__((alias("gj_pte_make_u")));
