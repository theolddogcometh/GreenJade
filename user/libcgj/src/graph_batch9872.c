/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9872: AHCI port ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_port_ok_u_9872(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       AHCI port / PxCMD readiness probe.
 *   uint32_t __gj_ahci_port_ok_u_9872  (alias)
 *   __libcgj_batch9872_marker = "libcgj-batch9872"
 *
 * Exclusive continuum CREATE-ONLY (9871-9880: ahci soft id stubs —
 * ahci_hba_ok_u, ahci_port_ok_u, ahci_cmd_ok_u, ahci_fis_ok_u,
 * ahci_prdt_ok_u, sata_dev_ok_u, sata_identify_ok_u, sata_dma_ok_u,
 * ahci_ready_u, batch_id_9880). Unique gj_ahci_port_ok_u_9872 surface
 * only; no multi-def. No parent wires. No __int128. No AHCI/SATA I/O.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9872_marker[] = "libcgj-batch9872";

/* Soft AHCI port ok lamp: always off (not a real port probe). */
#define B9872_AHCI_PORT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9872_ahci_port_ok(void)
{
	return B9872_AHCI_PORT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_port_ok_u_9872 - AHCI port ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not read PxSSTS/PxCMD
 * or claim a live port. No parent wires.
 */
uint32_t
gj_ahci_port_ok_u_9872(void)
{
	(void)NULL;
	return b9872_ahci_port_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_port_ok_u_9872(void)
    __attribute__((alias("gj_ahci_port_ok_u_9872")));
