/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9871: AHCI HBA ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_hba_ok_u_9871(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       AHCI HBA / ABAR readiness probe.
 *   uint32_t __gj_ahci_hba_ok_u_9871  (alias)
 *   __libcgj_batch9871_marker = "libcgj-batch9871"
 *
 * Exclusive continuum CREATE-ONLY (9871-9880: ahci soft id stubs —
 * ahci_hba_ok_u, ahci_port_ok_u, ahci_cmd_ok_u, ahci_fis_ok_u,
 * ahci_prdt_ok_u, sata_dev_ok_u, sata_identify_ok_u, sata_dma_ok_u,
 * ahci_ready_u, batch_id_9880). Unique gj_ahci_hba_ok_u_9871 surface
 * only; no multi-def. No parent wires. No __int128. No AHCI/SATA I/O.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9871_marker[] = "libcgj-batch9871";

/* Soft AHCI HBA ok lamp: always off (not a real HBA probe). */
#define B9871_AHCI_HBA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9871_ahci_hba_ok(void)
{
	return B9871_AHCI_HBA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_hba_ok_u_9871 - AHCI HBA ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not map ABAR or probe
 * HBA registers. No parent wires.
 */
uint32_t
gj_ahci_hba_ok_u_9871(void)
{
	(void)NULL;
	return b9871_ahci_hba_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_hba_ok_u_9871(void)
    __attribute__((alias("gj_ahci_hba_ok_u_9871")));
