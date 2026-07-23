/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5264: AHCI PRDT entry size in bytes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_prdt_entry_u(void);
 *     - Returns the size in bytes of one Physical Region Descriptor
 *       Table entry in an AHCI command table (always 16).
 *   uint32_t __gj_ahci_prdt_entry_u  (alias)
 *   __libcgj_batch5264_marker = "libcgj-batch5264"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Unique gj_ahci_prdt_entry_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5264_marker[] = "libcgj-batch5264";

/* Size in bytes of one AHCI PRDT entry (DBA/DBAU/reserved/DBC). */
#define B5264_PRDT_ENTRY  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5264_prdt_entry(void)
{
	return B5264_PRDT_ENTRY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_prdt_entry_u - AHCI PRDT entry size in bytes.
 *
 * Always returns 16 (one physical region descriptor). Does not call
 * libc. No parent wires.
 */
uint32_t
gj_ahci_prdt_entry_u(void)
{
	(void)NULL;
	return b5264_prdt_entry();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_prdt_entry_u(void)
    __attribute__((alias("gj_ahci_prdt_entry_u")));
