/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5180: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5180(void);
 *     - Returns the compile-time graph batch number for this TU (5180).
 *   uint32_t __gj_batch_id_5180  (alias)
 *   __libcgj_batch5180_marker = "libcgj-batch5180"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique —
 * acpi_sig_rsdt_u, acpi_sig_xsdt_u, acpi_sig_fadt_u, acpi_sig_madt_u,
 * acpi_sig_mcfg_u, acpi_is_ascii_sig_u, acpi_checksum_ok_u,
 * acpi_hdr_len_u, acpi_rev_u, batch_id_5180). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5180_marker[] = "libcgj-batch5180";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5180_id(void)
{
	return 5180u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5180 - report this TU's graph batch number.
 *
 * Always returns 5180. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5180(void)
{
	(void)NULL;
	return b5180_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5180(void)
    __attribute__((alias("gj_batch_id_5180")));
