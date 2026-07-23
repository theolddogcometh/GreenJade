/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6572: SMBIOS structure header length soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_hdr_len_ok_6572(uint32_t length);
 *     - Soft formatted-section length check: 1 when length is in
 *       [4, 255] (header is type+length+handle = 4 bytes minimum);
 *       else 0. Pure soft gate, not a wire parser.
 *   uint32_t __gj_smbios_hdr_len_ok_6572  (alias)
 *   __libcgj_batch6572_marker = "libcgj-batch6572"
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

const char __libcgj_batch6572_marker[] = "libcgj-batch6572";

#define B6572_LEN_MIN  4u
#define B6572_LEN_MAX  255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6572_len_ok(uint32_t u32Len)
{
	if (u32Len < B6572_LEN_MIN || u32Len > B6572_LEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_hdr_len_ok_6572 - soft SMBIOS formatted-section length ok.
 *
 * length: structure Length field (bytes of formatted area)
 *
 * Returns 1 when length is in [4, 255], else 0. Pure integer.
 * No parent wires.
 */
uint32_t
gj_smbios_hdr_len_ok_6572(uint32_t length)
{
	(void)NULL;
	return b6572_len_ok(length);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_hdr_len_ok_6572(uint32_t length)
    __attribute__((alias("gj_smbios_hdr_len_ok_6572")));
