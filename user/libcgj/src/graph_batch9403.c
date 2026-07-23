/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9403: continuum product deepen path tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_path_9403(void);
 *     - Returns the soft continuum product-deepen path layout tag for
 *       this continuum (always 0x43504450, fourCC "CPDP"). Not a
 *       filesystem path probe or string walk.
 *   uint32_t __gj_continuum_product_deepen_path_9403  (alias)
 *   __libcgj_batch9403_marker = "libcgj-batch9403"
 *
 * Exclusive continuum CREATE-ONLY (9401-9410: post-9400 continuum
 * product deepen). Unique gj_continuum_product_deepen_path_9403
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_9401,
 * gj_continuum_product_deepen_gate_9402,
 * gj_continuum_product_deepen_path_9303 (batch9303), and
 * gj_continuum_product_deepen_path_9203. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9403_marker[] = "libcgj-batch9403";

/* Continuum product deepen path fourCC "CPDP". */
#define B9403_CPD_PATH  0x43504450u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9403_path(void)
{
	return B9403_CPD_PATH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_path_9403 - continuum product deepen path.
 *
 * Always returns 0x43504450 ("CPDP"). Soft pure-data path layout tag
 * for the 9401-9410 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_path_9403(void)
{
	(void)NULL;
	return b9403_path();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_path_9403(void)
    __attribute__((alias("gj_continuum_product_deepen_path_9403")));
