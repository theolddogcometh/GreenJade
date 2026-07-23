/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6906: bar3 product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_code_6906(void);
 *     - Returns the soft bar3 product-deepen code tag for this
 *       continuum (always 0x42503931, fourCC "BP91"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_bar3_product_deepen_code_6906  (alias)
 *   __libcgj_batch6906_marker = "libcgj-batch6906"
 *
 * Exclusive continuum CREATE-ONLY (6901-6910: post-6900 bar3 product
 * deepen). Unique gj_bar3_product_deepen_code_6906 surface only; no
 * multi-def. Distinct from gj_bar3_product_deepen_code_6706,
 * gj_bar3_install_checklist_deepen_code_6506, and
 * gj_continuum_product_deepen_code_6606. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6906_marker[] = "libcgj-batch6906";

/* Bar3 product deepen code fourCC "BP91". */
#define B6906_BPD_CODE  0x42503931u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6906_code(void)
{
	return B6906_BPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_code_6906 - bar3 product deepen code.
 *
 * Always returns 0x42503931 ("BP91"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_bar3_product_deepen_code_6906(void)
{
	(void)NULL;
	return b6906_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_product_deepen_code_6906(void)
    __attribute__((alias("gj_bar3_product_deepen_code_6906")));
