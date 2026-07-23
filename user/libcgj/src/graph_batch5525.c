/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5525: flatpak runtime layer count helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_layer_count_5525(uint32_t flags);
 *     - Soft flatpak runtime layer count from a packed flags word.
 *       Low 8 bits are treated as the declared layer count (capped
 *       at 64). Zero flags yields the default stack depth of 3
 *       (app / runtime / extension-style soft model).
 *   uint32_t __gj_flatpak_layer_count_5525  (alias)
 *   __libcgj_batch5525_marker = "libcgj-batch5525"
 *
 * Exclusive continuum CREATE-ONLY (5521-5530: container/flatpak/steam
 * runtime layers, pressure-vessel stubs, scout/soldier/sniper runtime
 * IDs). Unique gj_flatpak_layer_count_5525 surface only; no multi-def.
 * Distinct from gj_steam_runtime_layer_depth_5526. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5525_marker[] = "libcgj-batch5525";

/* Default soft flatpak stack depth when flags is zero. */
#define B5525_DEFAULT_LAYERS  3u
/* Hard cap on declared layer count (low 8 bits of flags). */
#define B5525_MAX_LAYERS      64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5525_layer_count(uint32_t u32Flags)
{
	uint32_t u32N;

	if (u32Flags == 0u) {
		return B5525_DEFAULT_LAYERS;
	}
	u32N = u32Flags & 0xffu;
	if (u32N > B5525_MAX_LAYERS) {
		return B5525_MAX_LAYERS;
	}
	return u32N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_layer_count_5525 - soft flatpak runtime layer count.
 *
 * flags: packed descriptor; low 8 bits = declared layer count.
 *        Zero means use default depth 3.
 *
 * Returns the clamped layer count (1..64 when non-zero, else 3).
 * Integer-only; no OS or flatpak bus calls. No parent wires.
 */
uint32_t
gj_flatpak_layer_count_5525(uint32_t u32Flags)
{
	(void)NULL;
	return b5525_layer_count(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_layer_count_5525(uint32_t u32Flags)
    __attribute__((alias("gj_flatpak_layer_count_5525")));
