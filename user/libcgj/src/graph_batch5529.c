/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5529: Steam runtime path layout readiness.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_path_ok_5529(uint32_t flags);
 *     - Soft Steam runtime path/layout readiness from packed flags:
 *         bit0: scout path present (soft)
 *         bit1: soldier path present (soft)
 *         bit2: sniper path present (soft)
 *         bit3: pressure-vessel wrap path present (soft)
 *       Returns 1 when at least one of bits 0..2 is set AND bit3 is
 *       set (a runtime layer plus wrap path). Else 0.
 *   uint32_t __gj_steam_runtime_path_ok_5529  (alias)
 *   __libcgj_batch5529_marker = "libcgj-batch5529"
 *
 * Exclusive continuum CREATE-ONLY (5521-5530: container/flatpak/steam
 * runtime layers, pressure-vessel stubs, scout/soldier/sniper runtime
 * IDs). Unique gj_steam_runtime_path_ok_5529 surface only; no
 * multi-def. Distinct from pressure_vessel_wrap_ok_5528. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5529_marker[] = "libcgj-batch5529";

#define B5529_BIT_SCOUT    0x01u
#define B5529_BIT_SOLDIER  0x02u
#define B5529_BIT_SNIPER   0x04u
#define B5529_BIT_PV_WRAP  0x08u
#define B5529_RUNTIME_MASK (B5529_BIT_SCOUT | B5529_BIT_SOLDIER | \
                            B5529_BIT_SNIPER)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5529_path_ok(uint32_t u32Flags)
{
	if ((u32Flags & B5529_RUNTIME_MASK) == 0u) {
		return 0u;
	}
	if ((u32Flags & B5529_BIT_PV_WRAP) == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_path_ok_5529 - soft Steam runtime path readiness.
 *
 * flags: bit0 scout, bit1 soldier, bit2 sniper, bit3 pressure-vessel
 *        wrap path (all soft/caller-supplied; not host probes).
 *
 * Returns 1 when any runtime bit and the wrap bit are set; else 0.
 * Integer-only. No parent wires.
 */
uint32_t
gj_steam_runtime_path_ok_5529(uint32_t u32Flags)
{
	(void)NULL;
	return b5529_path_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_runtime_path_ok_5529(uint32_t u32Flags)
    __attribute__((alias("gj_steam_runtime_path_ok_5529")));
