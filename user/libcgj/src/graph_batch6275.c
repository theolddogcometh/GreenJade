/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6275: UEFI variable data-size soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_var_data_size_ok_6275(uint32_t data_size,
 *                                        uint32_t max_data);
 *     - Returns 1 if data_size <= max_data, else 0. Soft size gate for
 *       GetVariable/SetVariable data payloads. max_data 0 rejects all
 *       nonzero data_size (and accepts data_size 0).
 *   uint32_t __gj_efi_var_data_size_ok_6275  (alias)
 *   __libcgj_batch6275_marker = "libcgj-batch6275"
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

const char __libcgj_batch6275_marker[] = "libcgj-batch6275";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6275_data_ok(uint32_t u32Data, uint32_t u32Max)
{
	return (u32Data <= u32Max) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_var_data_size_ok_6275 - 1 if data_size is within max_data.
 *
 * data_size: candidate payload length in bytes
 * max_data:  platform / store soft maximum
 *
 * Soft pure-data compare only. No parent wires.
 */
uint32_t
gj_efi_var_data_size_ok_6275(uint32_t data_size, uint32_t max_data)
{
	(void)NULL;
	return b6275_data_ok(data_size, max_data);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_var_data_size_ok_6275(uint32_t data_size, uint32_t max_data)
    __attribute__((alias("gj_efi_var_data_size_ok_6275")));
