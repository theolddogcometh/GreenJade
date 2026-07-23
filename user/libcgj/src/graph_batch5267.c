/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5267: AHCI command-slot single-bit mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_slot_mask_u(uint32_t slot);
 *     - Return (1u << slot) for slot in 0..31; return 0 when slot >= 32
 *       (out of the AHCI 32-slot command issue mask width).
 *   uint32_t __gj_ahci_slot_mask_u  (alias)
 *   __libcgj_batch5267_marker = "libcgj-batch5267"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Unique gj_ahci_slot_mask_u surface only; no multi-def. Distinct from
 * gj_irq_mask_bit_u (batch5106). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5267_marker[] = "libcgj-batch5267";

/* AHCI command-list slot count / CI bit-mask width. */
#define B5267_SLOT_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5267_slot_mask(uint32_t u32Slot)
{
	if (u32Slot >= B5267_SLOT_BITS) {
		return 0u;
	}
	return 1u << u32Slot;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_slot_mask_u - single-bit AHCI command-slot issue mask.
 *
 * slot: command list slot index (0..31)
 *
 * Returns 1u << slot for slot 0..31, else 0. Safe against shift-width
 * UB. No parent wires.
 */
uint32_t
gj_ahci_slot_mask_u(uint32_t u32Slot)
{
	(void)NULL;
	return b5267_slot_mask(u32Slot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_slot_mask_u(uint32_t u32Slot)
    __attribute__((alias("gj_ahci_slot_mask_u")));
