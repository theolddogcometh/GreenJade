/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5265: test AHCI port signature for SATA ATA.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_is_sata_u(uint32_t sig);
 *     - Return 1 if sig equals the SATA ATA PxSIG (0x00000101), else 0.
 *   uint32_t __gj_ahci_is_sata_u  (alias)
 *   __libcgj_batch5265_marker = "libcgj-batch5265"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Unique gj_ahci_is_sata_u surface only; no multi-def. Complements
 * gj_ahci_sig_sata_u (batch5261). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5265_marker[] = "libcgj-batch5265";

/* AHCI PxSIG value for a SATA ATA hard disk. */
#define B5265_SIG_SATA  ((uint32_t)0x00000101u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5265_is_sata(uint32_t u32Sig)
{
	if (u32Sig == B5265_SIG_SATA) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_is_sata_u - 1 if sig is the SATA ATA AHCI port signature.
 *
 * sig: raw PxSIG register value
 *
 * Returns 1 when sig == 0x00000101, else 0. Self-contained; no parent
 * wires (does not call gj_ahci_sig_sata_u).
 */
uint32_t
gj_ahci_is_sata_u(uint32_t u32Sig)
{
	(void)NULL;
	return b5265_is_sata(u32Sig);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_is_sata_u(uint32_t u32Sig)
    __attribute__((alias("gj_ahci_is_sata_u")));
