/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14601: continuum product deepen root tag
 * (wave 46).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_14601(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_14601  (alias)
 *   __libcgj_batch14601_marker = "libcgj-batch14601"
 *
 * Exclusive continuum CREATE-ONLY (14601-14610: continuum product deepen wave 46, post-14600). Unique surface only; no multi-def. Distinct from prior continuum_product_deepen / continuum_smoke_soft / continuum_dyn_soft symbols (1450x/1440x/…). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14601_marker[] = "libcgj-batch14601";

/* Continuum product deepen root fourCC "CPDN". */
#define B14601_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14601_root(void)
{
	return B14601_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_14601 - soft continuum surface
 *
 * Always returns 0x4350444Eu. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_14601(void)
{
	(void)NULL;
	return b14601_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_14601(void)
    __attribute__((alias("gj_continuum_product_deepen_root_14601")));
