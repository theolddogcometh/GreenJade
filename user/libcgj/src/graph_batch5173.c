/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5173: ACPI FADT signature constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_sig_fadt_u(void);
 *     - Return the ACPI Fixed ACPI Description Table signature
 *       "FACP" packed as a little-endian uint32_t (0x50434146).
 *       (FADT tables are identified by the four-char "FACP" sig.)
 *   uint32_t __gj_acpi_sig_fadt_u  (alias)
 *   __libcgj_batch5173_marker = "libcgj-batch5173"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique).
 * Unique gj_acpi_sig_fadt_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5173_marker[] = "libcgj-batch5173";

/* "FACP": 'F' | ('A' << 8) | ('C' << 16) | ('P' << 24). */
#define B5173_FACP  0x50434146u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5173_sig_fadt(void)
{
	return B5173_FACP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_sig_fadt_u - report ACPI FADT signature ("FACP" LE).
 *
 * Always returns 0x50434146. Soft compile-time ACPI table signature
 * constant. Does not parse a table. No parent wires.
 */
uint32_t
gj_acpi_sig_fadt_u(void)
{
	(void)NULL;
	return b5173_sig_fadt();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_sig_fadt_u(void)
    __attribute__((alias("gj_acpi_sig_fadt_u")));
