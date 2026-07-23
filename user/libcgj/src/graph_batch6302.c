/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6302: continuum product deepen phase codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_phase_6302(uint32_t phase_id);
 *     - Map continuum product-deepen phase id (0..7) to a stable phase
 *       code. Unknown phase_id → 0.
 *   uint32_t gj_continuum_product_deepen_phase_count_6302(void);
 *     - Number of known continuum product-deepen phases (always 8).
 *   uint32_t __gj_continuum_product_deepen_phase_6302  (alias)
 *   uint32_t __gj_continuum_product_deepen_phase_count_6302  (alias)
 *   __libcgj_batch6302_marker = "libcgj-batch6302"
 *
 * Exclusive continuum CREATE-ONLY (6301-6310: post-6300 continuum
 * product deepen). Unique *_6302 surfaces only; no multi-def.
 * Distinct from gj_product_install_deepen_phase_6102 and
 * gj_bar3_install_media_phase_5802. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6302_marker[] = "libcgj-batch6302";

#define B6302_PHASE_MAX  8u

/*
 * Continuum product-deepen phase codes:
 *   0 continuum inventory, 1 product plan, 2 deepen expand,
 *   3 continuum bind, 4 product write, 5 deepen seal,
 *   6 verify seal, 7 ready.
 */
static const uint16_t s_b6302_phase[B6302_PHASE_MAX] = {
	0x4349u, /* 0 CI  continuum inventory */
	0x5050u, /* 1 PP  product plan */
	0x4445u, /* 2 DE  deepen expand */
	0x4342u, /* 3 CB  continuum bind */
	0x5057u, /* 4 PW  product write */
	0x4453u, /* 5 DS  deepen seal */
	0x5653u, /* 6 VS  verify seal */
	0x5244u  /* 7 RD  ready */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6302_phase(uint32_t u32Phase)
{
	if (u32Phase >= B6302_PHASE_MAX) {
		return 0u;
	}
	return (uint32_t)s_b6302_phase[u32Phase];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_phase_6302 - map CPD phase id to code.
 *
 * phase_id: 0..7 known; else 0. Soft table only; does not scan media.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_phase_6302(uint32_t phase_id)
{
	(void)NULL;
	return b6302_phase(phase_id);
}

/*
 * gj_continuum_product_deepen_phase_count_6302 - known CPD phase count.
 *
 * Always returns 8.
 */
uint32_t
gj_continuum_product_deepen_phase_count_6302(void)
{
	return B6302_PHASE_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_continuum_product_deepen_phase_6302(uint32_t phase_id)
    __attribute__((alias("gj_continuum_product_deepen_phase_6302")));

uint32_t __gj_continuum_product_deepen_phase_count_6302(void)
    __attribute__((alias("gj_continuum_product_deepen_phase_count_6302")));
