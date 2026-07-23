/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5172: ACPI XSDT signature constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_sig_xsdt_u(void);
 *     - Return the ACPI Extended System Description Table signature
 *       "XSDT" packed as a little-endian uint32_t (0x54445358).
 *   uint32_t __gj_acpi_sig_xsdt_u  (alias)
 *   __libcgj_batch5172_marker = "libcgj-batch5172"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique).
 * Unique gj_acpi_sig_xsdt_u surface only; no multi-def. Distinct from
 * gj_acpi_sig_rsdt_u (batch5171). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5172_marker[] = "libcgj-batch5172";

/* "XSDT": 'X' | ('S' << 8) | ('D' << 16) | ('T' << 24). */
#define B5172_XSDT  0x54445358u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5172_sig_xsdt(void)
{
	return B5172_XSDT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_sig_xsdt_u - report ACPI XSDT signature ("XSDT" LE).
 *
 * Always returns 0x54445358. Soft compile-time ACPI table signature
 * constant. Does not parse a table. No parent wires.
 */
uint32_t
gj_acpi_sig_xsdt_u(void)
{
	(void)NULL;
	return b5172_sig_xsdt();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_sig_xsdt_u(void)
    __attribute__((alias("gj_acpi_sig_xsdt_u")));
