/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5524: pressure-vessel stub version tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_stub_ver_5524(void);
 *     - Returns the soft pressure-vessel stub ABI version (1).
 *       Compile-time product tag for the pressure-vessel container
 *       wrapper surface used with scout/soldier/sniper runtimes.
 *   uint32_t __gj_pressure_vessel_stub_ver_5524  (alias)
 *   __libcgj_batch5524_marker = "libcgj-batch5524"
 *
 * Exclusive continuum CREATE-ONLY (5521-5530: container/flatpak/steam
 * runtime layers, pressure-vessel stubs, scout/soldier/sniper runtime
 * IDs). Unique gj_pressure_vessel_stub_ver_5524 surface only; no
 * multi-def. Distinct from wrap_ok (batch5528). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5524_marker[] = "libcgj-batch5524";

/* Soft pressure-vessel stub ABI version. */
#define B5524_PV_STUB_VER  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5524_stub_ver(void)
{
	return B5524_PV_STUB_VER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_stub_ver_5524 - report pressure-vessel stub version.
 *
 * Always returns 1. Soft compile-time ABI tag for the pressure-vessel
 * stub layer. Does not call libc, open containers, or probe host
 * tools. No parent wires.
 */
uint32_t
gj_pressure_vessel_stub_ver_5524(void)
{
	(void)NULL;
	return b5524_stub_ver();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_stub_ver_5524(void)
    __attribute__((alias("gj_pressure_vessel_stub_ver_5524")));
