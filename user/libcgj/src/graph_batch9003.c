/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9003: continuum product deepen path tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_path_9003(void);
 *     - Returns the soft continuum product-deepen path layout tag for
 *       this continuum (always 0x43504450, fourCC "CPDP"). Not a
 *       filesystem path probe or string walk.
 *   uint32_t __gj_continuum_product_deepen_path_9003  (alias)
 *   __libcgj_batch9003_marker = "libcgj-batch9003"
 *
 * Exclusive continuum CREATE-ONLY (9001-9010: post-9000 continuum
 * product deepen). Unique gj_continuum_product_deepen_path_9003
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_9001,
 * gj_continuum_product_deepen_gate_9002,
 * gj_continuum_product_deepen_path_8203, and
 * gj_continuum_product_deepen_root_8201. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9003_marker[] = "libcgj-batch9003";

/* Continuum product deepen path fourCC "CPDP". */
#define B9003_CPD_PATH  0x43504450u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9003_path(void)
{
	return B9003_CPD_PATH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_path_9003 - continuum product deepen path.
 *
 * Always returns 0x43504450 ("CPDP"). Soft pure-data path layout tag
 * for the 9001-9010 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_path_9003(void)
{
	(void)NULL;
	return b9003_path();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_path_9003(void)
    __attribute__((alias("gj_continuum_product_deepen_path_9003")));
