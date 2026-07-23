/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1578: clear bits under a 32-bit flag mask.
 *
 * Surface (unique symbols):
 *   void gj_flags32_clear_mask(uint32_t *flags, uint32_t mask);
 *     — *flags &= ~mask. NULL flags → no-op. Pure flag arithmetic; no
 *       bit-matrix coupling.
 *   void __gj_flags32_clear_mask  (alias)
 *   __libcgj_batch1578_marker = "libcgj-batch1578"
 *
 * Distinct from gj_bitset_clear (batch118) and gj_flags32_set_mask
 * (batch1577). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>
#include <stddef.h>

const char __libcgj_batch1578_marker[] = "libcgj-batch1578";

/* ---- freestanding helpers ---------------------------------------------- */

/* Apply AND-not mask into *pFlags; pFlags non-NULL. */
static void
b1578_andnot_into(uint32_t *pFlags, uint32_t u32Mask)
{
	*pFlags &= ~u32Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags32_clear_mask — clear every bit present in u32Mask on *pFlags.
 *
 * pFlags NULL → no-op. u32Mask 0 leaves *pFlags unchanged.
 */
void
gj_flags32_clear_mask(uint32_t *pFlags, uint32_t u32Mask)
{
	if (pFlags == NULL) {
		return;
	}
	b1578_andnot_into(pFlags, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_flags32_clear_mask(uint32_t *pFlags, uint32_t u32Mask)
    __attribute__((alias("gj_flags32_clear_mask")));
