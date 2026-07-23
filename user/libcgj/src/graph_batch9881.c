/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9881: xHCI capability soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xhci_cap_ok_u_9881(void);
 *     - Return 0 (soft lamp: xHCI capability regs not claimed present
 *       for this freestanding continuum). Catalog only; not MMIO probe.
 *   uint32_t __gj_xhci_cap_ok_u_9881  (alias)
 *   __libcgj_batch9881_marker = "libcgj-batch9881"
 *
 * Exclusive continuum CREATE-ONLY (9881-9890: xhci soft id stubs —
 * xhci_cap_ok_u, xhci_op_ok_u, xhci_rt_ok_u, xhci_db_ok_u, xhci_slot_ok_u,
 * xhci_ep_ok_u, xhci_trb_ok_u, xhci_cmd_ok_u, xhci_ready_u, batch_id_9890).
 * Unique gj_xhci_cap_ok_u_9881 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9881_marker[] = "libcgj-batch9881";

/* Soft xHCI capability lamp (not claimed / soft-off). */
#define B9881_XHCI_CAP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9881_cap_ok(void)
{
	return B9881_XHCI_CAP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xhci_cap_ok_u_9881 - soft xHCI capability-register lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not read CAPLENGTH
 * or HCSPARAMS. No parent wires.
 */
uint32_t
gj_xhci_cap_ok_u_9881(void)
{
	(void)NULL;
	return b9881_cap_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xhci_cap_ok_u_9881(void)
    __attribute__((alias("gj_xhci_cap_ok_u_9881")));
