/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9838: soft SMBIOS parse-ok unit flag (always 0).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_parse_ok_u_9838(void);
 *     - Always returns 0. Soft continuum deliberately reports that no
 *       real SMBIOS table parser is present in this freestanding wave;
 *       pure-data product tag only.
 *   uint32_t __gj_smbios_parse_ok_u_9838  (alias)
 *   __libcgj_batch9838_marker = "libcgj-batch9838"
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

const char __libcgj_batch9838_marker[] = "libcgj-batch9838";

/* Soft parse-ok deliberately off: no real table parser in this wave. */
#define B9838_PARSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9838_parse_ok(void)
{
	return B9838_PARSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_parse_ok_u_9838 - SMBIOS soft parse-ok (always 0).
 *
 * Always returns 0. Indicates freestanding soft-id stubs only; no
 * wire-level SMBIOS parser is provided. No parent wires.
 */
uint32_t
gj_smbios_parse_ok_u_9838(void)
{
	(void)NULL;
	return b9838_parse_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_parse_ok_u_9838(void)
    __attribute__((alias("gj_smbios_parse_ok_u_9838")));
