/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1847: Meta/Super modifier bit predicate.
 *
 * Surface (unique symbols):
 *   int gj_mod_mask_meta_p(uint32_t mods);
 *     — Return 1 if the Meta (Super/Win/Cmd) bit is set in mods, else 0.
 *       Convention: bit 3 (value 0x8) is Meta in this pure-data mask.
 *   int __gj_mod_mask_meta_p  (alias)
 *   __libcgj_batch1847_marker = "libcgj-batch1847"
 *
 * Keyboard/input exclusive pure-data wave (1841–1850). Shared mask
 * layout with batches 1844–1846 (Shift/Ctrl/Alt bits 0/1/2).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1847_marker[] = "libcgj-batch1847";

/* Pure-data modifier mask: Meta = bit 3. */
#define B1847_MOD_META 0x8u

/* ---- freestanding helpers ---------------------------------------------- */

/* True when Meta bit is set in uMods. */
static int
b1847_meta_p(uint32_t uMods)
{
	return ((uMods & B1847_MOD_META) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mod_mask_meta_p — 1 if mods has the Meta bit set.
 *
 * uMods: packed modifier bitfield (bit 3 = Meta/Super)
 * Returns 1 when (uMods & 0x8) != 0, else 0.
 */
int
gj_mod_mask_meta_p(uint32_t uMods)
{
	return b1847_meta_p(uMods);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mod_mask_meta_p(uint32_t uMods)
    __attribute__((alias("gj_mod_mask_meta_p")));
