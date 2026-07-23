/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5262: AHCI PxSIG ATAPI device signature.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_sig_atapi_u(void);
 *     - Returns the AHCI port signature value for a SATA ATAPI device
 *       (0xEB140101 — optical / packet device class).
 *   uint32_t __gj_ahci_sig_atapi_u  (alias)
 *   __libcgj_batch5262_marker = "libcgj-batch5262"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Unique gj_ahci_sig_atapi_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5262_marker[] = "libcgj-batch5262";

/* AHCI PxSIG value for a SATA ATAPI (packet) device. */
#define B5262_SIG_ATAPI  ((uint32_t)0xEB140101u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5262_sig_atapi(void)
{
	return B5262_SIG_ATAPI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_sig_atapi_u - AHCI port signature for a SATA ATAPI device.
 *
 * Always returns 0xEB140101 (PxSIG ATAPI / packet class). Does not call
 * libc. No parent wires.
 */
uint32_t
gj_ahci_sig_atapi_u(void)
{
	(void)NULL;
	return b5262_sig_atapi();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_sig_atapi_u(void)
    __attribute__((alias("gj_ahci_sig_atapi_u")));
