/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6301: continuum product deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_6301(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_6301  (alias)
 *   __libcgj_batch6301_marker = "libcgj-batch6301"
 *
 * Exclusive continuum CREATE-ONLY (6301-6310: post-6300 continuum
 * product deepen). Unique gj_continuum_product_deepen_root_6301
 * surface only; no multi-def. Distinct from
 * gj_product_install_deepen_root_6101 and gj_product_bar3_media_root_5801.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6301_marker[] = "libcgj-batch6301";

/* Continuum product deepen root fourCC "CPDN". */
#define B6301_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6301_root(void)
{
	return B6301_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_6301 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 6301-6310 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_6301(void)
{
	(void)NULL;
	return b6301_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_6301(void)
    __attribute__((alias("gj_continuum_product_deepen_root_6301")));
