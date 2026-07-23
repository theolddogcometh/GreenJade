/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6577: SMBIOS structure table length soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_table_len_ok_6577(uint32_t table_len);
 *     - Soft Structure Table Length check: 1 when table_len is in
 *       [4, 0xffff] (at least one minimal structure; 16-bit field cap
 *       for SMBIOS 2.x entry points). Else 0.
 *   uint32_t __gj_smbios_table_len_ok_6577  (alias)
 *   __libcgj_batch6577_marker = "libcgj-batch6577"
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

const char __libcgj_batch6577_marker[] = "libcgj-batch6577";

#define B6577_TABLE_MIN  4u
#define B6577_TABLE_MAX  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6577_table_ok(uint32_t u32Len)
{
	if (u32Len < B6577_TABLE_MIN || u32Len > B6577_TABLE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_table_len_ok_6577 - soft SMBIOS structure table length ok.
 *
 * table_len: Structure Table Length from entry point (bytes)
 *
 * Returns 1 when length is in [4, 0xffff], else 0. Pure integer.
 * No parent wires.
 */
uint32_t
gj_smbios_table_len_ok_6577(uint32_t table_len)
{
	(void)NULL;
	return b6577_table_ok(table_len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_table_len_ok_6577(uint32_t table_len)
    __attribute__((alias("gj_smbios_table_len_ok_6577")));
