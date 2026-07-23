/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1844: Shift modifier bit predicate.
 *
 * Surface (unique symbols):
 *   int gj_mod_mask_shift_p(uint32_t mods);
 *     — Return 1 if the Shift bit is set in mods, else 0.
 *       Convention: bit 0 (value 0x1) is Shift in this pure-data mask.
 *   int __gj_mod_mask_shift_p  (alias)
 *   __libcgj_batch1844_marker = "libcgj-batch1844"
 *
 * Keyboard/input exclusive pure-data wave (1841–1850). Shared mask
 * layout with batches 1845–1847 (Ctrl/Alt/Meta bits 1/2/3).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1844_marker[] = "libcgj-batch1844";

/* Pure-data modifier mask: Shift = bit 0. */
#define B1844_MOD_SHIFT 0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* True when Shift bit is set in uMods. */
static int
b1844_shift_p(uint32_t uMods)
{
	return ((uMods & B1844_MOD_SHIFT) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mod_mask_shift_p — 1 if mods has the Shift bit set.
 *
 * uMods: packed modifier bitfield (bit 0 = Shift)
 * Returns 1 when (uMods & 0x1) != 0, else 0.
 */
int
gj_mod_mask_shift_p(uint32_t uMods)
{
	return b1844_shift_p(uMods);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mod_mask_shift_p(uint32_t uMods)
    __attribute__((alias("gj_mod_mask_shift_p")));
