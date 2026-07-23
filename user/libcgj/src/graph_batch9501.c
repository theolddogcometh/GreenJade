/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9501: continuum product deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_9501(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_9501  (alias)
 *   __libcgj_batch9501_marker = "libcgj-batch9501"
 *
 * Exclusive continuum CREATE-ONLY (9501-9510: post-9500 continuum
 * product deepen). Unique gj_continuum_product_deepen_root_9501
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_9401 (batch9401),
 * gj_continuum_product_deepen_root_9301 (batch9301),
 * gj_continuum_product_deepen_root_9201, and
 * gj_continuum_product_deepen_root_9101. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9501_marker[] = "libcgj-batch9501";

/* Continuum product deepen root fourCC "CPDN". */
#define B9501_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9501_root(void)
{
	return B9501_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_9501 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 9501-9510 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_9501(void)
{
	(void)NULL;
	return b9501_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_9501(void)
    __attribute__((alias("gj_continuum_product_deepen_root_9501")));
