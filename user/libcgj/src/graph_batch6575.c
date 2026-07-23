/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6575: SMBIOS structure header pack soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_hdr_pack_6575(uint32_t type_id, uint32_t length,
 *                                    uint32_t handle);
 *     - Soft header key: (type_id & 0xff) | ((length & 0xff) << 8) |
 *       ((handle & 0xffff) << 16). Pure fold; not a wire format.
 *   uint32_t __gj_smbios_hdr_pack_6575  (alias)
 *   __libcgj_batch6575_marker = "libcgj-batch6575"
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

const char __libcgj_batch6575_marker[] = "libcgj-batch6575";

#define B6575_TYPE_MASK    0x000000ffu
#define B6575_LEN_MASK     0x000000ffu
#define B6575_HANDLE_MASK  0x0000ffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6575_hdr_pack(uint32_t u32Type, uint32_t u32Len, uint32_t u32Handle)
{
	return (u32Type & B6575_TYPE_MASK)
	    | ((u32Len & B6575_LEN_MASK) << 8)
	    | ((u32Handle & B6575_HANDLE_MASK) << 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_hdr_pack_6575 - soft SMBIOS structure header key fold.
 *
 * type_id: structure Type (low 8 bits used)
 * length:  formatted Length (low 8 bits used)
 * handle:  structure Handle (low 16 bits used)
 *
 * Returns packed soft key. Pure integer. No parent wires.
 */
uint32_t
gj_smbios_hdr_pack_6575(uint32_t type_id, uint32_t length, uint32_t handle)
{
	(void)NULL;
	return b6575_hdr_pack(type_id, length, handle);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_hdr_pack_6575(uint32_t type_id, uint32_t length,
    uint32_t handle)
    __attribute__((alias("gj_smbios_hdr_pack_6575")));
