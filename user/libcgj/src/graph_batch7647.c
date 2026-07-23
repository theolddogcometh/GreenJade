/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7647: sysinfo totalswap field id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sysinfo_totalswap_id_7647(void);
 *     - Return soft catalog field ordinal for struct sysinfo.totalswap (6).
 *   uint32_t __gj_sysinfo_totalswap_id_7647  (alias)
 *   __libcgj_batch7647_marker = "libcgj-batch7647"
 *
 * Exclusive continuum CREATE-ONLY (7641-7650: sysinfo field stubs —
 * uptime_id, loads_slots, totalram_id, freeram_ok, sharedram_id,
 * bufferram_id, totalswap_id, freeswap_ok, mem_unit_ok, batch_id_7650).
 * Unique gj_sysinfo_totalswap_id_7647 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7647_marker[] = "libcgj-batch7647";

/* Soft catalog ordinal for sysinfo.totalswap (total swap space). */
#define B7647_TOTALSWAP_ID ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7647_sysinfo_totalswap_id(void)
{
	return B7647_TOTALSWAP_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sysinfo_totalswap_id_7647 - soft field ordinal for sysinfo.totalswap.
 *
 * Always returns 6 (soft catalog ordinal). Catalog id only; does not
 * call sysinfo(2). No parent wires.
 */
uint32_t
gj_sysinfo_totalswap_id_7647(void)
{
	(void)NULL;
	return b7647_sysinfo_totalswap_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sysinfo_totalswap_id_7647(void)
    __attribute__((alias("gj_sysinfo_totalswap_id_7647")));
