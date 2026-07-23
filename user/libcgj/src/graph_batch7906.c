/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7906: bar3 product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_code_7906(void);
 *     - Returns the soft bar3 product-deepen code tag for this
 *       continuum (always 0x42503739, fourCC "BP79"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_bar3_product_deepen_code_7906  (alias)
 *   __libcgj_batch7906_marker = "libcgj-batch7906"
 *
 * Exclusive continuum CREATE-ONLY (7901-7910: post-7900 bar3 product
 * deepen). Unique gj_bar3_product_deepen_code_7906 surface only; no
 * multi-def. Distinct from gj_bar3_product_deepen_code_7706 ("BP77"),
 * gj_bar3_product_deepen_code_7606, gj_continuum_product_deepen_code_7806,
 * and gj_bar3_product_deepen_ok_7905. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7906_marker[] = "libcgj-batch7906";

/* Bar3 product deepen code fourCC "BP79". */
#define B7906_BPD_CODE  0x42503739u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7906_code(void)
{
	return B7906_BPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_code_7906 - bar3 product deepen code.
 *
 * Always returns 0x42503739 ("BP79"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_bar3_product_deepen_code_7906(void)
{
	(void)NULL;
	return b7906_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_product_deepen_code_7906(void)
    __attribute__((alias("gj_bar3_product_deepen_code_7906")));
