/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5263: AHCI command FIS length in DWORDs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_cmd_fis_len_u(void);
 *     - Returns the H2D Register FIS length in DWORDs used for a
 *       standard AHCI command header CFL field (always 5).
 *   uint32_t __gj_ahci_cmd_fis_len_u  (alias)
 *   __libcgj_batch5263_marker = "libcgj-batch5263"
 *
 * Exclusive continuum CREATE-ONLY (5261-5270: ahci_sig_sata_u,
 * ahci_sig_atapi_u, ahci_cmd_fis_len_u, ahci_prdt_entry_u,
 * ahci_is_sata_u, ahci_is_atapi_u, ahci_slot_mask_u,
 * ahci_port_offset_u, ahci_clb_align_u, batch_id_5270).
 * Unique gj_ahci_cmd_fis_len_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5263_marker[] = "libcgj-batch5263";

/* H2D Register FIS length in 32-bit DWORDs (CFL = 5). */
#define B5263_CMD_FIS_LEN  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5263_cmd_fis_len(void)
{
	return B5263_CMD_FIS_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_cmd_fis_len_u - AHCI command FIS length in DWORDs.
 *
 * Always returns 5 (standard Host-to-Device Register FIS CFL). Does not
 * call libc. No parent wires.
 */
uint32_t
gj_ahci_cmd_fis_len_u(void)
{
	(void)NULL;
	return b5263_cmd_fis_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_cmd_fis_len_u(void)
    __attribute__((alias("gj_ahci_cmd_fis_len_u")));
