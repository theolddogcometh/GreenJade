/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6102: product install deepen phase codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_install_deepen_phase_6102(uint32_t phase_id);
 *     - Map product install-deepen phase id (0..7) to a stable phase
 *       code. Unknown phase_id → 0.
 *   uint32_t gj_product_install_deepen_phase_count_6102(void);
 *     - Number of known install-deepen phases (always 8).
 *   uint32_t __gj_product_install_deepen_phase_6102  (alias)
 *   uint32_t __gj_product_install_deepen_phase_count_6102  (alias)
 *   __libcgj_batch6102_marker = "libcgj-batch6102"
 *
 * Exclusive continuum CREATE-ONLY (6101-6110: post-6100 product
 * deepen install). Unique *_6102 surfaces only; no multi-def.
 * Distinct from gj_bar3_install_media_phase_5802 and
 * gj_install_media_detect_5403. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6102_marker[] = "libcgj-batch6102";

#define B6102_PHASE_MAX  8u

/*
 * Product install-deepen phase codes:
 *   0 product inventory, 1 deepen plan, 2 path expand,
 *   3 ESP deepen, 4 image bind, 5 install write,
 *   6 verify seal, 7 ready.
 */
static const uint16_t s_b6102_phase[B6102_PHASE_MAX] = {
	0x5049u, /* 0 PI  product inventory */
	0x4450u, /* 1 DP  deepen plan */
	0x5045u, /* 2 PE  path expand */
	0x4544u, /* 3 ED  ESP deepen */
	0x4942u, /* 4 IB  image bind */
	0x4957u, /* 5 IW  install write */
	0x5653u, /* 6 VS  verify seal */
	0x5244u  /* 7 RD  ready */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6102_phase(uint32_t u32Phase)
{
	if (u32Phase >= B6102_PHASE_MAX) {
		return 0u;
	}
	return (uint32_t)s_b6102_phase[u32Phase];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_install_deepen_phase_6102 - map install-deepen phase id.
 *
 * phase_id: 0..7 known; else 0. Soft table only; does not scan media.
 * No parent wires.
 */
uint32_t
gj_product_install_deepen_phase_6102(uint32_t phase_id)
{
	(void)NULL;
	return b6102_phase(phase_id);
}

/*
 * gj_product_install_deepen_phase_count_6102 - known deepen phases.
 *
 * Always returns 8.
 */
uint32_t
gj_product_install_deepen_phase_count_6102(void)
{
	return B6102_PHASE_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_product_install_deepen_phase_6102(uint32_t phase_id)
    __attribute__((alias("gj_product_install_deepen_phase_6102")));

uint32_t __gj_product_install_deepen_phase_count_6102(void)
    __attribute__((alias("gj_product_install_deepen_phase_count_6102")));
