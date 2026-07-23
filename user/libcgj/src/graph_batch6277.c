/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6277: UEFI variable soft header size stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_var_header_size_6277(uint32_t name_ucs2_units);
 *     - Soft fixed+name header footprint for one NVRAM variable record:
 *         16 (GUID) + 4 (Attributes) + 4 (DataSize) + name_bytes
 *       where name_bytes = (clamp(units,1024) + 1) * 2.
 *   uint32_t __gj_efi_var_header_size_6277  (alias)
 *   __libcgj_batch6277_marker = "libcgj-batch6277"
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

const char __libcgj_batch6277_marker[] = "libcgj-batch6277";

#define B6277_GUID_SIZE      16u
#define B6277_ATTR_SIZE       4u
#define B6277_DATASIZE_SIZE   4u
#define B6277_NAME_UCS2_MAX  1024u

/* Fixed fields: VendorGuid + Attributes + DataSize. */
#define B6277_FIXED  (B6277_GUID_SIZE + B6277_ATTR_SIZE + B6277_DATASIZE_SIZE)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6277_clamp_units(uint32_t u32Units)
{
	if (u32Units > B6277_NAME_UCS2_MAX) {
		return B6277_NAME_UCS2_MAX;
	}
	return u32Units;
}

static uint32_t
b6277_header_size(uint32_t u32Units)
{
	uint32_t u32NameBytes;

	u32NameBytes = (b6277_clamp_units(u32Units) + 1u) * 2u;
	return B6277_FIXED + u32NameBytes;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_var_header_size_6277 - soft variable record header footprint.
 *
 * name_ucs2_units: UCS-2 units excluding NUL; clamped to 1024.
 * Returns 24 + (clamped_units + 1) * 2. Soft pure-data only.
 * No parent wires.
 */
uint32_t
gj_efi_var_header_size_6277(uint32_t name_ucs2_units)
{
	(void)NULL;
	return b6277_header_size(name_ucs2_units);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_var_header_size_6277(uint32_t name_ucs2_units)
    __attribute__((alias("gj_efi_var_header_size_6277")));
