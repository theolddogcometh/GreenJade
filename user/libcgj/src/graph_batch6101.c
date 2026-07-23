/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6101: product install deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_install_deepen_root_6101(void);
 *     - Returns the soft product install-deepen root layout tag for
 *       this continuum (always 0x5049444E, fourCC "PIDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_product_install_deepen_root_6101  (alias)
 *   __libcgj_batch6101_marker = "libcgj-batch6101"
 *
 * Exclusive continuum CREATE-ONLY (6101-6110: post-6100 product
 * deepen install). Unique gj_product_install_deepen_root_6101 surface
 * only; no multi-def. Distinct from gj_product_bar3_media_root_5801
 * and gj_install_media_ok (batch2638). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6101_marker[] = "libcgj-batch6101";

/* Product install deepen root fourCC "PIDN". */
#define B6101_INSTALL_DEEPEN_ROOT  0x5049444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6101_root(void)
{
	return B6101_INSTALL_DEEPEN_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_install_deepen_root_6101 - product install deepen root tag.
 *
 * Always returns 0x5049444E ("PIDN"). Soft pure-data layout tag for the
 * 6101-6110 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_product_install_deepen_root_6101(void)
{
	(void)NULL;
	return b6101_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_install_deepen_root_6101(void)
    __attribute__((alias("gj_product_install_deepen_root_6101")));
