/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5802: bar3 install media phase codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_media_phase_5802(uint32_t phase_id);
 *     - Map bar3/install-media phase id (0..7) to a stable phase code.
 *       Unknown phase_id → 0.
 *   uint32_t gj_bar3_install_media_phase_count_5802(void);
 *     - Number of known install-media phases (always 8).
 *   uint32_t __gj_bar3_install_media_phase_5802  (alias)
 *   uint32_t __gj_bar3_install_media_phase_count_5802  (alias)
 *   __libcgj_batch5802_marker = "libcgj-batch5802"
 *
 * Exclusive continuum CREATE-ONLY (5801-5810: product bar3 install
 * media finalize). Unique *_5802 surfaces only; no multi-def.
 * Distinct from gj_bar3_steam_boot_phase_5812 and
 * gj_install_media_detect_5403. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5802_marker[] = "libcgj-batch5802";

#define B5802_PHASE_MAX  8u

/*
 * Stable bar3/install-media phase codes:
 *   0 product prep, 1 bar3 gate, 2 media detect, 3 size/checksum,
 *   4 install path, 5 ESP layout, 6 finalize handoff, 7 ready.
 */
static const uint16_t s_b5802_phase[B5802_PHASE_MAX] = {
	0x5050u, /* 0 PP  product prep */
	0x4233u, /* 1 B3  bar3 gate */
	0x4D44u, /* 2 MD  media detect */
	0x5343u, /* 3 SC  size/checksum */
	0x4950u, /* 4 IP  install path */
	0x4550u, /* 5 EP  ESP layout */
	0x4648u, /* 6 FH  finalize handoff */
	0x5244u  /* 7 RD  ready */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5802_phase(uint32_t u32Phase)
{
	if (u32Phase >= B5802_PHASE_MAX) {
		return 0u;
	}
	return (uint32_t)s_b5802_phase[u32Phase];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_media_phase_5802 - map install-media phase id to code.
 *
 * phase_id: 0..7 known; else 0. Soft table only; does not scan media.
 * No parent wires.
 */
uint32_t
gj_bar3_install_media_phase_5802(uint32_t phase_id)
{
	(void)NULL;
	return b5802_phase(phase_id);
}

/*
 * gj_bar3_install_media_phase_count_5802 - known install-media phases.
 *
 * Always returns 8.
 */
uint32_t
gj_bar3_install_media_phase_count_5802(void)
{
	return B5802_PHASE_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bar3_install_media_phase_5802(uint32_t phase_id)
    __attribute__((alias("gj_bar3_install_media_phase_5802")));

uint32_t __gj_bar3_install_media_phase_count_5802(void)
    __attribute__((alias("gj_bar3_install_media_phase_count_5802")));
