/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7106: bar3 product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_code_7106(void);
 *     - Returns the soft bar3 product-deepen code tag for this
 *       continuum (always 0x42503131, fourCC "BP11"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_bar3_product_deepen_code_7106  (alias)
 *   __libcgj_batch7106_marker = "libcgj-batch7106"
 *
 * Exclusive continuum CREATE-ONLY (7101-7110: post-7100 bar3 product
 * deepen). Unique gj_bar3_product_deepen_code_7106 surface only; no
 * multi-def. Distinct from gj_bar3_product_deepen_code_6906 ("BP91"),
 * gj_bar3_product_deepen_code_6706 ("BP71"), and
 * gj_product_install_deepen_code_6106. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7106_marker[] = "libcgj-batch7106";

/* Bar3 product deepen code fourCC "BP11". */
#define B7106_BPD_CODE  0x42503131u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7106_code(void)
{
	return B7106_BPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_code_7106 - bar3 product deepen code.
 *
 * Always returns 0x42503131 ("BP11"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_bar3_product_deepen_code_7106(void)
{
	(void)NULL;
	return b7106_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_product_deepen_code_7106(void)
    __attribute__((alias("gj_bar3_product_deepen_code_7106")));
