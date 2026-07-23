/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8101: bar3 product deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_root_8101(void);
 *     - Returns the soft bar3 product-deepen root layout tag for this
 *       continuum (always 0x4250444E, fourCC "BPDN"). Not a filesystem
 *       path probe.
 *   uint32_t __gj_bar3_product_deepen_root_8101  (alias)
 *   __libcgj_batch8101_marker = "libcgj-batch8101"
 *
 * Exclusive continuum CREATE-ONLY (8101-8110: post-8100 bar3 product
 * deepen). Unique gj_bar3_product_deepen_root_8101 surface only; no
 * multi-def. Distinct from gj_bar3_product_deepen_root_7901,
 * gj_bar3_product_deepen_root_7701,
 * gj_continuum_product_deepen_root_8001, and
 * gj_product_install_deepen_root_6101. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8101_marker[] = "libcgj-batch8101";

/* Bar3 product deepen root fourCC "BPDN". */
#define B8101_BPD_ROOT  0x4250444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8101_root(void)
{
	return B8101_BPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_root_8101 - bar3 product deepen root.
 *
 * Always returns 0x4250444E ("BPDN"). Soft pure-data layout tag for the
 * 8101-8110 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_bar3_product_deepen_root_8101(void)
{
	(void)NULL;
	return b8101_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_product_deepen_root_8101(void)
    __attribute__((alias("gj_bar3_product_deepen_root_8101")));
