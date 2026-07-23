/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9878: SATA DMA ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sata_dma_ok_u_9878(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       SATA DMA / NCQ transfer readiness probe.
 *   uint32_t __gj_sata_dma_ok_u_9878  (alias)
 *   __libcgj_batch9878_marker = "libcgj-batch9878"
 *
 * Exclusive continuum CREATE-ONLY (9871-9880: ahci soft id stubs —
 * ahci_hba_ok_u, ahci_port_ok_u, ahci_cmd_ok_u, ahci_fis_ok_u,
 * ahci_prdt_ok_u, sata_dev_ok_u, sata_identify_ok_u, sata_dma_ok_u,
 * ahci_ready_u, batch_id_9880). Unique gj_sata_dma_ok_u_9878 surface
 * only; no multi-def. No parent wires. No __int128. No AHCI/SATA I/O.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9878_marker[] = "libcgj-batch9878";

/* Soft SATA DMA ok lamp: always off (not a real DMA probe). */
#define B9878_SATA_DMA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9878_sata_dma_ok(void)
{
	return B9878_SATA_DMA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sata_dma_ok_u_9878 - SATA DMA ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not start DMA or NCQ
 * transfers. No parent wires.
 */
uint32_t
gj_sata_dma_ok_u_9878(void)
{
	(void)NULL;
	return b9878_sata_dma_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sata_dma_ok_u_9878(void)
    __attribute__((alias("gj_sata_dma_ok_u_9878")));
