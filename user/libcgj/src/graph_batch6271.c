/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6271: UEFI EFI_GUID size constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_var_guid_size_6271(void);
 *     - Returns the soft on-wire / memory size of an EFI_GUID in bytes
 *       (always 16). Integer-only size bound for BIOS/UEFI variable
 *       store footprint math.
 *   uint32_t __gj_efi_var_guid_size_6271  (alias)
 *   __libcgj_batch6271_marker = "libcgj-batch6271"
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

const char __libcgj_batch6271_marker[] = "libcgj-batch6271";

/* EFI_GUID is 16 octets (Data1/2/3 + Data4[8]). */
#define B6271_GUID_SIZE  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6271_guid_size(void)
{
	return B6271_GUID_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_var_guid_size_6271 - soft EFI_GUID size in bytes.
 *
 * Always returns 16. Compile-time bound for freestanding UEFI variable
 * size math; does not touch firmware NVRAM. No parent wires.
 */
uint32_t
gj_efi_var_guid_size_6271(void)
{
	(void)NULL;
	return b6271_guid_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_var_guid_size_6271(void)
    __attribute__((alias("gj_efi_var_guid_size_6271")));
