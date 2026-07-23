/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9827: soft ACPI SSDT ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_ssdt_ok_u_9827(void);
 *     - Returns 1 (SSDT soft-id continuum ok). Pure-data product tag;
 *       does not load SSDT bytecode or evaluate methods.
 *   uint32_t __gj_acpi_ssdt_ok_u_9827  (alias)
 *   __libcgj_batch9827_marker = "libcgj-batch9827"
 *
 * Exclusive continuum CREATE-ONLY (9821-9830: acpi soft id stubs —
 * acpi_rsdp_ok_u_9821, acpi_xsdt_ok_u_9822, acpi_fadt_ok_u_9823,
 * acpi_madt_ok_u_9824, acpi_mcfg_ok_u_9825, acpi_dsdt_ok_u_9826,
 * acpi_ssdt_ok_u_9827, acpi_aml_ok_u_9828, acpi_ready_u_9829,
 * batch_id_9830).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No ACPI implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9827_marker[] = "libcgj-batch9827";

#define B9827_ACPI_SSDT_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9827_ssdt_ok(void)
{
	return B9827_ACPI_SSDT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_ssdt_ok_u_9827 - ACPI SSDT soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not parse SSDT.
 * No parent wires.
 */
uint32_t
gj_acpi_ssdt_ok_u_9827(void)
{
	(void)NULL;
	return b9827_ssdt_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_ssdt_ok_u_9827(void)
    __attribute__((alias("gj_acpi_ssdt_ok_u_9827")));
