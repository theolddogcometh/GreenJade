/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6273: UEFI variable attribute known-bit mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_var_attr_mask_6273(void);
 *     - Returns the soft OR of standard EFI_VARIABLE_* attribute bits
 *       (0x7f): NON_VOLATILE | BOOTSERVICE_ACCESS | RUNTIME_ACCESS |
 *       HARDWARE_ERROR_RECORD | AUTHENTICATED_WRITE_ACCESS |
 *       TIME_BASED_AUTHENTICATED_WRITE_ACCESS | APPEND_WRITE.
 *   uint32_t __gj_efi_var_attr_mask_6273  (alias)
 *   __libcgj_batch6273_marker = "libcgj-batch6273"
 *
 * Exclusive continuum CREATE-ONLY (6271-6280: BIOS/UEFI variable size
 * stubs — efi_var_guid_size_6271, efi_var_name_ucs2_max_6272,
 * efi_var_attr_mask_6273, efi_var_attr_ok_6274, efi_var_data_size_ok_6275,
 * efi_var_name_bytes_6276, efi_var_header_size_6277,
 * efi_var_store_bytes_6278, efi_var_size_pack_6279,
 * batch_id / wave_ready_6280). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Soft attribute bit stubs (not firmware calls):
 *   0x01 NON_VOLATILE
 *   0x02 BOOTSERVICE_ACCESS
 *   0x04 RUNTIME_ACCESS
 *   0x08 HARDWARE_ERROR_RECORD
 *   0x10 AUTHENTICATED_WRITE_ACCESS
 *   0x20 TIME_BASED_AUTHENTICATED_WRITE_ACCESS
 *   0x40 APPEND_WRITE
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6273_marker[] = "libcgj-batch6273";

/* Known EFI_VARIABLE_* bits 0..6 → 0x7f. */
#define B6273_ATTR_MASK  0x7fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6273_attr_mask(void)
{
	return B6273_ATTR_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_var_attr_mask_6273 - soft known-attribute bit mask.
 *
 * Always returns 0x7f. Used by attr_ok gates in the same continuum.
 * Soft pure-data only. No parent wires.
 */
uint32_t
gj_efi_var_attr_mask_6273(void)
{
	(void)NULL;
	return b6273_attr_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_var_attr_mask_6273(void)
    __attribute__((alias("gj_efi_var_attr_mask_6273")));
