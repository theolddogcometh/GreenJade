/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5174: ACPI MADT signature constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_sig_madt_u(void);
 *     - Return the ACPI Multiple APIC Description Table signature
 *       "APIC" packed as a little-endian uint32_t (0x43495041).
 *       (MADT tables are identified by the four-char "APIC" sig.)
 *   uint32_t __gj_acpi_sig_madt_u  (alias)
 *   __libcgj_batch5174_marker = "libcgj-batch5174"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique).
 * Unique gj_acpi_sig_madt_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5174_marker[] = "libcgj-batch5174";

/* "APIC": 'A' | ('P' << 8) | ('I' << 16) | ('C' << 24). */
#define B5174_APIC  0x43495041u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5174_sig_madt(void)
{
	return B5174_APIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_sig_madt_u - report ACPI MADT signature ("APIC" LE).
 *
 * Always returns 0x43495041. Soft compile-time ACPI table signature
 * constant. Does not parse a table. No parent wires.
 */
uint32_t
gj_acpi_sig_madt_u(void)
{
	(void)NULL;
	return b5174_sig_madt();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_sig_madt_u(void)
    __attribute__((alias("gj_acpi_sig_madt_u")));
