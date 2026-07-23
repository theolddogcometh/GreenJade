/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5261: AHCI PxSIG SATA / ATA device signature.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_sig_sata_u(void);
 *     - Returns the AHCI port signature value for a SATA ATA disk
 *       (0x00000101 — serial ATA device class).
 *   uint32_t __gj_ahci_sig_sata_u  (alias)
 *   __libcgj_batch5261_marker = "libcgj-batch5261"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Unique gj_ahci_sig_sata_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5261_marker[] = "libcgj-batch5261";

/* AHCI PxSIG value for a SATA ATA hard disk. */
#define B5261_SIG_SATA  ((uint32_t)0x00000101u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5261_sig_sata(void)
{
	return B5261_SIG_SATA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_sig_sata_u - AHCI port signature for a SATA ATA device.
 *
 * Always returns 0x00000101 (PxSIG ATA disk class). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_ahci_sig_sata_u(void)
{
	(void)NULL;
	return b5261_sig_sata();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_sig_sata_u(void)
    __attribute__((alias("gj_ahci_sig_sata_u")));
