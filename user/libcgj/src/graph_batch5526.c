/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5526: Steam runtime layer stack depth.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_layer_depth_5526(uint32_t runtime_id);
 *     - Soft stack depth for a Steam Runtime ID (scout=1, soldier=2,
 *       sniper=3). Unknown IDs return 0. Models pressure-vessel
 *       runtime layering without probing the host.
 *   uint32_t __gj_steam_runtime_layer_depth_5526  (alias)
 *   __libcgj_batch5526_marker = "libcgj-batch5526"
 *
 * Exclusive continuum CREATE-ONLY (5521-5530: container/flatpak/steam
 * runtime layers, pressure-vessel stubs, scout/soldier/sniper runtime
 * IDs). Unique gj_steam_runtime_layer_depth_5526 surface only; no
 * multi-def. Distinct from flatpak_layer_count_5525. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5526_marker[] = "libcgj-batch5526";

/* Soft Steam Runtime IDs (match 5521/5522/5523 tags). */
#define B5526_SCOUT    1u
#define B5526_SOLDIER  2u
#define B5526_SNIPER   3u

/* Soft layer depths for each known runtime. */
#define B5526_DEPTH_SCOUT    1u
#define B5526_DEPTH_SOLDIER  2u
#define B5526_DEPTH_SNIPER   3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5526_layer_depth(uint32_t u32RuntimeId)
{
	if (u32RuntimeId == B5526_SCOUT) {
		return B5526_DEPTH_SCOUT;
	}
	if (u32RuntimeId == B5526_SOLDIER) {
		return B5526_DEPTH_SOLDIER;
	}
	if (u32RuntimeId == B5526_SNIPER) {
		return B5526_DEPTH_SNIPER;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_layer_depth_5526 - soft Steam runtime stack depth.
 *
 * runtime_id: 1=scout, 2=soldier, 3=sniper; others unknown.
 *
 * Returns 1/2/3 for known IDs, else 0. Integer-only soft model of
 * pressure-vessel runtime layering. No parent wires.
 */
uint32_t
gj_steam_runtime_layer_depth_5526(uint32_t u32RuntimeId)
{
	(void)NULL;
	return b5526_layer_depth(u32RuntimeId);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_runtime_layer_depth_5526(uint32_t u32RuntimeId)
    __attribute__((alias("gj_steam_runtime_layer_depth_5526")));
