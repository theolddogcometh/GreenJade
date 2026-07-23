/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6281: ACPI DSDT signature constant (_6281).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_sig_dsdt_6281(void);
 *     - Return the ACPI Differentiated System Description Table signature
 *       "DSDT" packed as a little-endian uint32_t (0x54445344).
 *   uint32_t __gj_acpi_sig_dsdt_6281  (alias)
 *   __libcgj_batch6281_marker = "libcgj-batch6281"
 *
 * Exclusive continuum CREATE-ONLY (6281-6290: ACPI/table signature
 * stubs — acpi_sig_dsdt_6281, acpi_sig_ssdt_6282, acpi_sig_facs_6283, acpi_sig_hpet_6284, acpi_sig_srat_6285, acpi_sig_slit_6286, acpi_sig_bgrt_6287, acpi_sig_bert_6288, acpi_sig_hest_6289, batch_id_6290). Unique
 * gj_acpi_sig_dsdt_6281 surface only; no multi-def. Distinct from
 * prior gj_acpi_sig_*_u (batch5171-5175). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6281_marker[] = "libcgj-batch6281";

/* "DSDT": 'D' | ('S' << 8) | ('D' << 16) | ('T' << 24). */
#define B6281_DSDT  0x54445344u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6281_sig_dsdt(void)
{
	return B6281_DSDT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_sig_dsdt_6281 - report ACPI DSDT signature ("DSDT" LE).
 *
 * Always returns 0x54445344. Soft compile-time ACPI table signature
 * constant. Does not parse a table. No parent wires.
 */
uint32_t
gj_acpi_sig_dsdt_6281(void)
{
	(void)NULL;
	return b6281_sig_dsdt();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_sig_dsdt_6281(void)
    __attribute__((alias("gj_acpi_sig_dsdt_6281")));
