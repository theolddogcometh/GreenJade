/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5088: debug address/alignment predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbg_align_ok_u(uintptr_t addr, size_t a);
 *     - Return 1 if a is a non-zero power of two and addr is a multiple
 *       of a; else 0. a == 0 or non-pow2 → 0.
 *   uint32_t __gj_dbg_align_ok_u  (alias)
 *   __libcgj_batch5088_marker = "libcgj-batch5088"
 *
 * Exclusive continuum CREATE-ONLY (5081-5090: debug/assert unique).
 * Unique gj_dbg_align_ok_u surface only; no multi-def. Distinct from
 * gj_align_ok_u (batch4538, int return). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5088_marker[] = "libcgj-batch5088";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if a is a non-zero power of two and addr is aligned to it.
 */
static uint32_t
b5088_align_ok(uintptr_t uAddr, size_t uA)
{
	if (uA == 0u) {
		return 0u;
	}
	if ((uA & (uA - 1u)) != 0u) {
		return 0u;
	}
	if ((uAddr & (uintptr_t)(uA - 1u)) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbg_align_ok_u - 1 if a is pow2 and addr meets that alignment.
 *
 * addr: address under test
 * a:    required alignment (must be non-zero power of two)
 *
 * a 0 / non-pow2 → 0. a 1 → always 1 (byte alignment). No parent wires.
 */
uint32_t
gj_dbg_align_ok_u(uintptr_t uAddr, size_t uA)
{
	(void)NULL;
	return b5088_align_ok(uAddr, uA);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbg_align_ok_u(uintptr_t uAddr, size_t uA)
    __attribute__((alias("gj_dbg_align_ok_u")));
