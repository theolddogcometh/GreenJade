/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6706: bar3 product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_code_6706(void);
 *     - Returns the soft bar3 product-deepen code tag for this
 *       continuum (always 0x42503731, fourCC "BP71"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_bar3_product_deepen_code_6706  (alias)
 *   __libcgj_batch6706_marker = "libcgj-batch6706"
 *
 * Exclusive continuum CREATE-ONLY (6701-6710: post-6700 bar3 product
 * deepen). Unique gj_bar3_product_deepen_code_6706 surface only; no
 * multi-def. Distinct from gj_bar3_install_checklist_deepen_code_6506,
 * gj_continuum_product_deepen_code_6606, and
 * gj_bar3_product_deepen_ok_6705. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6706_marker[] = "libcgj-batch6706";

/* Bar3 product deepen code fourCC "BP71". */
#define B6706_BPD_CODE  0x42503731u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6706_code(void)
{
	return B6706_BPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_code_6706 - bar3 product deepen code.
 *
 * Always returns 0x42503731 ("BP71"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_bar3_product_deepen_code_6706(void)
{
	(void)NULL;
	return b6706_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_product_deepen_code_6706(void)
    __attribute__((alias("gj_bar3_product_deepen_code_6706")));
