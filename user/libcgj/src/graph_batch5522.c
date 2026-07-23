/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5522: Steam Runtime "soldier" soft ID tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_soldier_runtime_id_5522(void);
 *     - Returns the soft compile-time Steam Runtime soldier ID (2).
 *       Soldier is Steam Runtime 2 (pressure-vessel middle layer).
 *       Not a filesystem or package probe.
 *   uint32_t __gj_steam_soldier_runtime_id_5522  (alias)
 *   __libcgj_batch5522_marker = "libcgj-batch5522"
 *
 * Exclusive continuum CREATE-ONLY (5521-5530: container/flatpak/steam
 * runtime layers, pressure-vessel stubs, scout/soldier/sniper runtime
 * IDs). Unique gj_steam_soldier_runtime_id_5522 surface only; no
 * multi-def. Distinct from gj_steam_scout_runtime_id_5521 and
 * gj_steam_sniper_runtime_id_5523. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5522_marker[] = "libcgj-batch5522";

/* Soft Steam Runtime soldier ID (SR2 / pressure-vessel middle). */
#define B5522_SOLDIER_ID  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5522_soldier_id(void)
{
	return B5522_SOLDIER_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_soldier_runtime_id_5522 - report the soft soldier runtime ID.
 *
 * Always returns 2. Compile-time product tag for the Steam Runtime
 * soldier layer. Does not call libc or inspect the host. No parent
 * wires.
 */
uint32_t
gj_steam_soldier_runtime_id_5522(void)
{
	(void)NULL;
	return b5522_soldier_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_soldier_runtime_id_5522(void)
    __attribute__((alias("gj_steam_soldier_runtime_id_5522")));
