/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6272: UEFI variable name UCS-2 max units.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_var_name_ucs2_max_6272(void);
 *     - Returns the soft max UEFI variable name length in UCS-2 code
 *       units, excluding the terminating NUL (1024). Integer-only
 *       bound for name size gates.
 *   uint32_t __gj_efi_var_name_ucs2_max_6272  (alias)
 *   __libcgj_batch6272_marker = "libcgj-batch6272"
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

const char __libcgj_batch6272_marker[] = "libcgj-batch6272";

/* Soft practical max UCS-2 units for a variable name (excludes NUL). */
#define B6272_NAME_UCS2_MAX  1024u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6272_name_max(void)
{
	return B6272_NAME_UCS2_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_var_name_ucs2_max_6272 - soft max variable name UCS-2 units.
 *
 * Always returns 1024. Soft bound only; does not query firmware.
 * No parent wires.
 */
uint32_t
gj_efi_var_name_ucs2_max_6272(void)
{
	(void)NULL;
	return b6272_name_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_var_name_ucs2_max_6272(void)
    __attribute__((alias("gj_efi_var_name_ucs2_max_6272")));
