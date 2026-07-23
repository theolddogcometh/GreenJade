/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5662: Steam client bootstrap phase codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_client_boot_phase_5662(uint32_t phase_id);
 *     - Map bootstrap phase id (0..7) to a stable phase code. Unknown → 0.
 *   uint32_t gj_steam_client_boot_phase_count_5662(void);
 *     - Number of known bootstrap phases (always 8).
 *   uint32_t gj_batch_id_5662(void);
 *   uint32_t __gj_steam_client_boot_phase_5662
 *   uint32_t __gj_steam_client_boot_phase_count_5662
 *   uint32_t __gj_batch_id_5662  (aliases)
 *   __libcgj_batch5662_marker = "libcgj-batch5662"
 *
 * Exclusive continuum CREATE-ONLY (5661-5670: Steam client bootstrap,
 * steam.sh launch codes, steamui ready integers). Unique *_5662
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5662_marker[] = "libcgj-batch5662";

#define B5662_BATCH_ID    5662u
#define B5662_PHASE_MAX   8u

/*
 * Stable bootstrap phase codes (not OS errno / not process exit status):
 *   0 env prep, 1 runtime pin, 2 steam-runtime, 3 bin resolve,
 *   4 ld path, 5 reaper, 6 steamui handoff, 7 ready.
 */
static const uint16_t s_b5662_phase[B5662_PHASE_MAX] = {
	0x4501u, /* 0 ENV  */
	0x5250u, /* 1 RTP  */
	0x5352u, /* 2 SRT  */
	0x4249u, /* 3 BIN  */
	0x4c44u, /* 4 LD   */
	0x5250u, /* 5 RPR  */
	0x5549u, /* 6 UI   */
	0x5244u  /* 7 RDY  */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5662_phase(uint32_t u32Phase)
{
	if (u32Phase >= B5662_PHASE_MAX) {
		return 0u;
	}
	return (uint32_t)s_b5662_phase[u32Phase];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_client_boot_phase_5662 - map bootstrap phase id to phase code.
 *
 * phase_id: 0..7 known; else 0. Soft table only; does not run bootstrap.
 */
uint32_t
gj_steam_client_boot_phase_5662(uint32_t phase_id)
{
	(void)NULL;
	return b5662_phase(phase_id);
}

uint32_t
gj_steam_client_boot_phase_count_5662(void)
{
	return B5662_PHASE_MAX;
}

uint32_t
gj_batch_id_5662(void)
{
	return B5662_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_steam_client_boot_phase_5662(uint32_t phase_id)
    __attribute__((alias("gj_steam_client_boot_phase_5662")));

uint32_t __gj_steam_client_boot_phase_count_5662(void)
    __attribute__((alias("gj_steam_client_boot_phase_count_5662")));

uint32_t __gj_batch_id_5662(void)
    __attribute__((alias("gj_batch_id_5662")));
