/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10103: continuum product deepen path tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_path_10103(void);
 *     - Returns the soft continuum product-deepen path layout tag for
 *       this continuum (always 0x43504450, fourCC "CPDP"). Not a
 *       filesystem path probe or string walk.
 *   uint32_t __gj_continuum_product_deepen_path_10103  (alias)
 *   __libcgj_batch10103_marker = "libcgj-batch10103"
 *
 * Exclusive continuum CREATE-ONLY (10101-10110: post-10100 continuum
 * product deepen). Unique gj_continuum_product_deepen_path_10103
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_path_10003,
 * gj_continuum_product_deepen_path_10000,
 * gj_continuum_product_deepen_root_10101, and
 * gj_continuum_product_deepen_gate_10102. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10103_marker[] = "libcgj-batch10103";

/* Continuum product deepen path fourCC "CPDP". */
#define B10103_CPD_PATH  0x43504450u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10103_path(void)
{
	return B10103_CPD_PATH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_path_10103 - continuum product deepen path.
 *
 * Always returns 0x43504450 ("CPDP"). Soft pure-data path layout tag
 * for the 10101-10110 post-10100 continuum. Does not probe paths
 * or call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_path_10103(void)
{
	(void)NULL;
	return b10103_path();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_path_10103(void)
    __attribute__((alias("gj_continuum_product_deepen_path_10103")));
