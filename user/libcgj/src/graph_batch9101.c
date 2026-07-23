/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9101: continuum product deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_9101(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_9101  (alias)
 *   __libcgj_batch9101_marker = "libcgj-batch9101"
 *
 * Exclusive continuum CREATE-ONLY (9101-9110: post-9100 continuum
 * product deepen). Unique gj_continuum_product_deepen_root_9101
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_9001 (batch9001),
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

const char __libcgj_batch9101_marker[] = "libcgj-batch9101";

/* Continuum product deepen root fourCC "CPDN". */
#define B9101_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9101_root(void)
{
	return B9101_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_9101 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 9101-9110 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_9101(void)
{
	(void)NULL;
	return b9101_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_9101(void)
    __attribute__((alias("gj_continuum_product_deepen_root_9101")));
