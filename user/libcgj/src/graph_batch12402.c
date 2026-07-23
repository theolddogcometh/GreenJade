/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12402: continuum product deepen lane id
 * (wave 24).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_12402(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 12402). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_12402  (alias)
 *   __libcgj_batch12402_marker = "libcgj-batch12402"
 *
 * Exclusive continuum CREATE-ONLY (12401-12410: continuum product
 * deepen wave 24, post-12400). Unique gj_continuum_product_deepen_lane_12402
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_12302,
 * gj_continuum_product_deepen_lane_12202, and
 * gj_continuum_product_deepen_root_12401. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12402_marker[] = "libcgj-batch12402";

/* Continuum product deepen lane identity (batch number). */
#define B12402_CPD_LANE  12402u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12402_lane(void)
{
	return B12402_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_12402 - continuum product deepen lane.
 *
 * Always returns 12402. Soft pure-data lane identity for the
 * 12401-12410 continuum (wave 24). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_12402(void)
{
	(void)NULL;
	return b12402_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_12402(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_12402")));
