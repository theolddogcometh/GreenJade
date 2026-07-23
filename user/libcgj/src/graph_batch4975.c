/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4975: PE/DOS MZ magic sniff (uint8_t *).
 *
 * Surface (unique symbols):
 *   unsigned gj_pe_is_dos_u(const uint8_t *p);
 *     - Return 1u if p is non-NULL and the first two octets are
 *       'M' 'Z' (IMAGE_DOS_SIGNATURE little-endian bytes). Else 0u.
 *       Caller must ensure p points at least two readable octets
 *       when non-NULL (no length argument on this surface).
 *   unsigned __gj_pe_is_dos_u  (alias)
 *   __libcgj_batch4975_marker = "libcgj-batch4975"
 *
 * Exclusive continuum CREATE-ONLY PE unique wave (4971-4980). Distinct
 * from gj_pe_is_mz / gj_pe_is_mz_u (void*,size_t) — unique
 * gj_pe_is_dos_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4975_marker[] = "libcgj-batch4975";

/* ---- freestanding helpers ---------------------------------------------- */

/* IMAGE_DOS_HEADER.e_magic == "MZ". p non-NULL with >= 2 octets. */
static unsigned
b4975_is_dos(const uint8_t *p)
{
	if (p[0] != (uint8_t)'M' || p[1] != (uint8_t)'Z') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_is_dos_u - 1u if first two octets are 'M' 'Z'.
 *
 * p: image start (NULL → 0u). When non-NULL, must address >= 2 bytes.
 *
 * Distinct from gj_pe_is_mz_u (takes length). No parent wires.
 */
unsigned
gj_pe_is_dos_u(const uint8_t *p)
{
	(void)NULL;
	if (p == NULL) {
		return 0u;
	}
	return b4975_is_dos(p);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_pe_is_dos_u(const uint8_t *p)
    __attribute__((alias("gj_pe_is_dos_u")));
