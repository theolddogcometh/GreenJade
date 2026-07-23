/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9830: acpi soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9830(void);
 *     - Returns the compile-time graph batch number for this TU (9830).
 *   uint32_t __gj_batch_id_9830  (alias)
 *   __libcgj_batch9830_marker = "libcgj-batch9830"
 *
 * Exclusive continuum CREATE-ONLY (9821-9830: acpi soft id stubs —
 * acpi_rsdp_ok_u_9821, acpi_xsdt_ok_u_9822, acpi_fadt_ok_u_9823,
 * acpi_madt_ok_u_9824, acpi_mcfg_ok_u_9825, acpi_dsdt_ok_u_9826,
 * acpi_ssdt_ok_u_9827, acpi_aml_ok_u_9828, acpi_ready_u_9829,
 * batch_id_9830).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No ACPI implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9830_marker[] = "libcgj-batch9830";

#define B9830_BATCH_ID  9830u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9830_id(void)
{
	return B9830_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9830 - report this TU's graph batch number.
 *
 * Always returns 9830. Link-time presence tags the exclusive
 * acpi soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9830(void)
{
	(void)NULL;
	return b9830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9830(void)
    __attribute__((alias("gj_batch_id_9830")));
