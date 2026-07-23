/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1845: Ctrl modifier bit predicate.
 *
 * Surface (unique symbols):
 *   int gj_mod_mask_ctrl_p(uint32_t mods);
 *     — Return 1 if the Ctrl bit is set in mods, else 0.
 *       Convention: bit 1 (value 0x2) is Ctrl in this pure-data mask.
 *   int __gj_mod_mask_ctrl_p  (alias)
 *   __libcgj_batch1845_marker = "libcgj-batch1845"
 *
 * Keyboard/input exclusive pure-data wave (1841–1850). Shared mask
 * layout with batches 1844/1846/1847 (Shift/Alt/Meta bits 0/2/3).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1845_marker[] = "libcgj-batch1845";

/* Pure-data modifier mask: Ctrl = bit 1. */
#define B1845_MOD_CTRL 0x2u

/* ---- freestanding helpers ---------------------------------------------- */

/* True when Ctrl bit is set in uMods. */
static int
b1845_ctrl_p(uint32_t uMods)
{
	return ((uMods & B1845_MOD_CTRL) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mod_mask_ctrl_p — 1 if mods has the Ctrl bit set.
 *
 * uMods: packed modifier bitfield (bit 1 = Ctrl)
 * Returns 1 when (uMods & 0x2) != 0, else 0.
 */
int
gj_mod_mask_ctrl_p(uint32_t uMods)
{
	return b1845_ctrl_p(uMods);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mod_mask_ctrl_p(uint32_t uMods)
    __attribute__((alias("gj_mod_mask_ctrl_p")));
