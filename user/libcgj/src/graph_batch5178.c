/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5178: ACPI SDT header Length field (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_hdr_len_u(const uint8_t *p);
 *     - Read the little-endian uint32 Length at offset +4 of an ACPI
 *       system description table header. NULL → 0. When non-NULL,
 *       caller must ensure p addresses at least 8 readable octets.
 *   uint32_t __gj_acpi_hdr_len_u  (alias)
 *   __libcgj_batch5178_marker = "libcgj-batch5178"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique).
 * Unique gj_acpi_hdr_len_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5178_marker[] = "libcgj-batch5178";

/* ---- freestanding helpers ---------------------------------------------- */

/* ACPI SDT header: Length is LE u32 at byte offset 4. p >= 8 octets. */
static uint32_t
b5178_hdr_len(const uint8_t *p)
{
	return (uint32_t)p[4]
	    | ((uint32_t)p[5] << 8)
	    | ((uint32_t)p[6] << 16)
	    | ((uint32_t)p[7] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_hdr_len_u - read ACPI SDT header Length (LE u32 at +4).
 *
 * p: table header start (NULL → 0). When non-NULL, must address >= 8
 *    readable bytes (Signature[4] + Length[4]).
 *
 * Soft field extract only; does not validate signature or checksum.
 * No parent wires.
 */
uint32_t
gj_acpi_hdr_len_u(const uint8_t *p)
{
	(void)NULL;
	if (p == NULL) {
		return 0u;
	}
	return b5178_hdr_len(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_hdr_len_u(const uint8_t *p)
    __attribute__((alias("gj_acpi_hdr_len_u")));
