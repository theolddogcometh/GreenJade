/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10001: continuum product deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_10001(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_10001  (alias)
 *   __libcgj_batch10001_marker = "libcgj-batch10001"
 *
 * Exclusive continuum CREATE-ONLY (10001-10010: post-10000 continuum
 * product deepen). Unique gj_continuum_product_deepen_root_10001
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_10000,
 * gj_continuum_product_deepen_root_8201,
 * gj_continuum_product_deepen_root_8001, and
 * gj_continuum_product_deepen_root_7201. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10001_marker[] = "libcgj-batch10001";

/* Continuum product deepen root fourCC "CPDN". */
#define B10001_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10001_root(void)
{
	return B10001_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_10001 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 10001-10010 post-10000 continuum. Does not probe paths or call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_10001(void)
{
	(void)NULL;
	return b10001_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_10001(void)
    __attribute__((alias("gj_continuum_product_deepen_root_10001")));
