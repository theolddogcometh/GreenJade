/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6504: bar3 install checklist deepen steps.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_checklist_deepen_step_6504(uint32_t step_id);
 *     - Map bar3 install-checklist deepen step id (0..5) to a stable
 *       step code. Unknown step_id → 0.
 *   uint32_t gj_bar3_install_checklist_deepen_step_count_6504(void);
 *     - Number of known checklist-deepen steps (always 6).
 *   uint32_t __gj_bar3_install_checklist_deepen_step_6504  (alias)
 *   uint32_t __gj_bar3_install_checklist_deepen_step_count_6504  (alias)
 *   __libcgj_batch6504_marker = "libcgj-batch6504"
 *
 * Exclusive continuum CREATE-ONLY (6501-6510: post-6500 bar3 install
 * checklist deepen). Unique *_6504 surfaces only; no multi-def.
 * Distinct from gj_product_install_deepen_step_6104 and
 * gj_continuum_product_deepen_step_6304. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6504_marker[] = "libcgj-batch6504";

#define B6504_STEP_MAX  6u

/*
 * Bar3 install-checklist deepen step codes:
 *   0 steam slot, 1 deck slot, 2 top50 slot, 3 install slot,
 *   4 mask merge, 5 checklist seal.
 */
static const uint16_t s_b6504_step[B6504_STEP_MAX] = {
	0x5353u, /* 0 SS  steam slot */
	0x4453u, /* 1 DS  deck slot */
	0x5453u, /* 2 TS  top50 slot */
	0x4953u, /* 3 IS  install slot */
	0x4D4Du, /* 4 MM  mask merge */
	0x4353u  /* 5 CS  checklist seal */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6504_step(uint32_t u32Step)
{
	if (u32Step >= B6504_STEP_MAX) {
		return 0u;
	}
	return (uint32_t)s_b6504_step[u32Step];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_checklist_deepen_step_6504 - map deepen step id.
 *
 * step_id: 0..5 known; else 0. Soft table only; does not write images.
 * No parent wires.
 */
uint32_t
gj_bar3_install_checklist_deepen_step_6504(uint32_t step_id)
{
	(void)NULL;
	return b6504_step(step_id);
}

/*
 * gj_bar3_install_checklist_deepen_step_count_6504 - known step count.
 *
 * Always returns 6.
 */
uint32_t
gj_bar3_install_checklist_deepen_step_count_6504(void)
{
	return B6504_STEP_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bar3_install_checklist_deepen_step_6504(uint32_t step_id)
    __attribute__((alias("gj_bar3_install_checklist_deepen_step_6504")));

uint32_t __gj_bar3_install_checklist_deepen_step_count_6504(void)
    __attribute__((alias("gj_bar3_install_checklist_deepen_step_count_6504")));
