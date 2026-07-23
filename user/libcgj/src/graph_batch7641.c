/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7641: sysinfo uptime field id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sysinfo_uptime_id_7641(void);
 *     - Return soft catalog field ordinal for struct sysinfo.uptime (0).
 *   uint32_t __gj_sysinfo_uptime_id_7641  (alias)
 *   __libcgj_batch7641_marker = "libcgj-batch7641"
 *
 * Exclusive continuum CREATE-ONLY (7641-7650: sysinfo field stubs —
 * uptime_id, loads_slots, totalram_id, freeram_ok, sharedram_id,
 * bufferram_id, totalswap_id, freeswap_ok, mem_unit_ok, batch_id_7650).
 * Unique gj_sysinfo_uptime_id_7641 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7641_marker[] = "libcgj-batch7641";

/* Soft catalog ordinal for sysinfo.uptime (seconds since boot). */
#define B7641_UPTIME_ID ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7641_sysinfo_uptime_id(void)
{
	return B7641_UPTIME_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sysinfo_uptime_id_7641 - soft field ordinal for sysinfo.uptime.
 *
 * Always returns 0 (first field in the soft catalog). Catalog id only;
 * does not call sysinfo(2). No parent wires.
 */
uint32_t
gj_sysinfo_uptime_id_7641(void)
{
	(void)NULL;
	return b7641_sysinfo_uptime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sysinfo_uptime_id_7641(void)
    __attribute__((alias("gj_sysinfo_uptime_id_7641")));
