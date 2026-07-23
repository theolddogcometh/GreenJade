/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5521: Steam Runtime "scout" soft ID tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_scout_runtime_id_5521(void);
 *     - Returns the soft compile-time Steam Runtime scout ID (1).
 *       Scout is the legacy Steam Runtime 1 layer (pressure-vessel /
 *       container stack base). Not a filesystem or package probe.
 *   uint32_t __gj_steam_scout_runtime_id_5521  (alias)
 *   __libcgj_batch5521_marker = "libcgj-batch5521"
 *
 * Exclusive continuum CREATE-ONLY (5521-5530: container/flatpak/steam
 * runtime layers, pressure-vessel stubs, scout/soldier/sniper runtime
 * IDs — steam_scout_runtime_id_5521, steam_soldier_runtime_id_5522,
 * steam_sniper_runtime_id_5523, pressure_vessel_stub_ver_5524,
 * flatpak_layer_count_5525, steam_runtime_layer_depth_5526,
 * container_layer_tag_5527, pressure_vessel_wrap_ok_5528,
 * steam_runtime_path_ok_5529, batch_id_5530). Unique
 * gj_steam_scout_runtime_id_5521 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5521_marker[] = "libcgj-batch5521";

/* Soft Steam Runtime scout ID (legacy SR1 / pressure-vessel base). */
#define B5521_SCOUT_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5521_scout_id(void)
{
	return B5521_SCOUT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_scout_runtime_id_5521 - report the soft scout runtime ID.
 *
 * Always returns 1. Compile-time product tag for the Steam Runtime
 * scout layer in the container/pressure-vessel stack. Does not call
 * libc or inspect the host. No parent wires.
 */
uint32_t
gj_steam_scout_runtime_id_5521(void)
{
	(void)NULL;
	return b5521_scout_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_scout_runtime_id_5521(void)
    __attribute__((alias("gj_steam_scout_runtime_id_5521")));
