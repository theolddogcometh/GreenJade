/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1846: Alt modifier bit predicate.
 *
 * Surface (unique symbols):
 *   int gj_mod_mask_alt_p(uint32_t mods);
 *     — Return 1 if the Alt bit is set in mods, else 0.
 *       Convention: bit 2 (value 0x4) is Alt in this pure-data mask.
 *   int __gj_mod_mask_alt_p  (alias)
 *   __libcgj_batch1846_marker = "libcgj-batch1846"
 *
 * Keyboard/input exclusive pure-data wave (1841–1850). Shared mask
 * layout with batches 1844/1845/1847 (Shift/Ctrl/Meta bits 0/1/3).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1846_marker[] = "libcgj-batch1846";

/* Pure-data modifier mask: Alt = bit 2. */
#define B1846_MOD_ALT 0x4u

/* ---- freestanding helpers ---------------------------------------------- */

/* True when Alt bit is set in uMods. */
static int
b1846_alt_p(uint32_t uMods)
{
	return ((uMods & B1846_MOD_ALT) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mod_mask_alt_p — 1 if mods has the Alt bit set.
 *
 * uMods: packed modifier bitfield (bit 2 = Alt)
 * Returns 1 when (uMods & 0x4) != 0, else 0.
 */
int
gj_mod_mask_alt_p(uint32_t uMods)
{
	return b1846_alt_p(uMods);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mod_mask_alt_p(uint32_t uMods)
    __attribute__((alias("gj_mod_mask_alt_p")));
