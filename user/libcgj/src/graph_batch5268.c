/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5268: AHCI port register block byte offset.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_port_offset_u(uint32_t port);
 *     - Return ABAR + 0x100 + port * 0x80 style relative offset for the
 *       port register block. No upper-bound clamp on port (pure scale).
 *   uint32_t __gj_ahci_port_offset_u  (alias)
 *   __libcgj_batch5268_marker = "libcgj-batch5268"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Unique gj_ahci_port_offset_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5268_marker[] = "libcgj-batch5268";

/* First port register block offset from ABAR (port 0). */
#define B5268_PORT_BASE   ((uint32_t)0x100u)
/* Stride between consecutive port register blocks. */
#define B5268_PORT_STRIDE ((uint32_t)0x80u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5268_port_offset(uint32_t u32Port)
{
	return B5268_PORT_BASE + (u32Port * B5268_PORT_STRIDE);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_port_offset_u - AHCI port N register-block byte offset.
 *
 * port: zero-based port index
 *
 * Returns 0x100 + port * 0x80 (AHCI memory-map layout relative to ABAR).
 * Pure integer scale; no clamp. No parent wires.
 */
uint32_t
gj_ahci_port_offset_u(uint32_t u32Port)
{
	(void)NULL;
	return b5268_port_offset(u32Port);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_port_offset_u(uint32_t u32Port)
    __attribute__((alias("gj_ahci_port_offset_u")));
