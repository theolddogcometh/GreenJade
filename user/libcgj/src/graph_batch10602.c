/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10602: continuum product deepen lane id
 * (wave 6).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_10602(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 10602). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_10602  (alias)
 *   __libcgj_batch10602_marker = "libcgj-batch10602"
 *
 * Exclusive continuum CREATE-ONLY (10601-10610: continuum product
 * deepen wave 6). Unique gj_continuum_product_deepen_lane_10602
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_10502,
 * gj_continuum_product_deepen_lane_10402, and
 * gj_continuum_product_deepen_root_10601. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10602_marker[] = "libcgj-batch10602";

/* Continuum product deepen lane identity (batch number). */
#define B10602_CPD_LANE  10602u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10602_lane(void)
{
	return B10602_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_10602 - continuum product deepen lane.
 *
 * Always returns 10602. Soft pure-data lane identity for the
 * 10601-10610 continuum (wave 6). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_10602(void)
{
	(void)NULL;
	return b10602_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_10602(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_10602")));
