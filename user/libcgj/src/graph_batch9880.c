/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9880: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9880(void);
 *     - Returns the compile-time graph batch number for this TU (9880).
 *   uint32_t __gj_batch_id_9880  (alias)
 *   __libcgj_batch9880_marker = "libcgj-batch9880"
 *
 * Exclusive continuum CREATE-ONLY (9871-9880: ahci soft id stubs —
 * ahci_hba_ok_u, ahci_port_ok_u, ahci_cmd_ok_u, ahci_fis_ok_u,
 * ahci_prdt_ok_u, sata_dev_ok_u, sata_identify_ok_u, sata_dma_ok_u,
 * ahci_ready_u, batch_id_9880). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Soft pure-data catalog only; no AHCI/SATA I/O. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9880_marker[] = "libcgj-batch9880";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9880_id(void)
{
	return 9880u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9880 - report this TU's graph batch number.
 *
 * Always returns 9880. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_9880(void)
{
	(void)NULL;
	return b9880_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9880(void)
    __attribute__((alias("gj_batch_id_9880")));
