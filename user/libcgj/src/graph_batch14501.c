/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14501: continuum product deepen root tag
 * (wave 45).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_14501(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_14501  (alias)
 *   __libcgj_batch14501_marker = "libcgj-batch14501"
 *
 * Exclusive continuum CREATE-ONLY (14501-14510: continuum product deepen wave 45, post-14500). Unique surface only; no multi-def. Distinct from prior continuum_product_deepen / continuum_smoke_soft / continuum_dyn_soft symbols (1440x/1430x/…). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14501_marker[] = "libcgj-batch14501";

/* Continuum product deepen root fourCC "CPDN". */
#define B14501_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14501_root(void)
{
	return B14501_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_14501 - soft continuum surface
 *
 * Always returns 0x4350444E. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_14501(void)
{
	(void)NULL;
	return b14501_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_14501(void)
    __attribute__((alias("gj_continuum_product_deepen_root_14501")));
