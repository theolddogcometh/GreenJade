/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6279: UEFI variable size pack (name|data).
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_var_size_pack_6279(uint32_t name_ucs2_units,
 *                                     uint32_t data_size);
 *     - Pack soft size fields into one summary word:
 *         bits  0..15 = name_ucs2_units clamped to 0..0xffff
 *         bits 16..31 = data_size      clamped to 0..0xffff
 *       Compact probe tag for the BIOS/UEFI variable size continuum.
 *   uint32_t __gj_efi_var_size_pack_6279  (alias)
 *   __libcgj_batch6279_marker = "libcgj-batch6279"
 *
 * Exclusive continuum CREATE-ONLY (6271-6280: BIOS/UEFI variable size
 * stubs — efi_var_guid_size_6271, efi_var_name_ucs2_max_6272,
 * efi_var_attr_mask_6273, efi_var_attr_ok_6274, efi_var_data_size_ok_6275,
 * efi_var_name_bytes_6276, efi_var_header_size_6277,
 * efi_var_store_bytes_6278, efi_var_size_pack_6279,
 * batch_id / wave_ready_6280). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6279_marker[] = "libcgj-batch6279";

#define B6279_U16_MAX  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6279_clamp16(uint32_t u32V)
{
	if (u32V > B6279_U16_MAX) {
		return B6279_U16_MAX;
	}
	return u32V;
}

static uint32_t
b6279_pack(uint32_t u32Name, uint32_t u32Data)
{
	return b6279_clamp16(u32Name) | (b6279_clamp16(u32Data) << 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_var_size_pack_6279 - pack name UCS-2 units + data size.
 *
 * name_ucs2_units: name length in UCS-2 units (clamped to 16 bits)
 * data_size:       payload length in bytes (clamped to 16 bits)
 *
 * Returns packed summary word. Soft pure-data only. No parent wires.
 */
uint32_t
gj_efi_var_size_pack_6279(uint32_t name_ucs2_units, uint32_t data_size)
{
	(void)NULL;
	return b6279_pack(name_ucs2_units, data_size);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_var_size_pack_6279(uint32_t name_ucs2_units,
    uint32_t data_size)
    __attribute__((alias("gj_efi_var_size_pack_6279")));
