/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8203: continuum product deepen path tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_path_8203(void);
 *     - Returns the soft continuum product-deepen path layout tag for
 *       this continuum (always 0x43504450, fourCC "CPDP"). Not a
 *       filesystem path probe or string walk.
 *   uint32_t __gj_continuum_product_deepen_path_8203  (alias)
 *   __libcgj_batch8203_marker = "libcgj-batch8203"
 *
 * Exclusive continuum CREATE-ONLY (8201-8210: post-8200 continuum
 * product deepen). Unique gj_continuum_product_deepen_path_8203
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_8201,
 * gj_continuum_product_deepen_gate_8202, and
 * gj_continuum_product_deepen_root_8001. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8203_marker[] = "libcgj-batch8203";

/* Continuum product deepen path fourCC "CPDP". */
#define B8203_CPD_PATH  0x43504450u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8203_path(void)
{
	return B8203_CPD_PATH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_path_8203 - continuum product deepen path.
 *
 * Always returns 0x43504450 ("CPDP"). Soft pure-data path layout tag
 * for the 8201-8210 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_path_8203(void)
{
	(void)NULL;
	return b8203_path();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_path_8203(void)
    __attribute__((alias("gj_continuum_product_deepen_path_8203")));
