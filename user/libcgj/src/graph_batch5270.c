/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5270: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5270(void);
 *     - Returns the compile-time graph batch number for this TU (5270).
 *   uint32_t __gj_batch_id_5270  (alias)
 *   __libcgj_batch5270_marker = "libcgj-batch5270"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5270_marker[] = "libcgj-batch5270";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5270_id(void)
{
	return 5270u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5270 - report this TU's graph batch number.
 *
 * Always returns 5270. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5270(void)
{
	(void)NULL;
	return b5270_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5270(void)
    __attribute__((alias("gj_batch_id_5270")));
