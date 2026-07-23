/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6571: SMBIOS structure type id soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_type_id_ok_6571(uint32_t type_id);
 *     - Soft structure-type check: 1 when type_id is in [0, 127]
 *       (DMTF-assigned range); 0 for reserved/OEM-only soft reject of
 *       values > 127 in this stub (128-255 left to OEM helpers).
 *   uint32_t __gj_smbios_type_id_ok_6571  (alias)
 *   __libcgj_batch6571_marker = "libcgj-batch6571"
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

const char __libcgj_batch6571_marker[] = "libcgj-batch6571";

#define B6571_TYPE_MAX  127u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6571_type_ok(uint32_t u32Type)
{
	if (u32Type > B6571_TYPE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_type_id_ok_6571 - soft SMBIOS structure type id validity.
 *
 * type_id: raw Type field from structure header (low 8 bits meaningful)
 *
 * Returns 1 when type is in [0, 127], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_smbios_type_id_ok_6571(uint32_t type_id)
{
	(void)NULL;
	return b6571_type_ok(type_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_type_id_ok_6571(uint32_t type_id)
    __attribute__((alias("gj_smbios_type_id_ok_6571")));
