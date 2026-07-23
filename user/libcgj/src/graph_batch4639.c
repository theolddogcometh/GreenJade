/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4639: round bytes up to a 4 KiB page (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_round_page_u(uint64_t b);
 *     - Round b up to the next multiple of 4096.
 *       b already aligned → b.
 *       Overflow of (b + 4095) → 0.
 *   uint64_t __gj_bytes_round_page_u  (alias)
 *   __libcgj_batch4639_marker = "libcgj-batch4639"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, si_kb_to_bytes_u, si_mb_to_bytes_u, bytes_round_page_u,
 * batch_id_4640). Distinct from gj_align_page_4k_u (batch3142),
 * gj_u64_page_ceil_u (batch4265), and gj_page_round_up (batch975) —
 * unique gj_bytes_round_page_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4639_marker[] = "libcgj-batch4639";

/* Standard x86_64 base page size (4 KiB). */
#define B4639_PAGE_4K    4096ull
#define B4639_PAGE_MASK  (B4639_PAGE_4K - 1ull)

/* ---- freestanding helpers ---------------------------------------------- */

/* (b + 4095) & ~4095; overflow of intermediate → 0. */
static uint64_t
b4639_round_page(uint64_t u64B)
{
	if (u64B > (UINT64_MAX - B4639_PAGE_MASK)) {
		return 0ull;
	}
	return (u64B + B4639_PAGE_MASK) & ~B4639_PAGE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_round_page_u - round bytes up to a 4096-byte page boundary.
 *
 * b: byte count (or address-as-size) to round up.
 *
 * Already 4 KiB-aligned values return unchanged. Intermediate overflow
 * yields 0. No parent wires.
 */
uint64_t
gj_bytes_round_page_u(uint64_t u64B)
{
	(void)NULL;
	return b4639_round_page(u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_round_page_u(uint64_t u64B)
    __attribute__((alias("gj_bytes_round_page_u")));
