/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6106: product install deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_install_deepen_code_6106(void);
 *     - Returns the soft product install-deepen code tag for this
 *       continuum (always 0x49443331, fourCC "ID31"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_product_install_deepen_code_6106  (alias)
 *   __libcgj_batch6106_marker = "libcgj-batch6106"
 *
 * Exclusive continuum CREATE-ONLY (6101-6110: post-6100 product
 * deepen install). Unique gj_product_install_deepen_code_6106 surface
 * only; no multi-def. Distinct from gj_install_media_finalize_5806
 * and gj_product_install_deepen_ok_6105. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6106_marker[] = "libcgj-batch6106";

/* Product install deepen code fourCC "ID31". */
#define B6106_DEEPEN_CODE  0x49443331u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6106_code(void)
{
	return B6106_DEEPEN_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_install_deepen_code_6106 - install-deepen code tag.
 *
 * Always returns 0x49443331 ("ID31"). Soft pure-data product tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_product_install_deepen_code_6106(void)
{
	(void)NULL;
	return b6106_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_install_deepen_code_6106(void)
    __attribute__((alias("gj_product_install_deepen_code_6106")));
