/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7645: sysinfo sharedram field id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sysinfo_sharedram_id_7645(void);
 *     - Return soft catalog field ordinal for struct sysinfo.sharedram (4).
 *   uint32_t __gj_sysinfo_sharedram_id_7645  (alias)
 *   __libcgj_batch7645_marker = "libcgj-batch7645"
 *
 * Exclusive continuum CREATE-ONLY (7641-7650: sysinfo field stubs —
 * uptime_id, loads_slots, totalram_id, freeram_ok, sharedram_id,
 * bufferram_id, totalswap_id, freeswap_ok, mem_unit_ok, batch_id_7650).
 * Unique gj_sysinfo_sharedram_id_7645 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7645_marker[] = "libcgj-batch7645";

/* Soft catalog ordinal for sysinfo.sharedram (shared memory amount). */
#define B7645_SHAREDRAM_ID ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7645_sysinfo_sharedram_id(void)
{
	return B7645_SHAREDRAM_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sysinfo_sharedram_id_7645 - soft field ordinal for sysinfo.sharedram.
 *
 * Always returns 4 (soft catalog ordinal). Catalog id only; does not
 * call sysinfo(2). No parent wires.
 */
uint32_t
gj_sysinfo_sharedram_id_7645(void)
{
	(void)NULL;
	return b7645_sysinfo_sharedram_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sysinfo_sharedram_id_7645(void)
    __attribute__((alias("gj_sysinfo_sharedram_id_7645")));
