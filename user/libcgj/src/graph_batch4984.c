/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4984: x86_64 PTE No-Execute (NX) high-bit
 * mask (uint64, _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_pte_nx_bit_u(void);
 *     - Returns the soft NX / Execute-Disable high-bit mask concept as
 *       (1ull << 63). Soft freestanding constant; not a hardware probe.
 *   uint64_t __gj_pte_nx_bit_u  (alias)
 *   __libcgj_batch4984_marker = "libcgj-batch4984"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990). Unique gj_pte_nx_bit_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4984_marker[] = "libcgj-batch4984";

/* Soft NX / XD high-bit concept (bit 63 of a 64-bit PTE word). */
#define B4984_PTE_NX  (1ull << 63)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4984_nx_mask(void)
{
	return B4984_PTE_NX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_nx_bit_u - return the soft PTE NX high-bit mask (bit 63).
 *
 * Always returns (1ull << 63). Soft mask concept for freestanding PTE
 * construction / stripping; not a runtime hardware feature detect.
 * Self-contained; no parent wires.
 */
uint64_t
gj_pte_nx_bit_u(void)
{
	(void)NULL;
	return b4984_nx_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_pte_nx_bit_u(void)
    __attribute__((alias("gj_pte_nx_bit_u")));
