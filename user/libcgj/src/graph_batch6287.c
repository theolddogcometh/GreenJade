/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6287: ACPI BGRT signature constant (_6287).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_sig_bgrt_6287(void);
 *     - Return the ACPI Boot Graphics Resource Table signature
 *       "BGRT" packed as a little-endian uint32_t (0x54524742).
 *   uint32_t __gj_acpi_sig_bgrt_6287  (alias)
 *   __libcgj_batch6287_marker = "libcgj-batch6287"
 *
 * Exclusive continuum CREATE-ONLY (6281-6290: ACPI/table signature
 * stubs — acpi_sig_dsdt_6281, acpi_sig_ssdt_6282, acpi_sig_facs_6283, acpi_sig_hpet_6284, acpi_sig_srat_6285, acpi_sig_slit_6286, acpi_sig_bgrt_6287, acpi_sig_bert_6288, acpi_sig_hest_6289, batch_id_6290). Unique
 * gj_acpi_sig_bgrt_6287 surface only; no multi-def. Distinct from
 * prior gj_acpi_sig_*_u (batch5171-5175). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6287_marker[] = "libcgj-batch6287";

/* "BGRT": 'B' | ('G' << 8) | ('R' << 16) | ('T' << 24). */
#define B6287_BGRT  0x54524742u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6287_sig_bgrt(void)
{
	return B6287_BGRT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_sig_bgrt_6287 - report ACPI BGRT signature ("BGRT" LE).
 *
 * Always returns 0x54524742. Soft compile-time ACPI table signature
 * constant. Does not parse a table. No parent wires.
 */
uint32_t
gj_acpi_sig_bgrt_6287(void)
{
	(void)NULL;
	return b6287_sig_bgrt();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_sig_bgrt_6287(void)
    __attribute__((alias("gj_acpi_sig_bgrt_6287")));
