/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5171: ACPI RSDT signature constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_sig_rsdt_u(void);
 *     - Return the ACPI Root System Description Table signature
 *       "RSDT" packed as a little-endian uint32_t (0x54445352).
 *   uint32_t __gj_acpi_sig_rsdt_u  (alias)
 *   __libcgj_batch5171_marker = "libcgj-batch5171"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique —
 * acpi_sig_rsdt_u, acpi_sig_xsdt_u, acpi_sig_fadt_u, acpi_sig_madt_u,
 * acpi_sig_mcfg_u, acpi_is_ascii_sig_u, acpi_checksum_ok_u,
 * acpi_hdr_len_u, acpi_rev_u, batch_id_5180). Unique
 * gj_acpi_sig_rsdt_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5171_marker[] = "libcgj-batch5171";

/* "RSDT": 'R' | ('S' << 8) | ('D' << 16) | ('T' << 24). */
#define B5171_RSDT  0x54445352u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5171_sig_rsdt(void)
{
	return B5171_RSDT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_sig_rsdt_u - report ACPI RSDT signature ("RSDT" LE).
 *
 * Always returns 0x54445352. Soft compile-time ACPI table signature
 * constant. Does not parse a table. No parent wires.
 */
uint32_t
gj_acpi_sig_rsdt_u(void)
{
	(void)NULL;
	return b5171_sig_rsdt();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_sig_rsdt_u(void)
    __attribute__((alias("gj_acpi_sig_rsdt_u")));
