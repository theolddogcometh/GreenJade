/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6290: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6290(void);
 *     - Returns the compile-time graph batch number for this TU (6290).
 *   uint32_t __gj_batch_id_6290  (alias)
 *   __libcgj_batch6290_marker = "libcgj-batch6290"
 *
 * Exclusive continuum CREATE-ONLY (6281-6290: ACPI/table signature
 * stubs — acpi_sig_dsdt_6281, acpi_sig_ssdt_6282, acpi_sig_facs_6283, acpi_sig_hpet_6284, acpi_sig_srat_6285, acpi_sig_slit_6286, acpi_sig_bgrt_6287, acpi_sig_bert_6288, acpi_sig_hest_6289, batch_id_6290). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6290_marker[] = "libcgj-batch6290";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6290_id(void)
{
	return 6290u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6290 - report this TU's graph batch number.
 *
 * Always returns 6290. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6290(void)
{
	(void)NULL;
	return b6290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6290(void)
    __attribute__((alias("gj_batch_id_6290")));
