/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7602: bar3 product deepen phase codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_phase_7602(uint32_t phase_id);
 *     - Map bar3 product-deepen phase id (0..7) to a stable phase
 *       code. Unknown phase_id → 0.
 *   uint32_t gj_bar3_product_deepen_phase_count_7602(void);
 *     - Number of known bar3 product-deepen phases (always 8).
 *   uint32_t __gj_bar3_product_deepen_phase_7602  (alias)
 *   uint32_t __gj_bar3_product_deepen_phase_count_7602  (alias)
 *   __libcgj_batch7602_marker = "libcgj-batch7602"
 *
 * Exclusive continuum CREATE-ONLY (7601-7610: post-7600 continuum
 * product deepen). Unique *_7602 surfaces only; no multi-def. Distinct
 * from gj_bar3_product_deepen_phase_7502,
 * gj_bar3_product_deepen_phase_7302, and
 * gj_bar3_product_deepen_phase_7102. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7602_marker[] = "libcgj-batch7602";

#define B7602_PHASE_MAX  8u

/*
 * Bar3 product-deepen phase codes:
 *   0 bar3 inventory, 1 product plan, 2 deepen expand,
 *   3 bar3 bind, 4 product write, 5 deepen seal,
 *   6 verify seal, 7 ready.
 */
static const uint16_t s_b7602_phase[B7602_PHASE_MAX] = {
	0x4249u, /* 0 BI  bar3 inventory */
	0x5050u, /* 1 PP  product plan */
	0x4445u, /* 2 DE  deepen expand */
	0x4242u, /* 3 BB  bar3 bind */
	0x5057u, /* 4 PW  product write */
	0x4453u, /* 5 DS  deepen seal */
	0x5653u, /* 6 VS  verify seal */
	0x5244u  /* 7 RD  ready */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7602_phase(uint32_t u32Phase)
{
	if (u32Phase >= B7602_PHASE_MAX) {
		return 0u;
	}
	return (uint32_t)s_b7602_phase[u32Phase];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_phase_7602 - map BPD phase id to code.
 *
 * phase_id: 0..7 known; else 0. Soft table only; does not scan media.
 * No parent wires.
 */
uint32_t
gj_bar3_product_deepen_phase_7602(uint32_t phase_id)
{
	(void)NULL;
	return b7602_phase(phase_id);
}

/*
 * gj_bar3_product_deepen_phase_count_7602 - known BPD phase count.
 *
 * Always returns 8.
 */
uint32_t
gj_bar3_product_deepen_phase_count_7602(void)
{
	return B7602_PHASE_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bar3_product_deepen_phase_7602(uint32_t phase_id)
    __attribute__((alias("gj_bar3_product_deepen_phase_7602")));

uint32_t __gj_bar3_product_deepen_phase_count_7602(void)
    __attribute__((alias("gj_bar3_product_deepen_phase_count_7602")));
