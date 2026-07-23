/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5179: ACPI SDT header Revision field (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_rev_u(const uint8_t *p);
 *     - Read the Revision byte at offset +8 of an ACPI system
 *       description table header, widened to uint32_t. NULL → 0.
 *       When non-NULL, caller must ensure p addresses at least 9
 *       readable octets.
 *   uint32_t __gj_acpi_rev_u  (alias)
 *   __libcgj_batch5179_marker = "libcgj-batch5179"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique).
 * Unique gj_acpi_rev_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5179_marker[] = "libcgj-batch5179";

/* ---- freestanding helpers ---------------------------------------------- */

/* ACPI SDT header: Revision is a single byte at offset 8. p >= 9. */
static uint32_t
b5179_rev(const uint8_t *p)
{
	return (uint32_t)p[8];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_rev_u - read ACPI SDT header Revision (byte at +8).
 *
 * p: table header start (NULL → 0). When non-NULL, must address >= 9
 *    readable bytes (through Revision).
 *
 * Soft field extract only; does not validate signature or checksum.
 * No parent wires.
 */
uint32_t
gj_acpi_rev_u(const uint8_t *p)
{
	(void)NULL;
	if (p == NULL) {
		return 0u;
	}
	return b5179_rev(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_rev_u(const uint8_t *p)
    __attribute__((alias("gj_acpi_rev_u")));
