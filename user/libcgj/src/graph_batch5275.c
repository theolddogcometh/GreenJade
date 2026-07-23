/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5275: pure-value USB DATA1 PID constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_pid_data1_u(void);
 *     - Return the USB data PID for DATA1 (0x4b).
 *   uint32_t __gj_usb_pid_data1_u  (alias)
 *   __libcgj_batch5275_marker = "libcgj-batch5275"
 *
 * Exclusive continuum CREATE-ONLY (5271-5280: USB unique —
 * usb_pid_setup_u, usb_pid_in_u, usb_pid_out_u, usb_pid_data0_u,
 * usb_pid_data1_u, usb_pid_ack_u, usb_pid_nak_u, usb_pid_stall_u,
 * usb_ep0_max_u, batch_id_5280). Unique gj_usb_pid_data1_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5275_marker[] = "libcgj-batch5275";

/* USB data PID DATA1. */
#define B5275_PID_DATA1  ((uint32_t)0x4bu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5275_pid_data1(void)
{
	return B5275_PID_DATA1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_pid_data1_u - USB DATA1 PID constant 0x4b.
 *
 * Always returns 0x4b. Self-contained; no parent wires.
 */
uint32_t
gj_usb_pid_data1_u(void)
{
	(void)NULL;
	return b5275_pid_data1();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_pid_data1_u(void)
    __attribute__((alias("gj_usb_pid_data1_u")));
