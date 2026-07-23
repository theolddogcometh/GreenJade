/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6502: bar3 install checklist deepen phases.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_checklist_deepen_phase_6502(uint32_t phase_id);
 *     - Map bar3 install-checklist deepen phase id (0..7) to a stable
 *       phase code. Unknown phase_id → 0.
 *   uint32_t gj_bar3_install_checklist_deepen_phase_count_6502(void);
 *     - Number of known checklist-deepen phases (always 8).
 *   uint32_t __gj_bar3_install_checklist_deepen_phase_6502  (alias)
 *   uint32_t __gj_bar3_install_checklist_deepen_phase_count_6502  (alias)
 *   __libcgj_batch6502_marker = "libcgj-batch6502"
 *
 * Exclusive continuum CREATE-ONLY (6501-6510: post-6500 bar3 install
 * checklist deepen). Unique *_6502 surfaces only; no multi-def.
 * Distinct from gj_product_install_deepen_phase_6102 and
 * gj_continuum_product_deepen_phase_6302. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6502_marker[] = "libcgj-batch6502";

#define B6502_PHASE_MAX  8u

/*
 * Bar3 install-checklist deepen phase codes:
 *   0 steam inventory, 1 deck plan, 2 top50 expand,
 *   3 install bind, 4 checklist mask, 5 deepen seal,
 *   6 verify seal, 7 ready.
 */
static const uint16_t s_b6502_phase[B6502_PHASE_MAX] = {
	0x5349u, /* 0 SI  steam inventory */
	0x4450u, /* 1 DP  deck plan */
	0x5445u, /* 2 TE  top50 expand */
	0x4942u, /* 3 IB  install bind */
	0x434Du, /* 4 CM  checklist mask */
	0x4453u, /* 5 DS  deepen seal */
	0x5653u, /* 6 VS  verify seal */
	0x5244u  /* 7 RD  ready */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6502_phase(uint32_t u32Phase)
{
	if (u32Phase >= B6502_PHASE_MAX) {
		return 0u;
	}
	return (uint32_t)s_b6502_phase[u32Phase];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_checklist_deepen_phase_6502 - map checklist phase id.
 *
 * phase_id: 0..7 known; else 0. Soft table only; does not scan media.
 * No parent wires.
 */
uint32_t
gj_bar3_install_checklist_deepen_phase_6502(uint32_t phase_id)
{
	(void)NULL;
	return b6502_phase(phase_id);
}

/*
 * gj_bar3_install_checklist_deepen_phase_count_6502 - known phase count.
 *
 * Always returns 8.
 */
uint32_t
gj_bar3_install_checklist_deepen_phase_count_6502(void)
{
	return B6502_PHASE_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bar3_install_checklist_deepen_phase_6502(uint32_t phase_id)
    __attribute__((alias("gj_bar3_install_checklist_deepen_phase_6502")));

uint32_t __gj_bar3_install_checklist_deepen_phase_count_6502(void)
    __attribute__((alias("gj_bar3_install_checklist_deepen_phase_count_6502")));
