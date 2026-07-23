/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5409: steam runtime roots (core).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_root_ok_5409(const char *path);
 *     - Return 1 if path is absolute with length >= 2, else 0.
 *   uint32_t gj_steam_runtime_soldier_5409(void);
 *     - Returns 1 (soft soldier runtime generation tag).
 *   uint32_t gj_steam_runtime_sniper_5409(void);
 *     - Returns 2 (soft sniper runtime generation tag).
 *   uint32_t gj_steam_runtime_ready_5409(void);
 *     - Returns 0 (soft: runtime root not ready at compile time).
 *   uint32_t __gj_steam_runtime_root_ok_5409  (alias)
 *   uint32_t __gj_steam_runtime_soldier_5409  (alias)
 *   uint32_t __gj_steam_runtime_sniper_5409  (alias)
 *   uint32_t __gj_steam_runtime_ready_5409  (alias)
 *   __libcgj_batch5409_marker = "libcgj-batch5409"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410). Steam runtime roots
 * toward bar #3 Deck Top 50 / Steam Deck install. Unique batch-
 * suffixed symbols; distinct from gj_steam_path_is_compatdata
 * (batch1865). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5409_marker[] = "libcgj-batch5409";

#define B5409_SOLDIER   1u
#define B5409_SNIPER    2u
#define B5409_NOT_READY 0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5409_root_ok(const char *szPath)
{
	if (szPath == NULL) {
		return 0u;
	}
	if (szPath[0] != '/') {
		return 0u;
	}
	if (szPath[1] == '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_root_ok_5409 - soft absolute steam-runtime root check.
 *
 * path: NUL-terminated runtime root (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and length >= 2, else 0.
 * Does not walk filesystems or call libc.
 */
uint32_t
gj_steam_runtime_root_ok_5409(const char *szPath)
{
	(void)NULL;
	return b5409_root_ok(szPath);
}

/*
 * gj_steam_runtime_soldier_5409 - soft soldier runtime generation tag.
 *
 * Always returns 1.
 */
uint32_t
gj_steam_runtime_soldier_5409(void)
{
	return B5409_SOLDIER;
}

/*
 * gj_steam_runtime_sniper_5409 - soft sniper runtime generation tag.
 *
 * Always returns 2.
 */
uint32_t
gj_steam_runtime_sniper_5409(void)
{
	return B5409_SNIPER;
}

/*
 * gj_steam_runtime_ready_5409 - soft steam-runtime ready lamp.
 *
 * Always returns 0 (not ready at compile time). No path probe.
 */
uint32_t
gj_steam_runtime_ready_5409(void)
{
	return B5409_NOT_READY;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_steam_runtime_root_ok_5409(const char *szPath)
    __attribute__((alias("gj_steam_runtime_root_ok_5409")));

uint32_t __gj_steam_runtime_soldier_5409(void)
    __attribute__((alias("gj_steam_runtime_soldier_5409")));

uint32_t __gj_steam_runtime_sniper_5409(void)
    __attribute__((alias("gj_steam_runtime_sniper_5409")));

uint32_t __gj_steam_runtime_ready_5409(void)
    __attribute__((alias("gj_steam_runtime_ready_5409")));
