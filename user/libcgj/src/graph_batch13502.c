/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13502: continuum product deepen lane id
 * (wave 35).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_13502(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 13502). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_13502  (alias)
 *   __libcgj_batch13502_marker = "libcgj-batch13502"
 *
 * Exclusive continuum CREATE-ONLY (13501-13510: continuum product
 * deepen wave 35, post-13500). Unique gj_continuum_product_deepen_lane_13502
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_13402,
 * gj_continuum_product_deepen_lane_13302, and
 * gj_continuum_product_deepen_root_13501. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13502_marker[] = "libcgj-batch13502";

/* Continuum product deepen lane identity (batch number). */
#define B13502_CPD_LANE  13502u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13502_lane(void)
{
	return B13502_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_13502 - continuum product deepen lane.
 *
 * Always returns 13502. Soft pure-data lane identity for the
 * 13501-13510 continuum (wave 35). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_13502(void)
{
	(void)NULL;
	return b13502_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_13502(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_13502")));
