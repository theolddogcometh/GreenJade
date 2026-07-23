/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5528: pressure-vessel wrap readiness lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_wrap_ok_5528(uint32_t runtime_id);
 *     - Soft pressure-vessel wrap readiness for a Steam Runtime ID.
 *       Returns 1 when runtime_id is scout(1), soldier(2), or
 *       sniper(3); else 0. Not a host tool or bwrap probe.
 *   uint32_t __gj_pressure_vessel_wrap_ok_5528  (alias)
 *   __libcgj_batch5528_marker = "libcgj-batch5528"
 *
 * Exclusive continuum CREATE-ONLY (5521-5530: container/flatpak/steam
 * runtime layers, pressure-vessel stubs, scout/soldier/sniper runtime
 * IDs). Unique gj_pressure_vessel_wrap_ok_5528 surface only; no
 * multi-def. Distinct from pressure_vessel_stub_ver_5524. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5528_marker[] = "libcgj-batch5528";

#define B5528_SCOUT    1u
#define B5528_SOLDIER  2u
#define B5528_SNIPER   3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5528_wrap_ok(uint32_t u32RuntimeId)
{
	if (u32RuntimeId == B5528_SCOUT ||
	    u32RuntimeId == B5528_SOLDIER ||
	    u32RuntimeId == B5528_SNIPER) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_wrap_ok_5528 - soft pressure-vessel wrap lamp.
 *
 * runtime_id: 1=scout, 2=soldier, 3=sniper.
 *
 * Returns 1 for known Steam Runtime IDs, else 0. Soft readiness tag
 * only; does not spawn pressure-vessel or inspect containers. No
 * parent wires.
 */
uint32_t
gj_pressure_vessel_wrap_ok_5528(uint32_t u32RuntimeId)
{
	(void)NULL;
	return b5528_wrap_ok(u32RuntimeId);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_wrap_ok_5528(uint32_t u32RuntimeId)
    __attribute__((alias("gj_pressure_vessel_wrap_ok_5528")));
