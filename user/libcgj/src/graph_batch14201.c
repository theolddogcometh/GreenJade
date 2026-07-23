/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14201: continuum product deepen root tag
 * (wave 42).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_14201(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_14201  (alias)
 *   __libcgj_batch14201_marker = "libcgj-batch14201"
 *
 * Exclusive continuum CREATE-ONLY (14201-14210: continuum product
 * deepen wave 42, post-14200). Unique gj_continuum_product_deepen_root_14201
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_14101,
 * gj_continuum_product_deepen_root_14001, and
 * gj_continuum_product_deepen_root_13901. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14201_marker[] = "libcgj-batch14201";

/* Continuum product deepen root fourCC "CPDN". */
#define B14201_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14201_root(void)
{
	return B14201_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_14201 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 14201-14210 continuum (wave 42). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_14201(void)
{
	(void)NULL;
	return b14201_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_14201(void)
    __attribute__((alias("gj_continuum_product_deepen_root_14201")));
