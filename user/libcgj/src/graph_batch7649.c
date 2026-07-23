/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7649: sysinfo mem_unit validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sysinfo_mem_unit_ok_u_7649(uint32_t mem_unit);
 *     - Return 1 if mem_unit is non-zero (usable scale factor), else 0.
 *   uint32_t __gj_sysinfo_mem_unit_ok_u_7649  (alias)
 *   __libcgj_batch7649_marker = "libcgj-batch7649"
 *
 * Exclusive continuum CREATE-ONLY (7641-7650: sysinfo field stubs —
 * uptime_id, loads_slots, totalram_id, freeram_ok, sharedram_id,
 * bufferram_id, totalswap_id, freeswap_ok, mem_unit_ok, batch_id_7650).
 * Unique gj_sysinfo_mem_unit_ok_u_7649 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7649_marker[] = "libcgj-batch7649";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7649_sysinfo_mem_unit_ok(uint32_t u32MemUnit)
{
	return (u32MemUnit != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sysinfo_mem_unit_ok_u_7649 - 1 if mem_unit is a usable scale factor.
 *
 * mem_unit: soft sysinfo.mem_unit catalog value (bytes per memory unit)
 *
 * Returns 1 when mem_unit != 0, else 0. Soft field check; does not call
 * sysinfo(2). No parent wires.
 */
uint32_t
gj_sysinfo_mem_unit_ok_u_7649(uint32_t u32MemUnit)
{
	(void)NULL;
	return b7649_sysinfo_mem_unit_ok(u32MemUnit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sysinfo_mem_unit_ok_u_7649(uint32_t u32MemUnit)
    __attribute__((alias("gj_sysinfo_mem_unit_ok_u_7649")));
