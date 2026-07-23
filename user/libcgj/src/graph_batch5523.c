/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5523: Steam Runtime "sniper" soft ID tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_sniper_runtime_id_5523(void);
 *     - Returns the soft compile-time Steam Runtime sniper ID (3).
 *       Sniper is Steam Runtime 3 (pressure-vessel current layer).
 *       Not a filesystem or package probe.
 *   uint32_t __gj_steam_sniper_runtime_id_5523  (alias)
 *   __libcgj_batch5523_marker = "libcgj-batch5523"
 *
 * Exclusive continuum CREATE-ONLY (5521-5530: container/flatpak/steam
 * runtime layers, pressure-vessel stubs, scout/soldier/sniper runtime
 * IDs). Unique gj_steam_sniper_runtime_id_5523 surface only; no
 * multi-def. Distinct from gj_steam_scout_runtime_id_5521 and
 * gj_steam_soldier_runtime_id_5522. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5523_marker[] = "libcgj-batch5523";

/* Soft Steam Runtime sniper ID (SR3 / pressure-vessel current). */
#define B5523_SNIPER_ID  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5523_sniper_id(void)
{
	return B5523_SNIPER_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_sniper_runtime_id_5523 - report the soft sniper runtime ID.
 *
 * Always returns 3. Compile-time product tag for the Steam Runtime
 * sniper layer. Does not call libc or inspect the host. No parent
 * wires.
 */
uint32_t
gj_steam_sniper_runtime_id_5523(void)
{
	(void)NULL;
	return b5523_sniper_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_sniper_runtime_id_5523(void)
    __attribute__((alias("gj_steam_sniper_runtime_id_5523")));
