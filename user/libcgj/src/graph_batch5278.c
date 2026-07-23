/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5278: pure-value USB STALL PID constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_pid_stall_u(void);
 *     - Return the USB handshake PID for STALL (0x1e).
 *   uint32_t __gj_usb_pid_stall_u  (alias)
 *   __libcgj_batch5278_marker = "libcgj-batch5278"
 *
 * Exclusive continuum CREATE-ONLY (5271-5280: USB unique —
 * usb_pid_setup_u, usb_pid_in_u, usb_pid_out_u, usb_pid_data0_u,
 * usb_pid_data1_u, usb_pid_ack_u, usb_pid_nak_u, usb_pid_stall_u,
 * usb_ep0_max_u, batch_id_5280). Unique gj_usb_pid_stall_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5278_marker[] = "libcgj-batch5278";

/* USB handshake PID STALL. */
#define B5278_PID_STALL  ((uint32_t)0x1eu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5278_pid_stall(void)
{
	return B5278_PID_STALL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_pid_stall_u - USB STALL handshake PID constant 0x1e.
 *
 * Always returns 0x1e. Self-contained; no parent wires.
 */
uint32_t
gj_usb_pid_stall_u(void)
{
	(void)NULL;
	return b5278_pid_stall();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_pid_stall_u(void)
    __attribute__((alias("gj_usb_pid_stall_u")));
