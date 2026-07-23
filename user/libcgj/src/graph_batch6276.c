/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6276: UEFI variable name UCS-2 → byte size.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_var_name_bytes_6276(uint32_t name_ucs2_units);
 *     - Soft byte length of a UCS-2 variable name including the
 *       terminating NUL: (units + 1) * 2. Clamps units to the soft
 *       max 1024 before scaling. Overflow-safe within 32-bit.
 *   uint32_t __gj_efi_var_name_bytes_6276  (alias)
 *   __libcgj_batch6276_marker = "libcgj-batch6276"
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

const char __libcgj_batch6276_marker[] = "libcgj-batch6276";

/* Soft max UCS-2 units (excludes NUL); matches name_ucs2_max_6272. */
#define B6276_NAME_UCS2_MAX  1024u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6276_clamp_units(uint32_t u32Units)
{
	if (u32Units > B6276_NAME_UCS2_MAX) {
		return B6276_NAME_UCS2_MAX;
	}
	return u32Units;
}

static uint32_t
b6276_name_bytes(uint32_t u32Units)
{
	/* (units + 1) * 2  — include UCS-2 NUL terminator. */
	return (b6276_clamp_units(u32Units) + 1u) * 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_var_name_bytes_6276 - soft UCS-2 name size in bytes (with NUL).
 *
 * name_ucs2_units: code units excluding terminator; clamped to 1024.
 * Returns (clamped_units + 1) * 2. Soft pure-data only. No parent wires.
 */
uint32_t
gj_efi_var_name_bytes_6276(uint32_t name_ucs2_units)
{
	(void)NULL;
	return b6276_name_bytes(name_ucs2_units);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_var_name_bytes_6276(uint32_t name_ucs2_units)
    __attribute__((alias("gj_efi_var_name_bytes_6276")));
