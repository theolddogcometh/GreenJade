/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5274: pure-value USB DATA0 PID constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_pid_data0_u(void);
 *     - Return the USB data PID for DATA0 (0xc3).
 *   uint32_t __gj_usb_pid_data0_u  (alias)
 *   __libcgj_batch5274_marker = "libcgj-batch5274"
 *
 * Exclusive continuum CREATE-ONLY (5271-5280: USB unique —
 * usb_pid_setup_u, usb_pid_in_u, usb_pid_out_u, usb_pid_data0_u,
 * usb_pid_data1_u, usb_pid_ack_u, usb_pid_nak_u, usb_pid_stall_u,
 * usb_ep0_max_u, batch_id_5280). Unique gj_usb_pid_data0_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5274_marker[] = "libcgj-batch5274";

/* USB data PID DATA0. */
#define B5274_PID_DATA0  ((uint32_t)0xc3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5274_pid_data0(void)
{
	return B5274_PID_DATA0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_pid_data0_u - USB DATA0 PID constant 0xc3.
 *
 * Always returns 0xc3. Self-contained; no parent wires.
 */
uint32_t
gj_usb_pid_data0_u(void)
{
	(void)NULL;
	return b5274_pid_data0();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_pid_data0_u(void)
    __attribute__((alias("gj_usb_pid_data0_u")));
