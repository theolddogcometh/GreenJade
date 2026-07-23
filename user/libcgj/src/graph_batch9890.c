/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9890: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9890(void);
 *     - Returns the compile-time graph batch number for this TU (9890).
 *   uint32_t __gj_batch_id_9890  (alias)
 *   __libcgj_batch9890_marker = "libcgj-batch9890"
 *
 * Exclusive continuum CREATE-ONLY (9881-9890: xhci soft id stubs —
 * xhci_cap_ok_u, xhci_op_ok_u, xhci_rt_ok_u, xhci_db_ok_u, xhci_slot_ok_u,
 * xhci_ep_ok_u, xhci_trb_ok_u, xhci_cmd_ok_u, xhci_ready_u, batch_id_9890).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. Soft pure-data catalog only; no xHCI I/O.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9890_marker[] = "libcgj-batch9890";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9890_id(void)
{
	return 9890u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9890 - report this TU's graph batch number.
 *
 * Always returns 9890. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_9890(void)
{
	(void)NULL;
	return b9890_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9890(void)
    __attribute__((alias("gj_batch_id_9890")));
