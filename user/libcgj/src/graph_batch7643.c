/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7643: sysinfo totalram field id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sysinfo_totalram_id_7643(void);
 *     - Return soft catalog field ordinal for struct sysinfo.totalram (2).
 *   uint32_t __gj_sysinfo_totalram_id_7643  (alias)
 *   __libcgj_batch7643_marker = "libcgj-batch7643"
 *
 * Exclusive continuum CREATE-ONLY (7641-7650: sysinfo field stubs —
 * uptime_id, loads_slots, totalram_id, freeram_ok, sharedram_id,
 * bufferram_id, totalswap_id, freeswap_ok, mem_unit_ok, batch_id_7650).
 * Unique gj_sysinfo_totalram_id_7643 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7643_marker[] = "libcgj-batch7643";

/* Soft catalog ordinal for sysinfo.totalram (total usable main memory). */
#define B7643_TOTALRAM_ID ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7643_sysinfo_totalram_id(void)
{
	return B7643_TOTALRAM_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sysinfo_totalram_id_7643 - soft field ordinal for sysinfo.totalram.
 *
 * Always returns 2 (soft catalog after uptime/loads). Catalog id only;
 * does not call sysinfo(2). No parent wires.
 */
uint32_t
gj_sysinfo_totalram_id_7643(void)
{
	(void)NULL;
	return b7643_sysinfo_totalram_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sysinfo_totalram_id_7643(void)
    __attribute__((alias("gj_sysinfo_totalram_id_7643")));
