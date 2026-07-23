/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5266: test AHCI port signature for ATAPI.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_is_atapi_u(uint32_t sig);
 *     - Return 1 if sig equals the SATA ATAPI PxSIG (0xEB140101), else 0.
 *   uint32_t __gj_ahci_is_atapi_u  (alias)
 *   __libcgj_batch5266_marker = "libcgj-batch5266"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Unique gj_ahci_is_atapi_u surface only; no multi-def. Complements
 * gj_ahci_sig_atapi_u (batch5262). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5266_marker[] = "libcgj-batch5266";

/* AHCI PxSIG value for a SATA ATAPI (packet) device. */
#define B5266_SIG_ATAPI  ((uint32_t)0xEB140101u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5266_is_atapi(uint32_t u32Sig)
{
	if (u32Sig == B5266_SIG_ATAPI) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_is_atapi_u - 1 if sig is the SATA ATAPI AHCI port signature.
 *
 * sig: raw PxSIG register value
 *
 * Returns 1 when sig == 0xEB140101, else 0. Self-contained; no parent
 * wires (does not call gj_ahci_sig_atapi_u).
 */
uint32_t
gj_ahci_is_atapi_u(uint32_t u32Sig)
{
	(void)NULL;
	return b5266_is_atapi(u32Sig);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_is_atapi_u(uint32_t u32Sig)
    __attribute__((alias("gj_ahci_is_atapi_u")));
