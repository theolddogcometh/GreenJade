/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5269: AHCI command-list base alignment.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_clb_align_u(void);
 *     - Returns the required byte alignment for the AHCI command-list
 *       base (PxCLB / PxCLBU): always 1024 (1 KiB).
 *   uint32_t __gj_ahci_clb_align_u  (alias)
 *   __libcgj_batch5269_marker = "libcgj-batch5269"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Unique gj_ahci_clb_align_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5269_marker[] = "libcgj-batch5269";

/* AHCI command-list base (CLB) required alignment in bytes. */
#define B5269_CLB_ALIGN  1024u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5269_clb_align(void)
{
	return B5269_CLB_ALIGN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_clb_align_u - required AHCI command-list base alignment.
 *
 * Always returns 1024 (1 KiB CLB alignment). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_ahci_clb_align_u(void)
{
	(void)NULL;
	return b5269_clb_align();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_clb_align_u(void)
    __attribute__((alias("gj_ahci_clb_align_u")));
