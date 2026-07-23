/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8106: bar3 product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_code_8106(void);
 *     - Returns the soft bar3 product-deepen code tag for this
 *       continuum (always 0x42503831, fourCC "BP81"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_bar3_product_deepen_code_8106  (alias)
 *   __libcgj_batch8106_marker = "libcgj-batch8106"
 *
 * Exclusive continuum CREATE-ONLY (8101-8110: post-8100 bar3 product
 * deepen). Unique gj_bar3_product_deepen_code_8106 surface only; no
 * multi-def. Distinct from gj_bar3_product_deepen_code_7906 ("BP79"),
 * gj_bar3_product_deepen_code_7706, gj_continuum_product_deepen_code_8006,
 * and gj_bar3_product_deepen_ok_8105. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8106_marker[] = "libcgj-batch8106";

/* Bar3 product deepen code fourCC "BP81". */
#define B8106_BPD_CODE  0x42503831u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8106_code(void)
{
	return B8106_BPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_code_8106 - bar3 product deepen code.
 *
 * Always returns 0x42503831 ("BP81"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_bar3_product_deepen_code_8106(void)
{
	(void)NULL;
	return b8106_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_product_deepen_code_8106(void)
    __attribute__((alias("gj_bar3_product_deepen_code_8106")));
