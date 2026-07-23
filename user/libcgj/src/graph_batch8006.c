/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8006: continuum product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_code_8006(void);
 *     - Returns the continuum product-deepen code fourCC for this
 *       continuum (always 0x43443830, fourCC "CD80"). Soft pure-data
 *       code tag; not a live product checksum.
 *   uint32_t __gj_continuum_product_deepen_code_8006  (alias)
 *   __libcgj_batch8006_marker = "libcgj-batch8006"
 *
 * Exclusive continuum CREATE-ONLY (8001-8010: post-8000 continuum
 * product deepen). Unique gj_continuum_product_deepen_code_8006
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_code_7006,
 * gj_continuum_product_deepen_code_6806,
 * gj_continuum_product_deepen_code_6606, and
 * gj_continuum_product_deepen_root_8001. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8006_marker[] = "libcgj-batch8006";

/* Continuum product deepen code fourCC "CD80". */
#define B8006_CPD_CODE  0x43443830u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8006_code(void)
{
	return B8006_CPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_code_8006 - continuum product deepen code.
 *
 * Always returns 0x43443830 ("CD80"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_code_8006(void)
{
	(void)NULL;
	return b8006_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_code_8006(void)
    __attribute__((alias("gj_continuum_product_deepen_code_8006")));
