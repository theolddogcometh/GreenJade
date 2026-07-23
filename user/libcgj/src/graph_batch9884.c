/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9884: xHCI doorbell soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xhci_db_ok_u_9884(void);
 *     - Return 0 (soft lamp: xHCI doorbell array not claimed present for
 *       this freestanding continuum). Catalog only; not MMIO probe.
 *   uint32_t __gj_xhci_db_ok_u_9884  (alias)
 *   __libcgj_batch9884_marker = "libcgj-batch9884"
 *
 * Exclusive continuum CREATE-ONLY (9881-9890: xhci soft id stubs —
 * xhci_cap_ok_u, xhci_op_ok_u, xhci_rt_ok_u, xhci_db_ok_u, xhci_slot_ok_u,
 * xhci_ep_ok_u, xhci_trb_ok_u, xhci_cmd_ok_u, xhci_ready_u, batch_id_9890).
 * Unique gj_xhci_db_ok_u_9884 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9884_marker[] = "libcgj-batch9884";

/* Soft xHCI doorbell lamp (not claimed / soft-off). */
#define B9884_XHCI_DB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9884_db_ok(void)
{
	return B9884_XHCI_DB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xhci_db_ok_u_9884 - soft xHCI doorbell-array lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not write doorbell
 * registers. No parent wires.
 */
uint32_t
gj_xhci_db_ok_u_9884(void)
{
	(void)NULL;
	return b9884_db_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xhci_db_ok_u_9884(void)
    __attribute__((alias("gj_xhci_db_ok_u_9884")));
