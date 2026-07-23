/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5812: bar3 steam bootstrap phase codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_boot_phase_5812(uint32_t phase_id);
 *     - Map bar3/Steam bootstrap phase id (0..7) to a stable phase code.
 *       Unknown phase_id → 0.
 *   uint32_t gj_bar3_steam_boot_phase_count_5812(void);
 *     - Number of known bootstrap phases (always 8).
 *   uint32_t __gj_bar3_steam_boot_phase_5812  (alias)
 *   uint32_t __gj_bar3_steam_boot_phase_count_5812  (alias)
 *   __libcgj_batch5812_marker = "libcgj-batch5812"
 *
 * Exclusive continuum CREATE-ONLY (5811-5820: product bar3 steam
 * bootstrap finalize). Unique *_5812 surfaces only; no multi-def.
 * Distinct from gj_steam_client_boot_phase_5662. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5812_marker[] = "libcgj-batch5812";

#define B5812_PHASE_MAX  8u

/*
 * Stable bar3/Steam bootstrap phase codes:
 *   0 product prep, 1 bar3 gate, 2 steam pin, 3 runtime,
 *   4 deck/top50, 5 install path, 6 finalize handoff, 7 ready.
 */
static const uint16_t s_b5812_phase[B5812_PHASE_MAX] = {
	0x5050u, /* 0 PP  product prep */
	0x4233u, /* 1 B3  bar3 gate */
	0x5354u, /* 2 ST  steam pin */
	0x5254u, /* 3 RT  runtime */
	0x4454u, /* 4 DT  deck/top50 */
	0x4950u, /* 5 IP  install path */
	0x4648u, /* 6 FH  finalize handoff */
	0x5244u  /* 7 RD  ready */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5812_phase(uint32_t u32Phase)
{
	if (u32Phase >= B5812_PHASE_MAX) {
		return 0u;
	}
	return (uint32_t)s_b5812_phase[u32Phase];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_boot_phase_5812 - map bootstrap phase id to phase code.
 *
 * phase_id: 0..7 known; else 0. Soft table only; does not run bootstrap.
 * No parent wires.
 */
uint32_t
gj_bar3_steam_boot_phase_5812(uint32_t phase_id)
{
	(void)NULL;
	return b5812_phase(phase_id);
}

/*
 * gj_bar3_steam_boot_phase_count_5812 - known bootstrap phase count.
 *
 * Always returns 8.
 */
uint32_t
gj_bar3_steam_boot_phase_count_5812(void)
{
	return B5812_PHASE_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bar3_steam_boot_phase_5812(uint32_t phase_id)
    __attribute__((alias("gj_bar3_steam_boot_phase_5812")));

uint32_t __gj_bar3_steam_boot_phase_count_5812(void)
    __attribute__((alias("gj_bar3_steam_boot_phase_count_5812")));
