/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5280: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5280(void);
 *     - Returns the compile-time graph batch number for this TU (5280).
 *   uint32_t __gj_batch_id_5280  (alias)
 *   __libcgj_batch5280_marker = "libcgj-batch5280"
 *
 * Exclusive continuum CREATE-ONLY (5271-5280: USB unique —
 * usb_pid_setup_u, usb_pid_in_u, usb_pid_out_u, usb_pid_data0_u,
 * usb_pid_data1_u, usb_pid_ack_u, usb_pid_nak_u, usb_pid_stall_u,
 * usb_ep0_max_u, batch_id_5280). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5280_marker[] = "libcgj-batch5280";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5280_id(void)
{
	return 5280u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5280 - report this TU's graph batch number.
 *
 * Always returns 5280. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5280(void)
{
	(void)NULL;
	return b5280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5280(void)
    __attribute__((alias("gj_batch_id_5280")));
