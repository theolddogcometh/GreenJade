/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9860: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9860(void);
 *     - Returns the compile-time graph batch number for this TU (9860).
 *   uint32_t __gj_batch_id_9860  (alias)
 *   __libcgj_batch9860_marker = "libcgj-batch9860"
 *
 * Exclusive continuum CREATE-ONLY (9851-9860: usb soft id stubs —
 * usb_dev_ok_u, usb_hub_ok_u, usb_ep_ok_u, usb_config_ok_u,
 * usb_iface_ok_u, usb_ctrl_ok_u, usb_bulk_ok_u, usb_iso_ok_u,
 * usb_ready_u, batch_id_9860). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9860_marker[] = "libcgj-batch9860";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9860_id(void)
{
	return 9860u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9860 - report this TU's graph batch number.
 *
 * Always returns 9860.
 */
uint32_t
gj_batch_id_9860(void)
{
	(void)NULL;
	return b9860_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9860(void)
    __attribute__((alias("gj_batch_id_9860")));
