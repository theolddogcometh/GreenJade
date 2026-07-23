/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6274: UEFI variable attribute soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_var_attr_ok_6274(uint32_t attrs);
 *     - Returns 1 if attrs has no bits outside the soft known mask
 *       0x7f (bits 0..6 of EFI_VARIABLE_*), else 0. Zero attrs is ok.
 *   uint32_t __gj_efi_var_attr_ok_6274  (alias)
 *   __libcgj_batch6274_marker = "libcgj-batch6274"
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

const char __libcgj_batch6274_marker[] = "libcgj-batch6274";

/* Known EFI_VARIABLE_* bits 0..6 → 0x7f (matches attr_mask_6273). */
#define B6274_ATTR_MASK  0x7fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6274_attr_ok(uint32_t u32Attrs)
{
	return ((u32Attrs & ~B6274_ATTR_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_var_attr_ok_6274 - 1 if attrs only uses known EFI_VARIABLE bits.
 *
 * attrs: candidate UEFI variable attribute word.
 * Soft bit-mask gate only; does not call SetVariable. No parent wires.
 */
uint32_t
gj_efi_var_attr_ok_6274(uint32_t attrs)
{
	(void)NULL;
	return b6274_attr_ok(attrs);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_var_attr_ok_6274(uint32_t attrs)
    __attribute__((alias("gj_efi_var_attr_ok_6274")));
