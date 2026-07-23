/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9837: soft SMBIOS memory device ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_mem_ok_u_9837(void);
 *     - Returns 1 (SMBIOS Memory Device Type 17 soft-id continuum ok).
 *       Pure-data product tag; does not parse memory structures.
 *   uint32_t __gj_smbios_mem_ok_u_9837  (alias)
 *   __libcgj_batch9837_marker = "libcgj-batch9837"
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

const char __libcgj_batch9837_marker[] = "libcgj-batch9837";

#define B9837_MEM_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9837_mem_ok(void)
{
	return B9837_MEM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_mem_ok_u_9837 - SMBIOS Memory Device soft-id continuum ok.
 *
 * Always returns 1. Soft pure-data product tag; does not parse Type 17
 * or call libc. No parent wires.
 */
uint32_t
gj_smbios_mem_ok_u_9837(void)
{
	(void)NULL;
	return b9837_mem_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_mem_ok_u_9837(void)
    __attribute__((alias("gj_smbios_mem_ok_u_9837")));
