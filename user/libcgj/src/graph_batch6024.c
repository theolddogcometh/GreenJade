/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6024: Top50 path install-stage bit extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_top50_path_install_bit_6024(uint32_t mask);
 *     - Extract bit0 of mask (install stage on the product path toward
 *       Deck Top 50). Returns 0 or 1. Soft pure-data only.
 *   uint32_t __gj_top50_path_install_bit_6024  (alias)
 *   __libcgj_batch6024_marker = "libcgj-batch6024"
 *
 * Exclusive continuum CREATE-ONLY (6021-6030: post-6000 top50 path
 * deepen). Unique gj_top50_path_install_bit_6024 surface only; no
 * multi-def. Path stages: bit0=install, bit1=steam, bit2=proton,
 * bit3=matrix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6024_marker[] = "libcgj-batch6024";

#define B6024_INSTALL_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6024_install_bit(uint32_t uMask)
{
	if ((uMask & B6024_INSTALL_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_path_install_bit_6024 - extract install stage path bit.
 *
 * mask: product Top50 path bitmask (bit0 = install stage ready).
 * Returns 1 if install bit set, else 0.
 * Soft pure-data only; does not call libc. No parent wires.
 */
uint32_t
gj_top50_path_install_bit_6024(uint32_t uMask)
{
	(void)NULL;
	return b6024_install_bit(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_top50_path_install_bit_6024(uint32_t uMask)
    __attribute__((alias("gj_top50_path_install_bit_6024")));
