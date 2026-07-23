/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6578: SMBIOS Type 0 (BIOS Information) soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_type0_bios_ok_6578(uint32_t type_id,
 *                                         uint32_t length,
 *                                         uint32_t rom_size);
 *     - Soft Type 0 gate: type_id must be 0, length >= 18 (classic
 *       BIOS Info formatted size), rom_size in [0, 255] (ROM Size
 *       field encoding unit). Returns 1 when all pass, else 0.
 *   uint32_t __gj_smbios_type0_bios_ok_6578  (alias)
 *   __libcgj_batch6578_marker = "libcgj-batch6578"
 *
 * Exclusive continuum CREATE-ONLY (6571-6580: SMBIOS structure type stubs —
 * type_id_ok_6571, hdr_len_ok_6572, handle_ok_6573, str_idx_ok_6574,
 * hdr_pack_6575, entry_ver_ok_6576, table_len_ok_6577, type0_bios_ok_6578,
 * type4_proc_pack_6579, struct_ready_6580). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6578_marker[] = "libcgj-batch6578";

#define B6578_TYPE0       0u
#define B6578_LEN_MIN     18u
#define B6578_ROM_MAX     255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6578_type0_ok(uint32_t u32Type, uint32_t u32Len, uint32_t u32Rom)
{
	if (u32Type != B6578_TYPE0) {
		return 0u;
	}
	if (u32Len < B6578_LEN_MIN) {
		return 0u;
	}
	if (u32Rom > B6578_ROM_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_type0_bios_ok_6578 - soft SMBIOS Type 0 BIOS Information ok.
 *
 * type_id:  structure Type field (must be 0)
 * length:   formatted section length (need >= 18)
 * rom_size: Type 0 ROM Size field (0..255 encoding unit)
 *
 * Returns 1 when gates pass, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_smbios_type0_bios_ok_6578(uint32_t type_id, uint32_t length,
    uint32_t rom_size)
{
	(void)NULL;
	return b6578_type0_ok(type_id, length, rom_size);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_type0_bios_ok_6578(uint32_t type_id, uint32_t length,
    uint32_t rom_size)
    __attribute__((alias("gj_smbios_type0_bios_ok_6578")));
