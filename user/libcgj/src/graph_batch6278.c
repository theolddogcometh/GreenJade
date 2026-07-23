/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6278: UEFI variable store footprint stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_var_store_bytes_6278(uint32_t name_ucs2_units,
 *                                       uint32_t data_size);
 *     - Soft total NVRAM store bytes for one variable:
 *         header_size(name) + data_size
 *       header = 16+4+4 + (clamp(units,1024)+1)*2.
 *       Saturates at UINT32_MAX on overflow.
 *   uint32_t __gj_efi_var_store_bytes_6278  (alias)
 *   __libcgj_batch6278_marker = "libcgj-batch6278"
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

const char __libcgj_batch6278_marker[] = "libcgj-batch6278";

#define B6278_GUID_SIZE      16u
#define B6278_ATTR_SIZE       4u
#define B6278_DATASIZE_SIZE   4u
#define B6278_NAME_UCS2_MAX  1024u
#define B6278_FIXED  (B6278_GUID_SIZE + B6278_ATTR_SIZE + B6278_DATASIZE_SIZE)
#define B6278_U32_MAX  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6278_clamp_units(uint32_t u32Units)
{
	if (u32Units > B6278_NAME_UCS2_MAX) {
		return B6278_NAME_UCS2_MAX;
	}
	return u32Units;
}

static uint32_t
b6278_header(uint32_t u32Units)
{
	return B6278_FIXED + (b6278_clamp_units(u32Units) + 1u) * 2u;
}

static uint32_t
b6278_store_bytes(uint32_t u32Units, uint32_t u32Data)
{
	uint32_t u32Hdr;

	u32Hdr = b6278_header(u32Units);
	if (u32Data > (B6278_U32_MAX - u32Hdr)) {
		return B6278_U32_MAX;
	}
	return u32Hdr + u32Data;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_var_store_bytes_6278 - soft total store bytes for one variable.
 *
 * name_ucs2_units: UCS-2 units excluding NUL; clamped to 1024
 * data_size:       payload length in bytes
 *
 * Returns header + data, saturating at UINT32_MAX. Soft pure-data only.
 * No parent wires.
 */
uint32_t
gj_efi_var_store_bytes_6278(uint32_t name_ucs2_units, uint32_t data_size)
{
	(void)NULL;
	return b6278_store_bytes(name_ucs2_units, data_size);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_var_store_bytes_6278(uint32_t name_ucs2_units,
    uint32_t data_size)
    __attribute__((alias("gj_efi_var_store_bytes_6278")));
