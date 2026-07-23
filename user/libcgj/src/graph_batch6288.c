/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6288: ACPI BERT signature constant (_6288).
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_sig_bert_6288(void);
 *     - Return the ACPI Boot Error Record Table signature
 *       "BERT" packed as a little-endian uint32_t (0x54524542).
 *   uint32_t __gj_acpi_sig_bert_6288  (alias)
 *   __libcgj_batch6288_marker = "libcgj-batch6288"
 *
 * Exclusive continuum CREATE-ONLY (6281-6290: ACPI/table signature
 * stubs — acpi_sig_dsdt_6281, acpi_sig_ssdt_6282, acpi_sig_facs_6283, acpi_sig_hpet_6284, acpi_sig_srat_6285, acpi_sig_slit_6286, acpi_sig_bgrt_6287, acpi_sig_bert_6288, acpi_sig_hest_6289, batch_id_6290). Unique
 * gj_acpi_sig_bert_6288 surface only; no multi-def. Distinct from
 * prior gj_acpi_sig_*_u (batch5171-5175). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6288_marker[] = "libcgj-batch6288";

/* "BERT": 'B' | ('E' << 8) | ('R' << 16) | ('T' << 24). */
#define B6288_BERT  0x54524542u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6288_sig_bert(void)
{
	return B6288_BERT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_sig_bert_6288 - report ACPI BERT signature ("BERT" LE).
 *
 * Always returns 0x54524542. Soft compile-time ACPI table signature
 * constant. Does not parse a table. No parent wires.
 */
uint32_t
gj_acpi_sig_bert_6288(void)
{
	(void)NULL;
	return b6288_sig_bert();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_sig_bert_6288(void)
    __attribute__((alias("gj_acpi_sig_bert_6288")));
