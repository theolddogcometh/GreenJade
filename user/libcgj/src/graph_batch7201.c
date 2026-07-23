/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7201: continuum product deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_7201(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_7201  (alias)
 *   __libcgj_batch7201_marker = "libcgj-batch7201"
 *
 * Exclusive continuum CREATE-ONLY (7201-7210: post-7200 continuum
 * product deepen). Unique gj_continuum_product_deepen_root_7201
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_7001,
 * gj_continuum_product_deepen_root_6801,
 * gj_continuum_product_deepen_root_6601, and
 * gj_bar3_product_deepen_root_7101. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7201_marker[] = "libcgj-batch7201";

/* Continuum product deepen root fourCC "CPDN". */
#define B7201_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7201_root(void)
{
	return B7201_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_7201 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 7201-7210 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_7201(void)
{
	(void)NULL;
	return b7201_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_7201(void)
    __attribute__((alias("gj_continuum_product_deepen_root_7201")));
