/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6576: SMBIOS entry-point version soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_smbios_entry_ver_ok_6576(uint32_t major, uint32_t minor);
 *     - Soft entry-point version gate: major in [2, 3], minor <= 99 → 1;
 *       else 0. Covers common SMBIOS 2.x / 3.x entry stubs only.
 *   uint32_t __gj_smbios_entry_ver_ok_6576  (alias)
 *   __libcgj_batch6576_marker = "libcgj-batch6576"
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

const char __libcgj_batch6576_marker[] = "libcgj-batch6576";

#define B6576_MAJOR_MIN  2u
#define B6576_MAJOR_MAX  3u
#define B6576_MINOR_MAX  99u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6576_ver_ok(uint32_t u32Major, uint32_t u32Minor)
{
	if (u32Major < B6576_MAJOR_MIN || u32Major > B6576_MAJOR_MAX) {
		return 0u;
	}
	if (u32Minor > B6576_MINOR_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smbios_entry_ver_ok_6576 - soft SMBIOS entry-point version ok.
 *
 * major: SMBIOS major version (e.g. 2 or 3)
 * minor: SMBIOS minor version
 *
 * Returns 1 when major in [2, 3] and minor <= 99, else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_smbios_entry_ver_ok_6576(uint32_t major, uint32_t minor)
{
	(void)NULL;
	return b6576_ver_ok(major, minor);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smbios_entry_ver_ok_6576(uint32_t major, uint32_t minor)
    __attribute__((alias("gj_smbios_entry_ver_ok_6576")));
