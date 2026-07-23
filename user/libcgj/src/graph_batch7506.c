/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7506: bar3 product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_code_7506(void);
 *     - Returns the soft bar3 product-deepen code tag for this
 *       continuum (always 0x42503531, fourCC "BP51"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_bar3_product_deepen_code_7506  (alias)
 *   __libcgj_batch7506_marker = "libcgj-batch7506"
 *
 * Exclusive continuum CREATE-ONLY (7501-7510: post-7500 bar3 product
 * deepen). Unique gj_bar3_product_deepen_code_7506 surface only; no
 * multi-def. Distinct from gj_bar3_product_deepen_code_7306 ("BP31"),
 * gj_bar3_product_deepen_code_7106 ("BP11"), and
 * gj_bar3_product_deepen_code_6906 ("BP91"). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7506_marker[] = "libcgj-batch7506";

/* Bar3 product deepen code fourCC "BP51". */
#define B7506_BPD_CODE  0x42503531u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7506_code(void)
{
	return B7506_BPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_code_7506 - bar3 product deepen code.
 *
 * Always returns 0x42503531 ("BP51"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_bar3_product_deepen_code_7506(void)
{
	(void)NULL;
	return b7506_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_product_deepen_code_7506(void)
    __attribute__((alias("gj_bar3_product_deepen_code_7506")));
