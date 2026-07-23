/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5271: pure-value USB SETUP PID constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_pid_setup_u(void);
 *     - Return the USB token PID for SETUP (0x2d).
 *   uint32_t __gj_usb_pid_setup_u  (alias)
 *   __libcgj_batch5271_marker = "libcgj-batch5271"
 *
 * Exclusive continuum CREATE-ONLY (5271-5280: USB unique —
 * usb_pid_setup_u, usb_pid_in_u, usb_pid_out_u, usb_pid_data0_u,
 * usb_pid_data1_u, usb_pid_ack_u, usb_pid_nak_u, usb_pid_stall_u,
 * usb_ep0_max_u, batch_id_5280). Unique gj_usb_pid_setup_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5271_marker[] = "libcgj-batch5271";

/* USB token PID SETUP. */
#define B5271_PID_SETUP  ((uint32_t)0x2du)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5271_pid_setup(void)
{
	return B5271_PID_SETUP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_pid_setup_u - USB SETUP token PID constant 0x2d.
 *
 * Always returns 0x2d. Self-contained; no parent wires.
 */
uint32_t
gj_usb_pid_setup_u(void)
{
	(void)NULL;
	return b5271_pid_setup();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_pid_setup_u(void)
    __attribute__((alias("gj_usb_pid_setup_u")));
