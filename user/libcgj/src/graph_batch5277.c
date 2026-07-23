/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5277: pure-value USB NAK PID constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_pid_nak_u(void);
 *     - Return the USB handshake PID for NAK (0x5a).
 *   uint32_t __gj_usb_pid_nak_u  (alias)
 *   __libcgj_batch5277_marker = "libcgj-batch5277"
 *
 * Exclusive continuum CREATE-ONLY (5271-5280: USB unique —
 * usb_pid_setup_u, usb_pid_in_u, usb_pid_out_u, usb_pid_data0_u,
 * usb_pid_data1_u, usb_pid_ack_u, usb_pid_nak_u, usb_pid_stall_u,
 * usb_ep0_max_u, batch_id_5280). Unique gj_usb_pid_nak_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5277_marker[] = "libcgj-batch5277";

/* USB handshake PID NAK. */
#define B5277_PID_NAK  ((uint32_t)0x5au)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5277_pid_nak(void)
{
	return B5277_PID_NAK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_pid_nak_u - USB NAK handshake PID constant 0x5a.
 *
 * Always returns 0x5a. Self-contained; no parent wires.
 */
uint32_t
gj_usb_pid_nak_u(void)
{
	(void)NULL;
	return b5277_pid_nak();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_pid_nak_u(void)
    __attribute__((alias("gj_usb_pid_nak_u")));
