/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6574: SMBIOS string-index soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_str_idx_ok_6574(uint32_t str_idx, uint32_t str_count);
 *     - Soft string-number check: str_idx 0 always ok (unspecified);
 *       else 1 when 1 <= str_idx <= str_count and str_count in [1, 255].
 *   uint32_t __gj_smbios_str_idx_ok_6574  (alias)
 *   __libcgj_batch6574_marker = "libcgj-batch6574"
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

const char __libcgj_batch6574_marker[] = "libcgj-batch6574";

#define B6574_STR_MAX  255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6574_str_idx_ok(uint32_t u32Idx, uint32_t u32Count)
{
	if (u32Idx == 0u) {
		return 1u;
	}
	if (u32Count == 0u || u32Count > B6574_STR_MAX) {
		return 0u;
	}
	if (u32Idx > u32Count) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_str_idx_ok_6574 - soft SMBIOS unformatted string index ok.
 *
 * str_idx:   string number referenced by a structure field (0 = none)
 * str_count: count of strings in the trailing string set
 *
 * Returns 1 when index is unspecified (0) or in [1, str_count] with
 * a sane count, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_smbios_str_idx_ok_6574(uint32_t str_idx, uint32_t str_count)
{
	(void)NULL;
	return b6574_str_idx_ok(str_idx, str_count);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_str_idx_ok_6574(uint32_t str_idx, uint32_t str_count)
    __attribute__((alias("gj_smbios_str_idx_ok_6574")));
