/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7644: sysinfo freeram vs totalram ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sysinfo_freeram_ok_u_7644(uint32_t freeram, uint32_t totalram);
 *     - Return 1 if freeram <= totalram (soft field consistency), else 0.
 *   uint32_t __gj_sysinfo_freeram_ok_u_7644  (alias)
 *   __libcgj_batch7644_marker = "libcgj-batch7644"
 *
 * Exclusive continuum CREATE-ONLY (7641-7650: sysinfo field stubs —
 * uptime_id, loads_slots, totalram_id, freeram_ok, sharedram_id,
 * bufferram_id, totalswap_id, freeswap_ok, mem_unit_ok, batch_id_7650).
 * Unique gj_sysinfo_freeram_ok_u_7644 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7644_marker[] = "libcgj-batch7644";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7644_sysinfo_freeram_ok(uint32_t u32Freeram, uint32_t u32Totalram)
{
	return (u32Freeram <= u32Totalram) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sysinfo_freeram_ok_u_7644 - 1 if freeram is soft-consistent with totalram.
 *
 * freeram:  soft sysinfo.freeram catalog value (mem_unit units)
 * totalram: soft sysinfo.totalram catalog value (mem_unit units)
 *
 * Returns 1 when freeram <= totalram, else 0. Soft field check; does
 * not call sysinfo(2). No parent wires.
 */
uint32_t
gj_sysinfo_freeram_ok_u_7644(uint32_t u32Freeram, uint32_t u32Totalram)
{
	(void)NULL;
	return b7644_sysinfo_freeram_ok(u32Freeram, u32Totalram);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sysinfo_freeram_ok_u_7644(uint32_t u32Freeram, uint32_t u32Totalram)
    __attribute__((alias("gj_sysinfo_freeram_ok_u_7644")));
