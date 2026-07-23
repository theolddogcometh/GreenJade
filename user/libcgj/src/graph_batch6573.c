/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6573: SMBIOS structure handle soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_handle_ok_6573(uint32_t handle);
 *     - Soft handle check: 1 when handle is in [0, 0xfffe]; reserved
 *       all-bits-one (0xffff) and values above 16-bit width → 0.
 *   uint32_t __gj_smbios_handle_ok_6573  (alias)
 *   __libcgj_batch6573_marker = "libcgj-batch6573"
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

const char __libcgj_batch6573_marker[] = "libcgj-batch6573";

#define B6573_HANDLE_MAX  0xfffeu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6573_handle_ok(uint32_t u32Handle)
{
	if (u32Handle > B6573_HANDLE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_handle_ok_6573 - soft SMBIOS structure handle validity.
 *
 * handle: 16-bit structure Handle field (low 16 bits meaningful)
 *
 * Returns 1 when handle is in [0, 0xfffe], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_smbios_handle_ok_6573(uint32_t handle)
{
	(void)NULL;
	return b6573_handle_ok(handle);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_handle_ok_6573(uint32_t handle)
    __attribute__((alias("gj_smbios_handle_ok_6573")));
