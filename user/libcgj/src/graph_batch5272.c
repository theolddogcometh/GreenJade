/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5272: pure-value USB IN PID constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_pid_in_u(void);
 *     - Return the USB token PID for IN (0x69).
 *   uint32_t __gj_usb_pid_in_u  (alias)
 *   __libcgj_batch5272_marker = "libcgj-batch5272"
 *
 * Exclusive continuum CREATE-ONLY (5271-5280: USB unique —
 * usb_pid_setup_u, usb_pid_in_u, usb_pid_out_u, usb_pid_data0_u,
 * usb_pid_data1_u, usb_pid_ack_u, usb_pid_nak_u, usb_pid_stall_u,
 * usb_ep0_max_u, batch_id_5280). Unique gj_usb_pid_in_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5272_marker[] = "libcgj-batch5272";

/* USB token PID IN. */
#define B5272_PID_IN  ((uint32_t)0x69u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5272_pid_in(void)
{
	return B5272_PID_IN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_pid_in_u - USB IN token PID constant 0x69.
 *
 * Always returns 0x69. Self-contained; no parent wires.
 */
uint32_t
gj_usb_pid_in_u(void)
{
	(void)NULL;
	return b5272_pid_in();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_pid_in_u(void)
    __attribute__((alias("gj_usb_pid_in_u")));
