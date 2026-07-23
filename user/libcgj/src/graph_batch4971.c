/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4971: PE/DOS IMAGE_DOS_SIGNATURE constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_dos_magic_u(void);
 *     - Return the classic DOS MZ magic as a little-endian uint16
 *       widened to uint32_t: IMAGE_DOS_SIGNATURE 0x5A4D ("MZ").
 *   uint32_t __gj_pe_dos_magic_u  (alias)
 *   __libcgj_batch4971_marker = "libcgj-batch4971"
 *
 * Exclusive continuum CREATE-ONLY PE unique wave (4971-4980:
 * pe_dos_magic_u, pe_nt_sig_u, pe_machine_amd64_u, pe_machine_i386_u,
 * pe_is_dos_u, pe_opt_hdr64_magic_u, pe_opt_hdr32_magic_u,
 * pe_subdir_count_u, pe_align_u, batch_id_4980). Distinct from
 * gj_pe_is_mz / gj_pe_is_mz_u (sniff, not constant) — unique
 * gj_pe_dos_magic_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4971_marker[] = "libcgj-batch4971";

/* IMAGE_DOS_SIGNATURE: 'M' | ('Z' << 8) on LE = 0x5A4D. */
#define B4971_DOS_MAGIC  0x5A4Du

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4971_dos_magic(void)
{
	return B4971_DOS_MAGIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_dos_magic_u - report IMAGE_DOS_SIGNATURE (0x5A4D).
 *
 * Always returns 0x5A4D. Soft compile-time PE/DOS product constant.
 * Does not parse an image; see gj_pe_is_dos_u / gj_pe_is_mz_u for
 * buffer sniff. No parent wires.
 */
uint32_t
gj_pe_dos_magic_u(void)
{
	(void)NULL;
	return b4971_dos_magic();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_dos_magic_u(void)
    __attribute__((alias("gj_pe_dos_magic_u")));
