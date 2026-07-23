/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4538: address/alignment validity predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_align_ok_u(uintptr_t addr, size_t align);
 *     - Return 1 if align is a non-zero power of two and addr is a
 *       multiple of align; else 0. align == 0 or non-pow2 → 0.
 *   int __gj_align_ok_u  (alias)
 *   __libcgj_batch4538_marker = "libcgj-batch4538"
 *
 * Exclusive continuum CREATE-ONLY (4531-4540: validation unique). Unique
 * gj_align_ok_u surface only; no multi-def. Distinct from
 * gj_ptr_is_aligned (batch977, void * form). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4538_marker[] = "libcgj-batch4538";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if align is a non-zero power of two and addr is aligned to it.
 */
static int
b4538_align_ok(uintptr_t uAddr, size_t uAlign)
{
	if (uAlign == 0u) {
		return 0;
	}
	if ((uAlign & (uAlign - 1u)) != 0u) {
		return 0;
	}
	if ((uAddr & (uintptr_t)(uAlign - 1u)) != 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_align_ok_u - 1 if align is pow2 and addr meets that alignment.
 *
 * addr:  address under test
 * align: required alignment (must be non-zero power of two)
 *
 * align 0 / non-pow2 → 0. align 1 → always 1 (byte alignment).
 * No parent wires.
 */
int
gj_align_ok_u(uintptr_t uAddr, size_t uAlign)
{
	(void)NULL;
	return b4538_align_ok(uAddr, uAlign);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_align_ok_u(uintptr_t uAddr, size_t uAlign)
    __attribute__((alias("gj_align_ok_u")));
