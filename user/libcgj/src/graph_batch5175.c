/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5175: ACPI MCFG signature constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_sig_mcfg_u(void);
 *     - Return the ACPI PCI Express Memory Mapped Configuration
 *       Table signature "MCFG" packed as a little-endian uint32_t
 *       (0x4746434D).
 *   uint32_t __gj_acpi_sig_mcfg_u  (alias)
 *   __libcgj_batch5175_marker = "libcgj-batch5175"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique).
 * Unique gj_acpi_sig_mcfg_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5175_marker[] = "libcgj-batch5175";

/* "MCFG": 'M' | ('C' << 8) | ('F' << 16) | ('G' << 24). */
#define B5175_MCFG  0x4746434Du

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5175_sig_mcfg(void)
{
	return B5175_MCFG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_sig_mcfg_u - report ACPI MCFG signature ("MCFG" LE).
 *
 * Always returns 0x4746434D. Soft compile-time ACPI table signature
 * constant. Does not parse a table. No parent wires.
 */
uint32_t
gj_acpi_sig_mcfg_u(void)
{
	(void)NULL;
	return b5175_sig_mcfg();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_sig_mcfg_u(void)
    __attribute__((alias("gj_acpi_sig_mcfg_u")));
