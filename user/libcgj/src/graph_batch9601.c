/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9601: continuum product deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_9601(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_9601  (alias)
 *   __libcgj_batch9601_marker = "libcgj-batch9601"
 *
 * Exclusive continuum CREATE-ONLY (9601-9610: post-9600 continuum
 * product deepen). Unique gj_continuum_product_deepen_root_9601
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_8201,
 * gj_continuum_product_deepen_root_6601,
 * gj_continuum_product_deepen_root_6301, and
 * gj_graph_milestone_9600 (batch9600). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9601_marker[] = "libcgj-batch9601";

/* Continuum product deepen root fourCC "CPDN". */
#define B9601_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9601_root(void)
{
	return B9601_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_9601 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 9601-9610 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_9601(void)
{
	(void)NULL;
	return b9601_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_9601(void)
    __attribute__((alias("gj_continuum_product_deepen_root_9601")));
