/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9834: soft SMBIOS Type 2 (Baseboard) ok unit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_baseboard_ok_u_9834(void);
 *     - Returns 1 (SMBIOS Baseboard Information Type 2 soft-id
 *       continuum ok). Pure-data product tag; does not parse Type 2.
 *   uint32_t __gj_smbios_baseboard_ok_u_9834  (alias)
 *   __libcgj_batch9834_marker = "libcgj-batch9834"
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

const char __libcgj_batch9834_marker[] = "libcgj-batch9834";

#define B9834_BASEBOARD_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9834_baseboard_ok(void)
{
	return B9834_BASEBOARD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_baseboard_ok_u_9834 - SMBIOS Type 2 Baseboard soft-id ok.
 *
 * Always returns 1. Soft pure-data product tag; does not parse Type 2
 * or call libc. No parent wires.
 */
uint32_t
gj_smbios_baseboard_ok_u_9834(void)
{
	(void)NULL;
	return b9834_baseboard_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_baseboard_ok_u_9834(void)
    __attribute__((alias("gj_smbios_baseboard_ok_u_9834")));
