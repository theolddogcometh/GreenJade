/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9835: soft SMBIOS Type 3 (Chassis) ok unit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_chassis_ok_u_9835(void);
 *     - Returns 1 (SMBIOS System Enclosure/Chassis Type 3 soft-id
 *       continuum ok). Pure-data product tag; does not parse Type 3.
 *   uint32_t __gj_smbios_chassis_ok_u_9835  (alias)
 *   __libcgj_batch9835_marker = "libcgj-batch9835"
 *
 * Exclusive continuum CREATE-ONLY (9831-9840: smbios soft id stubs —
 * entry_ok_u_9831, bios_ok_u_9832, system_ok_u_9833,
 * baseboard_ok_u_9834, chassis_ok_u_9835, cpu_ok_u_9836, mem_ok_u_9837,
 * parse_ok_u_9838, ready_u_9839, batch_id_9840). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9835_marker[] = "libcgj-batch9835";

#define B9835_CHASSIS_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9835_chassis_ok(void)
{
	return B9835_CHASSIS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_chassis_ok_u_9835 - SMBIOS Type 3 Chassis soft-id continuum ok.
 *
 * Always returns 1. Soft pure-data product tag; does not parse Type 3
 * or call libc. No parent wires.
 */
uint32_t
gj_smbios_chassis_ok_u_9835(void)
{
	(void)NULL;
	return b9835_chassis_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_chassis_ok_u_9835(void)
    __attribute__((alias("gj_smbios_chassis_ok_u_9835")));
