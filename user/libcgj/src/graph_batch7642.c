/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7642: sysinfo loads[] slot count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sysinfo_loads_slots_7642(void);
 *     - Return soft count of load-average slots in sysinfo.loads (3).
 *   uint32_t __gj_sysinfo_loads_slots_7642  (alias)
 *   __libcgj_batch7642_marker = "libcgj-batch7642"
 *
 * Exclusive continuum CREATE-ONLY (7641-7650: sysinfo field stubs —
 * uptime_id, loads_slots, totalram_id, freeram_ok, sharedram_id,
 * bufferram_id, totalswap_id, freeswap_ok, mem_unit_ok, batch_id_7650).
 * Unique gj_sysinfo_loads_slots_7642 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7642_marker[] = "libcgj-batch7642";

/* Soft slot count for sysinfo.loads[3] (1 / 5 / 15 minute averages). */
#define B7642_LOADS_SLOTS ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7642_sysinfo_loads_slots(void)
{
	return B7642_LOADS_SLOTS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sysinfo_loads_slots_7642 - soft count of sysinfo.loads elements.
 *
 * Always returns 3 (POSIX/Linux loadavg triple). Catalog constant only;
 * does not call sysinfo(2). No parent wires.
 */
uint32_t
gj_sysinfo_loads_slots_7642(void)
{
	(void)NULL;
	return b7642_sysinfo_loads_slots();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sysinfo_loads_slots_7642(void)
    __attribute__((alias("gj_sysinfo_loads_slots_7642")));
